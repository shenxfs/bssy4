#include <avr/interrupt.h>
#include <util/delay.h>
#include "adc.h"
#include "measure.h"

fir_t cur1;

#if DEBUG == 1
#include "uart.h"
#endif

int main(void)
{
//    int16_t val;
    volatile int16_t fir;
    uint8_t i = 0;
#if DEBUG == 1
    uart_init(57600UL);
#endif
    DDRB |= _BV(PB7);
    PORTB &= ~_BV(PB7);
    sei();
 //   val = get_adc(AVCC|CURRENTA10);
    while(1)
    {
        //val = get_adc(INTREF25|CURRENTA10);
        fir = get_voltag_filter(INTREF25|VOLTAGEA);
#if DEBUG == 1
        uart_printnumx(i);
        uart_send(',');
        uart_printnumx(fir);
        uart_send('\r');
        uart_send('\n');
#endif
        _delay_ms(1000);
        PORTB ^= _BV(PB7);
        i++;
    }
    return 0;
}
