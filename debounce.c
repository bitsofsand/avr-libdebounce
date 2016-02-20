#include <avr/io.h>
#include <avr/interrupt.h>

#include "debounce.h"

#define CLOCK_DIVISOR	10000
#define OCR_VALUE		F_CPU / CLOCK_DIVISOR

/*********************************************************************
 * File global variables
 *********************************************************************/

volatile struct button {

	struct button *next;
	volatile uint8_t *port;
	uint8_t pin;
	uint8_t current_debounce_count;
	uint8_t short_press;
	uint8_t isr_short_press;
	uint8_t long_press;
	uint8_t dirty;
	
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

static void setup_io(struct debounce_button *data)
{

}

static void add_button_to_list(struct debounce_data *data)
{

}

static struct button *get_next_button(void)
{

}


/******************************************************************
 * Timer0 compare match interrupt: debounce button press
 *
 * This function does all the actual work of the library. It gets
 * called regularly and checks each button
 ******************************************************************/

// TODO: adapt to multiple buttons

ISR(TIM0_COMPA_vect)
{

	struct button *button;

	while(button = get_next_button() != NULL) {

		if(button->dirty)
			continue;

		switch (button->current_debounce_count) {

			case 0:
				if (button_is_pressed(button))
					button->current_debounce_count = 1;
				break;

		}

	}
/*
	if (button_press_acknowledged) {

		switch (current_debounce_count) {

			case 0:
				if (bit_is_clear(PINB,BUTTON)) {
					current_debounce_count = 1;
				}
				break;

			case DEBOUNCE_COUNT_SHORT:
				if (bit_is_clear(PINB,BUTTON)) {
					// Press detected
					isr_short_press = 1;
				}
				current_debounce_count++;
				break;

			case DEBOUNCE_COUNT_MID:
				if (isr_short_press && bit_is_set(PINB,BUTTON)) {
					// Button no longer pressed: it's a short
					short_press = 1;
					button_press_acknowledged = 0;
					current_debounce_count = 0;	
					isr_short_press = 0;
				} else {
					// Button still pressed - wait for a long press
					current_debounce_count++;
				}
				break;

			// If we get to this case, the button has been pressed and was still
			// pressed at DEBOUNCE_COUNT_MID. So it is (probably) not two short
			// presses DEBOUNCE_COUNT_LONG apart
			case DEBOUNCE_COUNT_LONG:
				if (bit_is_clear(PINB,BUTTON)) {
					// It's a long press
					long_press = 1;
					button_press_acknowledged = 0;
				} else if (isr_short_press) {
					// It was a short press after all
					short_press = 1;
					button_press_acknowledged = 0;
				}
				current_debounce_count = 0;
				isr_short_press = 0;
				break;

			default:	// 1 to DEBOUNCE_COUNT_LONG-1
				current_debounce_count++;
				break;
			
				
		}
	}
*/

}

/*********************************************************************
 * Public functions
 *********************************************************************/


extern struct debounce_button *debounce_init(struct debounce_button *data)
{

	
	// Sanity check

	// Set up new button data 
	if (struct button *button = malloc(sizeof(struct button)) == NULL)
		return NULL;

	// Squirrel away data
	data->private_data = (void *) button;

	// Setup io for button
	setup_io(data);

	// Start timer if necessary
	if (button_list_head == NULL)
		init_timer();

	// Register button
	add_button_to_list(data);
	
	// Chocks away
	return data;

}
