/*
 * PWM_remote_AS.c
 *
 * Created: 23/02/2016 22:31:50
 * Author : robartes
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "debounce.h"
#include "serial.h"

int main(void)
{
	
	serial_initialise();

	button_t button_1 = debounce_init("PB0");
	button_t button_2 = debounce_init("PB2");
	
	button_auto_acknowledge(button_2);
	
	sei();

	serial_send_data("Off we go\r\n");
	
    while (1) 
    {

		if(button_check(button_1) == BUTTON_PRESS_SHORT) {
			serial_send_data("Button 1 short");
			button_acknowledge(button_1);
		}

		if(button_check(button_1) == BUTTON_PRESS_LONG) {
			serial_send_data("Button 1 long");
			button_acknowledge(button_1);
		}
		
   		if(button_check(button_2) == BUTTON_PRESS_SHORT) {
			serial_send_data("Button 2 short");
		}

		_delay_ms(200);

		serial_send_data("Canary\r\n");

 }
}

