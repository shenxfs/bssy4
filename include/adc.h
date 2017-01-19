/**
 *@brief ADC接口模块头文件
 *@file adc.h
 *@author shenxf
 *@version v1.0.0
 *@date  2016-11-12
 */ 
#ifndef ADC_H
#define ADC_H
#include <avr/io.h>
#include <stdint.h>

#define CURRENTA10      0x09U   /**<电流测量通道A，Gain=10*/
#define CURRENTB10      0x0DU   /**<电流测量通道B，Gain=10*/
#define CURRENTA200     0x0BU   /**<电流测量通道A，Gain=200*/
#define CURRENTB200     0x0FU   /**<电流测量通道B，Gain=200*/
#define VOLTAGEA        0x04U   /**<电压测量通道A*/
#define VOLTAGEB        0x05U   /**<电压测量通道B*/
#define VBG11           0x1EU   /**<内部VBG通道*/
#define AVCC            0x40U   /**<AVCC基准*/
#define INTREF11        0x80U   /**<1.1V内部基准*/
#define INTREF25        0xC0U   /**<2.56V内部基准*/

/**
 * @brief FIR滤波器延迟结构
 * @struct strfir
 */
typedef struct strfir
{
    int8_t index;/**<延迟数组下标，总是指向前一个输入x(n-1)*/
    int16_t dn[50];/**<延迟数组,存储前50个输入x(n-1),x(n-2),..,x(n-m-1),m=50*/
}fir_t; /**<FIR滤波器延迟结构类型*/   

extern volatile uint8_t adc_timeout;/**<采样时间到标志*/
int16_t get_adc(uint8_t channel);
int16_t fir_filter(fir_t *pd, int16_t xn);
void adc_start(void);
void adc_stop(void);

#endif
