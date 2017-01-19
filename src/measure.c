/**
 * @brief 测量管理
 * @author shenxf 
 * @file measure.c
 * @version v1.0.0
 * @date 2016-11-12
 */ 
#include <util/delay.h>
#include "measure.h"
#include "adc.h"

#define CURCOEF200  (0x6666) /**<Gain=200、Vref=2.56V时,电流系数12.8mA,Q=11定点数*/
#define CURCOEF10   (0x4000) /**<Gain=10、Vref=2.56V时,电流系数256mA,Q=6定点数*/
#define VOLCOEF     (0x4147) /**<Gain=1、Vref=2.56V,n=15时,电压系数130.56V,Q=7定点数*/
#define CURTH       (0x7800) /**<电流阈值15mA(Q=11)*/
#define CURRMAX     (0x5000) /**<Gain=200、Vref=2.56V时,电流最大值10mA,Q=11定点数*/
#define CURRDIFF    (0x0800) /**<Gain=200、Vref=2.56V时,电流最大差值1mA,Q=11定点数*/
#define VOLMAX      (0x3C00) /**<绝缘测试电压最大值，120V，Q=7定点数*/
#define VOLMIN      (0x2800) /**<绝缘测试电压最小值，120V，Q=7定点数*/
#define QABS(a)     (((a)>=0)?(a):-(a)) /**<求a的绝对值*/

fir_t current;/**<測电流时FIR滤波延迟结构数据*/
fir_t voltag;/**<測电压时FIR滤波延迟结构数据*/
volatile uint8_t prekey;/**<开关状态*/

void init_port (void) __attribute__ ((naked)) __attribute__ ((section (".init1")));
/**
 * @brief IO口初始化，系统上电自动调用
 */
void init_port (void)
{
    KEY_PORT |= KEY_MASK;
    KEY_DDR &= ~KEY_MASK;
    LED_PORT &= ~LED_MASK;
    LED_DDR |= LED_MASK;
    K3022_PORT &= ~_BV(K3022_ON);
    K3022_DDR |= _BV(K3022_ON);
    MODE_PORT &= ~MODE_MASK;
    MODE_DDR |= MODE_MASK;
    prekey = KEY_MASK;
}

/**
 * @brief FIR滤波电流测量
 * @param[in] ch 电流采集通道(差分)
 * @param[out] q Q值
 * @return 电流值 Q定点数，Q=6或11,由q参数指定，单位mA 
 */
int16_t get_current_filter(uint8_t ch,uint8_t *q)
{
    uint8_t i;
    int16_t ret;
    int32_t sum;
    for(i = 0;i < 50U;i++)
    {
        current.dn[i] = 0;
    }
    current.index = 0;
    adc_start();
    for(i = 0;i<100;i++)
    {
        while(0 == adc_timeout)
        {
            __asm__ __volatile__("nop");
        }
        ret = get_adc(ch);
        ret = fir_filter(&current,ret);
        adc_timeout = 0;
    }
    ret = QABS(ret);
    if(CURTH < ret)
    {
        for(i = 0;i < 50U;i++)
        {
            current.dn[i] = 0;
        }
        current.index = 0;
        for(i = 0;i<100U;i++)
        {
            while(0 == adc_timeout)
            {
                __asm__ __volatile__("nop");
            }
            ret = get_adc(ch-2);
            ret = fir_filter(&current,ret);
            adc_timeout = 0;
        }
        ret = QABS(ret);
        sum = (int32_t)ret*CURCOEF10;
        *q = 6U;
    }
    else
    {
        sum = (int32_t)ret*CURCOEF200;
        *q = 11U;
    }
    adc_stop();
    return (int16_t)(sum/512L);
}

/**
 * @brief FIR滤波电压测量
 * @fn int16_t get_voltag_filter(uint8_t ch)
 * @param[in] ch 电压采集通道
 * @return 电压值 Q=7定点数表示范围(-256~255.9921875）V，实际电压测量范围(0~130.427V)\n
 * 由宏VOLCOEF决定，1023/1024*16711/128=130.427
 * @sa VOLCOEF 
 */
int16_t get_voltag_filter(uint8_t ch)
{
    uint8_t i;
    int16_t ret;
    int32_t sum;
    for(i = 0;i < 50U;i++)
    {
        voltag.dn[i] = 0;
    }
    voltag.index = 0;
    adc_start();
    for(i=0;i < 100U;i++)
    {
        while(0 == adc_timeout)
        {
            __asm__ __volatile__("nop");
        }
        ret = get_adc(ch);
        ret = fir_filter(&voltag,ret);
        adc_timeout = 0;        
    }
    ret = QABS(ret);
    sum = (int32_t)ret*VOLCOEF;
    adc_stop();
    return (int16_t)(sum/1024);
}

/**
 * @brief ADC测式
 * @sa timer_test()
 * 用内部2.56V基准采集VBG(1.2V或1.1V)电压，如果得到400~520的转换值(AD),可判断ADC工作正常
 */
uint8_t adc_test(void)
{
    uint8_t ret,i;
    int16_t val;
    for(i = 0;i < 50U;i++)
    {
        voltag.dn[i] = 0;
    }
    voltag.index = 0;
    for(i = 0;i < 100U;i++)
    {
        val = get_adc(INTREF25|VBG11);
        fir_filter(&voltag,val);
        _delay_ms(1);
    }
    
    if((val > 400) && (val < 520))
    {
        ret = NO_ERROR;
    }
    else
    {
        ret = ADC_ERROR;
    }
    return ret;
}

/**
 * @brief 定时器测式
 * @sa adc_start()
 * @sa adc_stop()
 * @sa adc_test()
 * 
 */
uint8_t timer_test(void)
{
    uint8_t ret;
    adc_start();
    _delay_ms(2);
    ret = adc_timeout;
    adc_stop();
    if(ret != 0)
    {
        ret = NO_ERROR;
    }
    else
    {
        ret = TIMER_ERROR;
    }
    return ret;
}

/**
 * @brief 短路电流测试
 * @return 错误标识 
 * 
 */
uint8_t current_test(void)
{
    uint8_t ret,qa,qb;
    int16_t va,vb;
    va = get_current_filter(INTREF25|CURRENTA200,&qa);
    vb = get_current_filter(INTREF25|CURRENTB200,&qb);
    if(qa == qb)
    {
        if(qa == 6U)
        {
            ret = CURRENT_OVER;
        }
        else
        {
            vb -=  va;
            if(QABS(vb) > CURRDIFF)
            {
                if((va < CURRMAX) && (va > 20))
                {
                    ret = NO_ERROR;
                }
                else if(((va+vb) > CURRMAX) && ((va+vb) > 20))
                {
                    ret = NO_ERROR;
                }
                else if((va <= 20)&&((va+vb) > 20))
                {
                    ret = CUR_NC;
                }
                else
                {
                    ret = CURRENT_OVER;
                }
            }
            else
            {
                va =va - vb/2;
                if(va < CURRMAX)
                {
                    ret = NO_ERROR;
                }
                else
                {
                    ret = CURRENT_OVER;
                }
            }
        }
    }
    else 
    {
        if(qa == 11)
        {
            if((va < CURRMAX) && (va > 20))
            {
                ret = NO_ERROR;
            }
            else
            {
                ret = CURRENT_OVER;
            }
        }
        else
        {
            if((vb < CURRMAX) && (vb > 20))
            {
                ret = NO_ERROR;
            }
            else
            {
                ret = CURRENT_OVER;
            }                        
        }
    }
    return ret;
}

/**
 * @brief 电压测试
 */
uint8_t voltag_test(void)
{
    uint8_t ret;
    int16_t va,vb;
    va = get_voltag_filter(INTREF25|VOLTAGEA); 
    vb = get_voltag_filter(INTREF25|VOLTAGEB);
    vb = vb -va;
    if(QABS(vb) >= (VOLMAX - VOLMIN))
    {
        if((va > VOLMIN) && (va < VOLMAX))
        {
            ret = NO_ERROR;
        }
        else if(((va+vb) > VOLMIN) && ((va+vb) < VOLMAX))
        {
            ret = NO_ERROR;
        }
        else
        {
            ret = HV_OVER;
        }
    }
    else
    {
        va = va +vb/2;
        if((va > VOLMIN) && (va < VOLMAX))
        {
            ret = NO_ERROR;
        }
        else if((va < 20) && ((va + vb)< 20))
        {
            ret = HV_NC;
        }
        else
        {
            ret = HV_OVER;
        }
    }
    return ret;
}

/**
 * @brief 获取开关状态
 * @return 开关键值码
 */
enmkey_t key_scan(void)
{
    enmkey_t ret = NOKEY;
    uint8_t ind = 0,key,tmp;
    key = KEY_PIN & KEY_MASK;
    if((KEY_MASK != key) && (prekey != key))
    {
        while(ind < 20U)
        {
            _delay_ms(10);
            tmp = KEY_PIN & KEY_MASK;
            if(key == tmp)
            {
                ind++;
            }
            else
            {
                break;
            }            
        }
        if(ind >= 20U)
        {
            prekey = key;
            key ^= KEY_MASK;
            key &= KEY_MASK;
            if(_BV(KEY1) == (key & _BV(KEY1)))
            {
                ret = K1;
            }
            else if(_BV(KEY2) == (key & _BV(KEY2)))
            {
                ret = K2;
            }
            else if(_BV(KEY3) == (key & _BV(KEY3)))
            {
                ret = K3;
            }
            else if(_BV(KEY4) == (key & _BV(KEY4)))
            {
                ret = K4;
            }
            else if(_BV(KEY5) == (key & _BV(KEY5))) 
            {            
                ret = K5;
            }
            else 
            {
                ret = K6;
            }
        }
    }
    return ret;
}
