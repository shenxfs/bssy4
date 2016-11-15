#ifndef ADC_H
#define ADC_H
#include <avr/io.h>
#include <stdint.h>

#define CURRENTA10      0x09U
#define CURRENTB10      0x0DU
#define CURRENTA200     0x0BU
#define CURRENTB200     0x0FU
#define VOLTAGEA        0x04U
#define VOLTAGEB        0x05U
#define VBG11           0x1EU
#define AVCC            0x40U
#define INTREF11        0x80U
#define INTREF25        0xC0U

/**
 * @struct FIR滤波器延迟结构
 */
typedef struct strfir
{
    int8_t index;/**<延迟数组下标，总是指向前一个输入x(n-1)*/
    int16_t dn[50];/**<延迟数组,存储前50个输入x(n-1),x(n-2),..,x(n-m-1),m=50*/
}fir_t;    

extern volatile uint8_t adc_timeout;/**<采样时间到标志*/
int16_t get_adc(uint8_t channel);
int16_t fir_filter(fir_t *pd, int16_t xn);
void adc_start(void);
void adc_stop(void);

#endif
