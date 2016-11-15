/**
 * @brief 测量管理头文件
 * @author shenxf 
 * @file measuer.h
 * @version v1.0.0
 * @date 2016-11-12
 */ 
#ifndef MEASURE_H
#define MEASURE_H
#include <avr/io.h>
#include <stdint.h>
/*开关接口定义*/
#define KEY_PORT    PORTD /**<开关接口输出*/
#define KEY_DDR     DDRD /**<开关接口方向*/
#define KEY_PIN     PIND /**<开关接口输入*/
#define KEY1        PD7 /**<开关1位*/
#define KEY2        PD6 /**<开关2位*/
#define KEY3        PD5 /**<开关3位*/
#define KEY4        PD4 /**<开关4位*/
#define KEY5        PD3 /**<开关5位*/
#define KEY6        PD2 /**<开关6位*/
/**开关接口屏蔽字，用于整体操作*/
#define KEY_MASK    (_BV(KEY1)|_BV(KEY2)|_BV(KEY3)|_BV(KEY4)|_BV(KEY5)|_BV(KEY6))

/*指示灯接口定义*/
#define LED_PORT    PORTB /**<指示灯接口输出*/
#define LED_DDR     DDRB /**<指示灯接口方向*/
#define LED_GREEN   PB2 /**<红指示灯位*/
#define LED_RED     PB1 /**<绿指示灯位*/
#define LED_MASK    (_BV(LED_GREEN)|_BV(LED_RED)) /**指示灯接口屏蔽字，用于整体操作*/

/*绝缘电阻测量仪K3022控制接口定义*/
#define K3022_PORT  PORTB /**<绝缘电阻测量仪K3022控制接口输出*/
#define K3022_DDR   DDRB /**<绝缘电阻测量仪K3022控制接口方向*/
#define K3022_ON    PB3 /**<控制开*/
#define K3022_OFF   PB4 /**<控制关*/
/**绝缘电阻测量仪K3022控制接口屏蔽字，用于整体操作*/
#define K3022_MASK  (_BV(K3022_ON)|_BV(K3022_OFF))

/*电桥电阻测量仪QJ41A控制接口定义*/
#define QJ41A_PORT  PORTB /**<电桥电阻测量仪QJ41A控制接口输出*/
#define QJ41A_DDR   DDRB /**<电桥电阻测量仪QJ41A控制接口方向*/
#define QJ41A_CTRL  PB0 /**<控制位*/

/*测量模式控制接口定义*/
#define MODE_PORT   PORTC /**<测量模式控制接口输出*/
#define MODE_DDR    DDRC /**<测量模式控制接口方向*/
#define MODE_QJA    PC0  /**<电桥电阻测量模式1*/   
#define MODE_QJB    PC1 /**<电桥电阻测量模式2*/
#define MODE_BRA    PC2 /**<绝缘电阻测量模式1*/
#define MODE_BRB    PC3 /**<绝缘电阻测量模式2*/
#define MODE_PC4    PC4 /**<电桥电阻测试电流检测模式*/
#define MODE_HV     PC5 /**<绝缘电阻表校准模式*/
#define MODE_CNT    PC6 /**<电桥电阻表校准模式*/
#define MODE_PC7    PC7 /**<绝缘电阻表测试电压检测模式*/
/**测量模式控制接口屏蔽字，用于整体操作*/
#define MODE_MASK   (_BV(MODE_QJA)|_BV(MODE_QJB)|_BV(MODE_BRA)|_BV(MODE_BRB)|_BV(MODE_PC4)|_BV(MODE_HV)|_BV(MODE_CNT)|_BV(MODE_PC7))

int16_t get_current_filt(uint8_t ch,uint8_t *q);
int16_t get_voltag_filter(uint8_t ch);

#endif
