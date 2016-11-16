/**
 * @brief 测量管理
 * @author shenxf 
 * @file measuer.c
 * @version v1.0.0
 * @date 2016-11-12
 */ 
#include "measure.h"
#include "adc.h"

#define CURCOEF200  (0x6666) /**<Gain=200、Vref=2.56V时,电流系数12.8mA,Q=11定点数*/
#define CURCOEF10   (0x4000) /**<Gain=10、Vref=2.56V时,电流系数256mA,Q=6定点数*/
#define VOLCOEF     (0x4147) /**<Gain=1、Vref=2.56V,n=15时,电压系数130.56V,Q=7定点数*/
#define CURTH       (0x7800) /**<电流阈值15mA(Q=11)*/
#define QABS(a)     ((a>=0)?a:-a)

fir_t current,voltag;
void init_port (void) __attribute__ ((naked)) __attribute__ ((section (".init1")));
void init_port (void)
{
    KEY_PORT |= KEY_MASK;
    KEY_DDR &= ~KEY_MASK;
    LED_PORT &= ~LED_MASK;
    LED_DDR |= LED_MASK;
    K3022_PORT &= ~K3022_MASK;
    K3022_DDR |= K3022_MASK;
    QJ41A_PORT &= ~_BV(QJ41A_CTRL);
    QJ41A_DDR |= _BV(QJ41A_CTRL);
    MODE_PORT &= ~MODE_MASK;
    MODE_DDR |= MODE_MASK;
}

/**
 * @brief 电流测量
 * @param[in] ch 电流采集通道(差分),Gain=200
 * @param[out] q Q值
 * @retval 电流
 */
int16_t get_current_filt(uint8_t ch,uint8_t *q)
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
 * @brief 电压测量
 * @param [in]ch 电压采集通道
 * @retval 电压值 Q=7定点数，表示范围(-256~255.9921875）V，实际电压测量范围(0~130.427V)由@var VOLCOEF决定，1023/1024*16711/128=130.427
 * @details 
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
