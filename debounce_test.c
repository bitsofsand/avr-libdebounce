#define F_CPU 8000000

#include "debounce.h"
#include "serial.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

void exit_error(char *message)
{

	serial_send_data(message);
	_delay_ms(1000); // Time to write out message
	exit(1);

}

int main(void)
{

	struct debounce_button *button_1;
	struct debounce_button *button_2;

	char *pin_1 = malloc(4);
	char *pin_2 = malloc(4);
	strcpy(pin_1, "PB0");
	strcpy(pin_2, "PB2");

	serial_initialise();
    PORTB |=(1 << PB5 | 1 << PB4 | 1 << PB3); // pullups on non used pins

	// Setup button 1
	if ((button_1 = malloc(sizeof(struct debounce_button))) == NULL) 
		exit_error("Oops malloc button_1");
	button_1->button_pin=pin_1;
	button_1->auto_acknowledge_button=0;
	if ((button_1 = debounce_init(button_1)) == NULL) 
		exit_error("Oops init button_1");

	// Setup button 2
	if ((button_2 = malloc(sizeof(struct debounce_button))) == NULL) 
		exit_error("Oops malloc button_2");
	button_2->button_pin=pin_2;
	button_2->auto_acknowledge_button=1;
	if ((button_2 = debounce_init(button_2)) == NULL) 
		exit_error("Oops init button_1");

	serial_send_data("Ready. Start pressing buttons");

	while (1) {

		switch(button_check(button_1)) {

			case BUTTON_PRESS_NONE:
				break;
		
			case BUTTON_PRESS_SHORT:
				serial_send_data("Button 1 short");
				button_acknowledge(button_1);
				break;

			case BUTTON_PRESS_LONG:
				serial_send_data("Button 1 long. I will not ack this one");
				// Not acknowledging -> no more button 1 presses should register
				break;

		}

		switch(button_check(button_2)) {

			case BUTTON_PRESS_NONE:
				break;
		
			case BUTTON_PRESS_SHORT:
				serial_send_data("Button 2 short");
				break;

			case BUTTON_PRESS_LONG:
				serial_send_data("Button 2 long");
				break;

		}

		_delay_ms(100);

	}	

		
}
