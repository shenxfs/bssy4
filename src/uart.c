/**
 * @brief 串口接口模块
 * @file uart.c
 * @author shenxf 380406785@qq.com
 * @version V1.1.0
 * @date 2016-10-17
 *
 * 串口接口驱动程序，中断接收，查询发送\n
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
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)||defined (__AVR_ATmega164A__) \
            || defined (__AVR_ATmega324A__)||defined (__AVR_ATmega644A__)||defined (__AVR_ATmega1284A__)
#define USART_RX_vect USART0_RX_vect
#elif defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
#define USART_RX_vect USART_RXC_vect
#endif
uint8_t uart_rxbuf[16];      /**<接收循环队列缓冲区*/
volatile uint8_t uart_head;  /**<队头*/
volatile uint8_t uart_end;   /**<队尾*/

/**
 *@brief 中断接收服务程序
 */
ISR(USART_RX_vect)
{
  uint8_t ind;
  ind = uart_end;
  uart_rxbuf[ind] = UDR;
  ind++;
  if(ind >= 16U)
  {
    ind = 0;
  }
  uart_end = ind;
}

/**
 *@brief 清空缓冲区
 *@sa uart_send() 发送一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
 */
void uart_flush(void)
{
  uart_head = 0;
  uart_end = 0;
}

/**
 *@brief 是否已接收到数据
 *@return 0未接收，非零已接收
 *@sa uart_send() 发送一个字符
 *@sa uart_getchar() 接收一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
 */
uint8_t uart_received(void)
{
  return (uint8_t)(uart_head != uart_end);
}

/**
 *@brief 发送FALSH中的字符串
 *@param[in] str 字符串
 *@param[in] n 字符串长度  
 *@sa uart_send() 发送一个字符
 *@sa uart_getchar() 接收一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
 */
void uart_putsn_P(const __flash char str[],uint8_t n)
{
	uint8_t i;
    uint8_t ch;
    for(i = 0;i < n;i++)      
	{
      ch = (uint8_t)str[i];
      if('\n' == ch)
      {
        uart_send('\r');
        uart_send('\n');
      }
      else if('\0' == ch)
      {
        break;
      }
      else
      {
        uart_send(ch);
      }
	}
}

/**
 *@brief 接收一个字符
 *@return  接收的字符 
 *@sa uart_send() 发送一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
*/
uint8_t uart_getchar(void)
{
	uint8_t ret;
	while(uart_head == uart_end)
	{
		__builtin_avr_wdr();
	}
	ret = uart_rxbuf[uart_head];
	uart_head++;
	if(uart_head >= 16U)
	{
		uart_head = 0;
	}
	return ret;
}

/**
 *@brief 接收1-5位十进制数字符串
 *@param[out] str 字符串
 *@return 遇非数字字符返回0，返回不大于5的数字字符串长度
 *@sa uart_send() 发送一个字符
 *@sa uart_getchar() 接收一个字符
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
*/
int8_t uart_getnum(uint8_t str[])
{
	int8_t ret = 0;
	uint8_t ch;
	str[0] = 0;
	ch = uart_getchar();
	while(ch != '\r')
	{
		if((ch >= '0')&&(ch <= '9'))
		{
			if(ret < 5U)
			{
			    str[ret] = ch;
				ret++;
				uart_send(ch);
			}
			else
			{
			    ret = 5U;
			}
		}
		else if(0x08U == ch)
		{
			if(ret >0)
			{
				ret--;
				uart_send(ch);
			}
		}
		else
		{
		  break;
		}
        ch = uart_getchar();
	}
	return ret;
}

/**
 *@brief 串口初始化
 *@param[in] baud 波特率
*/
void uart_init(uint32_t baud)
{
	uint16_t pri;
	if(baud < 19200U)
	{
		pri = (uint16_t)((int16_t)(F_CPU/(16*baud)) - 1);
	}
	else
	{
		pri = (uint16_t)((int16_t)(F_CPU/(8*baud)) - 1);
		UCSRA |= _BV(U2X);
	}		
	UBRRH = (uint8_t)(pri >> 8);
	UBRRL = (uint8_t)pri;
	UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
#if defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
    UCSRC = _BV(URSEL)|_BV(UCSZ1) | _BV(UCSZ0);
#else
    UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
#endif    
}

/**
 *@brief 发送一个字符数据
 *@param byte 预发送的字符
 *@sa uart_getchar() 接收一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
 *@sa uart_write_times() 发送时间参数数据
*/
void uart_send(uint8_t byte)
{
	while(_BV(UDRE) != (UCSRA & _BV(UDRE)))
	{
	  __builtin_avr_nop();
	} 
	UDR = byte;
}

/**
 *@brief 发送时间参数数据，按“＃.####“
 *@param num 时间参数，单位0.1ms
 *@sa uart_send() 发送一个字符
 *@sa uart_getchar() 接收一个字符
 *@sa uart_getnum()  接收数字字符串
 *@sa uart_putsn_P() 发送FLASH的字符串
 *@sa uart_flush() 清空接收缓冲区
 *@sa uart_received() 是否已接收了数据／字符
*/
void uart_write_times(uint32_t num)
{
  uint8_t str[6];
  uint8_t i,dgt;
  uint32_t n;
  for(i = 0;i < 6;i++)
  {
    str[i] = '0';
  }
  str[1] = '.';
  n = num;
  if(n < 100000UL)
  {
    i = 5U;
    while(n!= 0)
    {
      dgt = (uint8_t)(n % 10U);
      str[i] += dgt;
      n /= 10U;
      i--;
      if(i == 1U)
      {
        break;
      }
    }
    str[0] += (uint8_t)(n % 10U);
    for(i =0;i < 6;i++)
    {
       uart_send(str[i]);
    }
  }
}

/***/
void uart_printnumx(uint16_t num)
{
    uint8_t ch[5],i;
    ch[4] = '\0'; 
    snprintf((char*)ch,5,"%04X",num);
    for(i = 0;i<4;i++)
    {
        uart_send(ch[i]);
    }
}
