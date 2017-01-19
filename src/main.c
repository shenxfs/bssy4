/**
 *@brief 某测试控制系统
 *@file main.c
 *@author shenxf
 *@version V1.0.0
 *@date 2016-11-17
 *@details  测试控制系统
 *
 * 
*/
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "adc.h"
#include "measure.h"

#if DEBUG == 1
#include "uart.h"
#endif

int main(void)
{
#if DEBUG == 1
    int16_t tmp;
    uint8_t q;
#endif
    uint8_t errors;
    enmkey_t sky;
    sei();
    errors = adc_test();
    errors |= timer_test();
    K3022_PORT &= ~_BV(K3022_ON);
    MODE_PORT &= ~_BV(MODE_QJPWR);
    _delay_ms(1000);
    MODE_PORT |= _BV(MODE_QJPWR);
#if DEBUG == 1
    uart_init(57600UL);
    DDRB |= _BV(PB7);
    PORTB &= ~_BV(PB7);
#endif
    while(1)
    {
        sky = key_scan();
#if DEBUG == 1
        tmp = get_voltag_filter(INTREF25|VOLTAGEA);
        uart_printnumx(sky);
        uart_send(',');
        uart_printnumx(tmp);
        q = 11;
        tmp = get_current_filter(INTREF25|CURRENTA200,&q);
        uart_send(',');
        uart_printnumx(tmp);
        q = 6;
        tmp = get_current_filter(INTREF25|CURRENTA10,&q);
        uart_send(',');
        uart_printnumx(tmp);
        tmp = get_adc(INTREF25|VBG11);
        uart_send(',');
        uart_printnumx(tmp);
        uart_send('\r');
        uart_send('\n');
        _delay_ms(1000);
        PORTB ^= _BV(PB7);
#endif
        if(sky!= NOKEY)
        {
            MODE_PORT &= _BV(MODE_QJPWR);
            K3022_PORT &= ~_BV(K3022_ON);
            if(sky == K1)/*自检*/
            {
                MODE_PORT |= _BV(MODE_CNT);
                errors |= current_test();
                MODE_PORT &= ~_BV(MODE_CNT);
                MODE_PORT |= _BV(MODE_HVT);
                _delay_ms(100);
                K3022_PORT |= _BV(K3022_ON);
                _delay_ms(1000);
                errors |= voltag_test();
                K3022_PORT &= ~_BV(K3022_ON);
                MODE_PORT |= _BV(MODE_HVT);
                /**/
                if(0 == errors)
                {
                    LED_PORT |= _BV(LED_GREEN);
                    LED_PORT &= ~_BV(LED_RED);
                }
                else
                {
                    LED_PORT &= ~_BV(LED_GREEN);
                    LED_PORT |= _BV(LED_RED);
                }
            }
            else if(sky == K2)/*电桥电阻测试1*/
            {
                MODE_PORT |= _BV(MODE_CNA);
            }
            else if(sky == K3)/*电桥电阻测试2*/
            {
                MODE_PORT |= _BV(MODE_CNB);
            }
            else if(sky == K4)/*绝缘电阻测试*/
            {
                MODE_PORT |= _BV(MODE_HV);
                _delay_ms(100);
                K3022_PORT |= _BV(K3022_ON);
                _delay_ms(2000);
                K3022_PORT &= ~_BV(K3022_ON);
            }
            else if(sky == K5)/*电桥电阻表校准*/
            {
                MODE_PORT |= _BV(MODE_CNCK);
            }
            else if(sky == K6)/*绝缘电阻表校准*/
            {
                MODE_PORT |= _BV(MODE_HVCK);
                _delay_ms(100);
                K3022_PORT |= _BV(K3022_ON);
                _delay_ms(2000);
                K3022_PORT &= ~_BV(K3022_ON);
            }
        }
        //val = get_adc(INTREF25|CURRENTA10);
    }
    return 0;
}
