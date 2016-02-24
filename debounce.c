/*
 * debounce.c
 *
 * Created: 24/02/2016 10:14:18
 * Author : robartes
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include "debounce.h"

typedef enum {
	RETURN_OK,
	RETURN_ERROR,
} return_code_t;

/*********************************************************************
 * File global variables
 *********************************************************************/

struct button {

	struct button *next;
	volatile uint8_t *port;
	uint8_t pin;
	uint8_t current_debounce_count;
	uint8_t short_press;
	uint8_t isr_short_press;
	uint8_t long_press;
	uint8_t auto_acknowledge;
	uint8_t dead_time_counter;
	
};

struct button *button_list_head = NULL;

/*********************************************************************
 * Private functions
 *********************************************************************/

/******************************************************************
 * init_timer: initialise Timer0 for debouncing
 *
 * Timer0 runs in CTC mode with a /1024 prescaler and counts to
 * F_CPU / 10000
 * This results in ISR every 10ms, which will be used to debounce
 ******************************************************************/

static void init_timer(void) 
{

	// CTC mode
	TCCR0A &= ~(1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B &= ~(1 << WGM02);

	OCR0A = OCR_VALUE; 

	// Enable Compare Match interrupt
	TIMSK |= (1 << OCIE0A);

	// Start timer with prescaler /1024
	TCCR0B |= (1 << CS02 | 1 << CS00);
	TCCR0B &= ~(1 << CS01);

}

static return_code_t setup_io(char *button_pin, struct button *button)
{

	// Sanity checks
	if (strlen(button_pin) != 3)
		return RETURN_ERROR;
	if (strstr(button_pin, "B") == NULL)
		return RETURN_ERROR;

	uint8_t pin_number = *(button_pin+2) - 0x30; // 0x30 = ASCII "0"
	if (pin_number > 5)
		return RETURN_ERROR;

	// Set port as input with pullup
	DDRB &= ~(1 << pin_number);
	PORTB |= (1 << pin_number);
	
	// Squirrel away data
	button->port = &PINB;
	button->pin = pin_number; 

	return RETURN_OK;
	
}

static struct button *get_first_button(void)
{

	return button_list_head;

}

static struct button *get_next_button(struct button *button)
{

	return button->next;

}


static struct button *get_last_button(void) 
{

	struct button *tb = get_first_button();
	
	if (tb != NULL) {

		while (tb->next != NULL)
			tb = get_next_button(tb);

	}

	return tb;
	
}

static uint8_t button_is_pressed(struct button *button)
{

	return bit_is_clear(*(button->port), button->pin);

}

static void add_button(struct button *button)

{

	struct button *tb;

	if (button_list_head == NULL) {
		button_list_head = button;
	} else {
		tb = get_last_button();
		tb->next = button;
	}
	
}

/******************************************************************
 * Timer0 compare match interrupt: debounce button press
 *
 * This function does all the actual work of the library. It gets
 * called regularly and checks each button
 ******************************************************************/

ISR(TIM0_COMPA_vect)
{

	struct button *button = get_first_button();

	while(button != NULL) {

		// Don't check this button if not acknowledged yet
		if(button->short_press || button->long_press) {
			button = get_next_button(button);
			continue;
		}		

		// Don't check this button if we are in dead time
		if(button->dead_time_counter) {
			button->dead_time_counter--;
			button = get_next_button(button);
			continue;
		}

		switch (button->current_debounce_count) {

			// No previous presses detected
			case 0:
				if (button_is_pressed(button))
					button->current_debounce_count = 1;
				break;

			// Possible short press
			case DEBOUNCE_COUNT_SHORT:
				if (button_is_pressed(button))
					button->isr_short_press = 1;
				button->current_debounce_count++;
				break;

			// Halfway between possible short and long press
			case DEBOUNCE_COUNT_MID:
				if (button->isr_short_press && ! button_is_pressed(button)) {
					// It's a short press
					button->short_press = 1;
					button->dead_time_counter = DEBOUNCE_DEAD_TIME_SHORT;
					button->isr_short_press = 0;
					button->current_debounce_count = 0;
				} else {
					// Button still pressed - it could be long
					button->current_debounce_count++;
				}
				break;

			// Possible long press
			case DEBOUNCE_COUNT_LONG:
				if (button_is_pressed(button)) {
					// It's a long press
					button->long_press = 1;
					button->dead_time_counter = DEBOUNCE_DEAD_TIME_LONG;
				} else if (button->isr_short_press) {
					// It was a short press after all
					button->short_press = 1;
					button->dead_time_counter = DEBOUNCE_DEAD_TIME_SHORT;
				}
				button->current_debounce_count = 0;
				button->isr_short_press = 0;
				break;

			// All other cases, i.e. > 0 and < LONG_PRESS and not already covered
			default:
				button->current_debounce_count++;
				break;

		}

		button = get_next_button(button);

	}

}

/*********************************************************************
 * Public functions
 *********************************************************************/

/*********************************************************************
 * debounce_init: setup a button for debouncing
 *
 * Parameters
 *		struct debounce_button *db
 *			See struct debounce_button
 * Returns
 *		struct debounce_button *db
 *			Returns the argument, with extra data filled in. Use this
 *			as argument to following function calls
 * 
 * This library expects the button to pull the pin to GND. 
 *
 * This function will setup the I/O direction & pull up resistor for
 * the pin and then start watching the pin for button presses
 *********************************************************************/

extern void *debounce_init(char *pin)
{
	
	struct button *button;


	// Set up new button data 
	if ((button = malloc(sizeof(struct button))) == NULL)
		return NULL;

	button->next = NULL;
	button->port = NULL;
	button->current_debounce_count = 0;
	button->short_press = 0;
	button->long_press = 0;
	button->isr_short_press = 0;
	button->auto_acknowledge = 0;
	button->dead_time_counter = 0;

	// Setup io for button
	if (setup_io(pin, button) == RETURN_ERROR) 
		return NULL;

	// Start timer if necessary
	if (button_list_head == NULL)
		init_timer();

	// Register button
	add_button(button);
	
	// Chocks away
	return (button_t) button;

}

/*********************************************************************
 * button_check: check whether a button has been pressed
 *
 * Parameters:
 *		struct debounce_button *db - The button to check
 *					This is what the debounce_init function returns
 * Returns:
 *		button_press_t result: 
 *			One of BUTTON_PRESS_NONE, BUTTON_PRESS_SHORT or 
 *			BUTTON_PRESS_LONG
 *********************************************************************/

extern button_press_t button_check(button_t param)
{

	struct button *button = (struct button *) param;

	button_press_t result = BUTTON_PRESS_NONE;

	if (button->short_press) 
		result = BUTTON_PRESS_SHORT;

	if (button->long_press)
		result = BUTTON_PRESS_LONG;

	if (result != BUTTON_PRESS_NONE && button->auto_acknowledge) {
		button->long_press = 0;
		button->short_press = 0;
	}
		
	return result;

}

/*********************************************************************
 * button_acknowledge: acknowledge that you read the button status
 *
 * Parameters:
 *		struct debounce_button *db - The button to check
 *					This is what the debounce_init function returns
 *
 * This function will acknowledge a button press. While a button press
 * is not acknowledged, the library will not check its status, so this
 * acts as a lock on the button. If auto_acknowledge_button was set,
 * this need not be called explicitely
 *********************************************************************/
extern void button_acknowledge(button_t param)
{
	
	struct button *button = (struct button *) param;
	button->short_press = 0;
	button->long_press = 0;

}

extern void button_auto_acknowledge(button_t param)
{

	struct button *button = (struct button *) param;
	button->auto_acknowledge = 1;

}
