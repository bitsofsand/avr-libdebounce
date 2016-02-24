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
#include "debounce.h"

int main(void)
{
    /* Replace with your application code */
	

	button_t button_1 = debounce_init("PB0");
	button_t button_2 = debounce_init("PB2");
	
	
	sei();
	
    while (1) 
    {
		
		if(button_check(button_1) == BUTTON_PRESS_SHORT) {
			DDRB |= (1 << PB1);
			PORTB |= (1 << PB1);
				
		}
    }
}

