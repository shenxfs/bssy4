/**
 * @brief 串口接口模块头文件
 * @file uart.h
 * @author shenxf 380406785@qq.com
 * @version V1.1.0
 * @date 2016-10-17
 *
 * 串口接口常数定义与函数声明\n
 * 函数列表：
 *@sa uart_init() 初始化
 *@sa uart_send() 发送一个字符
 *@sa uart_getchar() 接收一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
 */
#ifndef UART_H
#define UART_H
#include <avr/io.h>

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)||defined (__AVR_ATmega164A__) \
            ||defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284__)
#define UBRRH 	UBRR0H  /**<usart波特率寄存器高八位*/
#define UBRRL 	UBRR0L  /**<usart波特率寄存器低八位*/
#define UDR		UDR0    /**<usart数据接收/发送寄存器*/
#define UCSRA	UCSR0A  /**<usart控制寄存器A*/
#define UCSRB	UCSR0B  /**<usart控制寄存器B*/
#define UCSRC	UCSR0C  /**<usart控制寄存器C*/
#define UDRE	UDRE0   /**<usart控制寄存器A,UDRE位*/
#define RXC		RXC0    /**<usart控制寄存器A,RXC位*/
#define FE		FE0     /**<usart控制寄存器A,FE位*/
#define DOR		DOR0    /**<usart控制寄存器A,DOR位*/
#define U2X		U2X0    /**<usart控制寄存器A,U2X位*/
#define RXEN	RXEN0   /**<usart控制寄存器B,RXEN位*/
#define RXCIE	RXCIE0   /**<usart控制寄存器B,RXCIE位*/
#define TXEN	TXEN0   /**<usart控制寄存器B,TXEN位*/
#define UCSZ1	UCSZ01  /**<usart控制寄存器C,UCSZ1位*/
#define UCSZ0	UCSZ00  /**<usart控制寄存器C,UCSZ0位*/
#define USBS	USBS0   /**<usart控制寄存器C,USBS位*/
#endif
void uart_send(uint8_t byte);
void uart_init(uint32_t baud);
uint8_t uart_getchar(void);
int8_t uart_getnum(uint8_t str[]);
void uart_putsn(char str[],uint8_t n);
void uart_putsn_P(const __flash char str[],uint8_t n);
void uart_flush(void);
uint8_t uart_received(void);
void uart_write_times(uint32_t num);
void uart_printnumx(uint16_t num);
#endif
