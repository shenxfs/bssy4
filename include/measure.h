/**
 * @brief 测量管理头文件
 * @author shenxf 
 * @file measure.h
 * @version v1.0.0
 * @date 2016-11-12
 */ 
#ifndef MEASURE_H
#define MEASURE_H
#include <avr/io.h>
#include <stdint.h>
/*开关接口定义*/
#define KEY_PORT    PORTD   /**<开关接口输出*/
#define KEY_DDR     DDRD    /**<开关接口方向*/
#define KEY_PIN     PIND    /**<开关接口输入*/
#define KEY1        PD7     /**<开关1位*/
#define KEY2        PD6     /**<开关2位*/
#define KEY3        PD5     /**<开关3位*/
#define KEY4        PD4     /**<开关4位*/
#define KEY5        PD3     /**<开关5位*/
#define KEY6        PD2     /**<开关6位*/
/**开关接口屏蔽字，用于整体操作*/
#define KEY_MASK    (_BV(KEY1)|_BV(KEY2)|_BV(KEY3)|_BV(KEY4)|_BV(KEY5)|_BV(KEY6))

/*指示灯接口定义*/
#define LED_PORT    PORTB   /**<指示灯接口输出*/
#define LED_DDR     DDRB    /**<指示灯接口方向*/
#define LED_GREEN   PB2     /**<P11,红指示灯位*/
#define LED_RED     PB1     /**<P10,绿指示灯位*/
#define LED_MASK    (_BV(LED_GREEN)|_BV(LED_RED)) /**<指示灯接口屏蔽字，用于整体操作*/

/*绝缘电阻测量仪K3022控制接口定义*/
#define K3022_PORT  PORTB /**<绝缘电阻测量仪K3022控制接口输出*/
#define K3022_DDR   DDRB /**<绝缘电阻测量仪K3022控制接口方向*/
#define K3022_ON    PB0 /**<P9,绝缘电阻测量控制按钮*/

/*测量模式控制接口定义*/
#define MODE_PORT   PORTC   /**<测量模式控制接口输出*/
#define MODE_DDR    DDRC    /**<测量模式控制接口方向*/
#define MODE_QJPWR  PC0     /**<P8,电桥电阻测量仪电源控制*/   
#define MODE_HVCK   PC1     /**<P7,绝缘电阻表校准*/
#define MODE_CNCK   PC2     /**<P6,电桥电阻表校准*/
#define MODE_HV     PC3     /**<P5,绝缘电阻测量*/
#define MODE_CNB    PC4     /**<P4,电桥电阻测试B*/
#define MODE_CNA    PC5     /**<P3,电桥电阻测试A*/
#define MODE_HVT    PC6     /**<P2,绝缘电阻表测试电压检测*/
#define MODE_CNT    PC7     /**<P1,电桥电阻表短路电流检测*/
/**测量模式控制接口屏蔽字，用于整体操作*/
#define MODE_MASK   (_BV(MODE_QJPWR)|_BV(MODE_HVCK)|_BV(MODE_CNCK)|_BV(MODE_HV)|_BV(MODE_CNB)|_BV(MODE_CNA)|_BV(MODE_HVT)|_BV(MODE_CNT))

#define NO_ERROR        (0x00U)     /**<无错误*/
#define TIMER_ERROR     (0x01U)     /**<定时器0运行错误*/
#define ADC_ERROR       (0x02U)     /**<ADC运行错误*/
#define CURRENT_OVER    (0x04U)     /**<短路电流超限*/
#define HV_OVER         (0x08U)     /**<绝缘测试电压超限*/
#define CUR_NC          (0x10U)     /**<短路电流测试未连接*/
#define HV_NC           (0x20U)     /**<绝缘测试未连接*/

/**
 * @brief 开关键值
 * @enum key_code
 */
typedef enum key_code
{
    NOKEY = 0,/**<无键*/
    K1 = 1,/**<键1,PD7*/
    K2 = 2,/**<键2,PD6*/
    K3 = 3,/**<键3,PD5*/
    K4 = 4,/**<键4,PD4*/
    K5 = 5,/**<键5,PD3*/
    K6 = 6 /**<键6,PD2*/   
}enmkey_t;/**<开关键值类型*/

int16_t get_current_filter(uint8_t ch,uint8_t *q);
int16_t get_voltag_filter(uint8_t ch);
uint8_t adc_test(void);
uint8_t timer_test(void);
uint8_t current_test(void);
uint8_t voltag_test(void);
enmkey_t key_scan(void);
#endif
