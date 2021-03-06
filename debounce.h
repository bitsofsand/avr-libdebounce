/*
 * debounce.h
 *
 * Created: 24/02/2016 10:15:35
 *  Author: robartes
 */ 


#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_


#include <stdint.h>

/************************************************************
 * DEBOUNCE_COUNTs
 *
 * Number of 10ms slices before a button press is counted as
 * short, resp. long
 * Dead time is number of 10ms slices after a press detection
 * to not check the button
 ************************************************************/

#define DEBOUNCE_COUNT_SHORT	10
#define DEBOUNCE_COUNT_LONG		100
#define DEBOUNCE_COUNT_MID		(DEBOUNCE_COUNT_LONG - DEBOUNCE_COUNT_SHORT) / 2 
#define DEBOUNCE_DEAD_TIME_SHORT	50 	// 0.5 sec
#define DEBOUNCE_DEAD_TIME_LONG		250 	// 2.5 sec

#define OCR_VALUE		80 // Hardcoded for 8 MHz, results in 10ms slices

typedef void * button_t;

typedef enum {
	BUTTON_PRESS_NONE,
	BUTTON_PRESS_SHORT,
	BUTTON_PRESS_LONG,
} button_press_t;


/*********************************************************************
 * debounce_init: setup a button for debouncing
 *
 * Parameters
 *		char * button_pin
 *			Pin the button is connected to. Use standard
 *			AVR naming, e.g. "PB0"
 * Returns
 *		button_t button
 *			Opaque data structure to be used in further
 *			calls in this library
 * 
 * This library expects the button to pull the pin to GND. 
 *
 * This function will setup the I/O direction & pull up resistor for
 * the pin and then start watching the pin for button presses
 *********************************************************************/
extern button_t debounce_init(char *);

/*********************************************************************
 * button_check: check whether a button has been pressed
 *
 * Parameters:
 * 		button_t button
 * 			The button to check
 * Returns:
 *		button_press_t result: 
 *			One of BUTTON_PRESS_NONE, BUTTON_PRESS_SHORT or 
 *			BUTTON_PRESS_LONG
 *
 * This function will, apart from returning the button state, also
 * acknowledge a button press if auto_acknowledge_button was set
 *********************************************************************/

extern button_press_t button_check(button_t);

/*********************************************************************
 * button_acknowledge: acknowledge that you read the button status
 *
 * Parameters:
 * 		button_t button
 * 			The button to acknowledge
 *
 * This function will acknowledge a button press. While a button press
 * is not acknowledged, the library will not check its status, so this
 * acts as a lock on the button. If auto_acknowledge_button was set,
 * this need not be called explicitely
 *********************************************************************/
extern void button_acknowledge(button_t);

/*********************************************************************
 * button_auto_acknowledge: set a button to auto-acknowledge checks
 *
 * Parameters:
 * 		button_t button
 * 			The button to auto acknowledge
 *
 * This will set up the button to auto acknowledge checks. When 
 * button_check is called for a button with auto_acknowledge set, that
 * will immediately acknowledge the button
 *********************************************************************/

extern void button_auto_acknowledge(button_t);



#endif /* DEBOUNCE_H_ */
