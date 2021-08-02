/**
 * @file main.c
 * @author Bartosz Szpila
 * @brief Sample code, used for toolchain test
 * @details LED light - intesivity controlled with potentiometer
 * @version filler
 * @date 2021-08-02
 *
 * @copyright Copyright (c) 2021
 */

#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
#include <util/delay.h>

#define LED         PB4
#define LED_DDR     DDRB
#define LED_PORT    PORTB

#define MS_CYCLE    4
#define TIME        210

#define BOOST       40

#define _MICRO_STEP 10
#define _DELAY_US(delay)                                        \
    {                                                           \
        for (uint32_t _i = 0; _i < (delay) / _MICRO_STEP; _i++) \
            _delay_us (_MICRO_STEP);                            \
    }

#define BAUD       9600
#define UBRR_VALUE ((F_CPU) / 16 / (BAUD)-1)

/**
 * @brief Initalize uart
 */
void uart_init (void)
{
    UBRR0  = UBRR_VALUE;
    UCSR0B = _BV (RXEN0) | _BV (TXEN0);
    UCSR0C = _BV (UCSZ00) | _BV (UCSZ01);
}

/**
 * @brief Transmit character through stream
 *
 * @param[in] data character to send
 * @param[in] stream io stream to transmit through
 * @return success status
 */
int uart_transmit (char data, FILE *stream)
{
    while (!(UCSR0A & _BV (UDRE0)))
        ;
    UDR0 = data;
    return 0;
}

/**
 * @brief Receive character from stream
 *
 * @param[in] stream io stream to receive from
 * @return int received character
 */
int uart_receive (FILE *stream)
{
    // czekaj aż znak dostępny
    while (!(UCSR0A & _BV (RXC0)))
        ;
    return UDR0;
}
FILE uart_file;

/**
 * @brief Initalize ADC
 */
void adc_init (void)
{
    /* reference set to Vcc, channel ADC0 */
    ADMUX  = _BV (REFS0);
    DIDR0  = _BV (ADC0D);

    /* ADC frequency is 125 kHz (16 MHz / 128) */
    ADCSRA = _BV (ADPS0) | _BV (ADPS1) | _BV (ADPS2);

    /* turn on ADC */
    ADCSRA |= _BV (ADEN);
}

int main ()
{
    uint32_t val, light_time;

    /* Initalization */
    uart_init ();
    fdev_setup_stream (&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    adc_init ();
    LED_DDR |= _BV (LED);

    /* Main loop */
    while (1)
    {
        ADCSRA |= _BV (ADSC);
        /* Wait for result */
        while (!(ADCSRA & _BV (ADIF)))
            ;
        ADCSRA |= _BV (ADIF);

        val        = ADC;
        light_time = ((val * val) >> 12) - (val >> 7);
        printf ("%d  ", val);
        printf ("%d\r\n", light_time);
        for (int i = 0; i < MS_CYCLE; i++)
        {
            LED_PORT |= _BV (LED);
            _DELAY_US (light_time);

            LED_PORT &= ~_BV (LED);
            _DELAY_US (TIME - light_time);
        }
    }
}
