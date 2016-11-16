/**
 *@brief
 *@file
 *@author
 *@version
 *@date 
 */ 
#include "adc.h"
#include <avr/interrupt.h>

#if defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
#define TIMER_COMP_vect TIMER0_COMP_vect
#define OCR OCR0
#elif defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)||defined (__AVR_ATmega164A__) \
            || defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284__)
#define OCR  OCR0A 
#define TIMER_COMP_vect TIMER0_COMPA_vect 
#define TIMSK TIMSK0
#define OCIE0 OCIE0A            
#endif

/**
 *@brief FIR数字滤波器系数
 *
 * FIR数字滤波器:fs=976Hz,fc=16Hz,阶数50,海明窗函数，51个系数Q定点数Q=15,有符号
 * 50Hz衰减-70dB
 */
#if __GNUC__>=5 ||(__GNUC__ == 4 && __GNUC_MINOR__>7)
__flash const  int16_t coef[51] = {
#else
#include <avr/pgmspaces.h>
const  int16_t coef[51] PROGMEM= {
#endif
/* 由MATLAB的滤波设计与分析工具(Filter Design and Analysis Tool)生成的原始数据 
  0.0007310887503072,0.0009205481773948,  0.00122009721501, 0.001668603627278,
    0.00230240338685, 0.003153488848845, 0.004247804301653, 0.005603725180684,
   0.007230791134002, 0.009128753865062,  0.01128698860691,  0.01368430369282,
    0.01628916657764,  0.01906034751282,   0.0219479646191,  0.02489489708998,
    0.02783851743763,  0.03071267975839,   0.0334498895662,  0.03598357233505,
    0.03825035289117,  0.04019225644156,  0.04175874440111,  0.04290850420386,
    0.04361092171639,  0.04384717732457,  0.04361092171639,  0.04290850420386,
    0.04175874440111,  0.04019225644156,  0.03825035289117,  0.03598357233505,
     0.0334498895662,  0.03071267975839,  0.02783851743763,  0.02489489708998,
     0.0219479646191,  0.01906034751282,  0.01628916657764,  0.01368430369282,
    0.01128698860691, 0.009128753865062, 0.007230791134002, 0.005603725180684,
   0.004247804301653, 0.003153488848845,  0.00230240338685, 0.001668603627278,
    0.00122009721501,0.0009205481773948,0.0007310887503072
*/
       25,     30,     40,     55,     75,    103,    139,    184,    237,
      299,    370,    448,    534,    625,    719,    816,    912,   1006,
     1096,   1179,   1253,   1317,   1368,   1406,   1429,   1438,   1429,
     1406,   1368,   1317,   1253,   1179,   1096,   1006,    912,    816,
      719,    625,    534,    448,    370,    299,    237,    184,    139,
      103,     75,     55,     40,     30,     25
      
};

volatile uint8_t adc_timeout;/**<采样时间到标志*/
void init_adc (void) __attribute__ ((naked)) __attribute__ ((section (".init1")));

/**
 * @brief ADC初始化，链接“init1”段，由系统调用
 */
void init_adc(void)
{
    ADCSRA = _BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)
    DIDR0 = 0x3FU;
    DIDR1 = 0x00U;
    ADCSRB = 0;
#elif defined (__AVR_ATmega164A__)|| defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284__)
    DIDR0 = 0x3FU;
    ADCSRB = 0;
#else
    SFIOR = 0;
#endif    
    OCR = (uint8_t)(F_CPU/976/256-1);
    TIMSK |= _BV(OCIE0);
#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)||defined (__AVR_ATmega164A__) \
            || defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284__)
    TCCR0A |= _BV(WGM01);
#elif defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
    TCCR0 |=_BV(WGM01);
#endif    
}

/**
 * @fn ADC转换函数
 * @param[in] channel
 * @retval  转换值
 */
int16_t get_adc(uint8_t channel)
{
    uint16_t ret;
    uint8_t tmp;
    tmp = channel & 0x1fU;
    ADMUX = channel;
    ADCSRA |= _BV(ADSC);
    while(_BV(ADIF) != (ADCSRA & _BV(ADIF)))
    {
        __asm__ __volatile__("nop");
    }
    ret = ADC;
    if(tmp >= 8U && tmp < 0x1CU)
    {
        if(0x200U == (ret & 0x200U))
        {
            ret |= 0xfC00U;
        }
    }
    return (int16_t)ret;
}

/**
 * @brief FIR数字滤波函数
 * @param [in]pd 指向FIR延迟结构
 * @param [in]xn 输入值，任意Q值(0~15)
 * @retval 滤波值 与输入相同的Q值
 * 
 *FIR数字滤波函数，差分方程y(n)=h(0)*x(n)+h(1)*x(n-1)+..+h(m)*x(n-m-1)
 */ 
int16_t fir_filter(fir_t *pd, int16_t xn)
{
    uint32_t sum;;
    int8_t i,ind;
    ind = pd->index;
#if __GNUC__>=5 ||(__GNUC__ == 4 && __GNUC_MINOR__>7)
    sum = (uint32_t)xn*coef[0];
#else
    sum = (uint32_t)xn*((int16_t)pgm_read_word_near(&coef[0]));
#endif
    for(i = 1;i < 51;i++,ind--)
    {
        if(ind < 0)
        {
            ind = 49;
        }
#if __GNUC__>=5 ||(__GNUC__ == 4 && __GNUC_MINOR__>7)
        sum += (uint32_t)pd->dn[ind]*coef[i];
#else
        sum += (uint32_t)pd->dn[ind]*((int16_t)pgm_read_word_near(&coef[i]));
#endif
    }
    ind++;
    pd->index = ind;
    pd->dn[ind] = xn;
    return (uint16_t)(sum/32768U);
}

/**
 * @brief 启动采样周期
 */
void adc_start(void)
{
    TCNT0 = 0;
    adc_timeout = 0;
#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)||defined (__AVR_ATmega164A__) \
            || defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284A__)
    TIFR0 |= _BV(OCF0A);
    TCCR0B |= _BV(CS02);
#elif defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
    TIFR |= _BV(OCF0);
    TCCR0 |=_BV(CS02);
#endif
    
}

/**
 * @brief 关闭采样周期
 */
void adc_stop(void)
{
    TCNT0 = 0;
    adc_timeout = 0;
#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)||defined (__AVR_ATmega164A__) \
            || defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284A__)
    TIFR0 |= _BV(OCF0A);
    TCCR0B &= ~_BV(CS02);
#elif defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
    TIFR |= _BV(OCF0);
    TCCR0 &= ~_BV(CS02);
#endif    
}

/**
 * @brief 定时器0输出比较中断服务
 */
ISR(TIMER_COMP_vect)
{
    adc_timeout =1U;
}

