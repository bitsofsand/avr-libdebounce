/************************************************************
 * DEBOUNCE_COUNTs
 *
 * Number of 10ms slices before a button press is counted as
 * short, resp. long
 ************************************************************/

#define DEBOUNCE_COUNT_SHORT	10
#define DEBOUNCE_COUNT_LONG		100
#define DEBOUNCE_COUNT_MID		(DEBOUNCE_COUNT_LONG - DEBOUNCE_COUNT_SHORT) / 2 

/******************************************************************
 * struct debounce_button: initialisation data
 *
 * Members:
 *		char *button_pin	The pin the button is connected to
 *							Use Pxx format, eg PB0
 *		uint8_t auto_acknowledge_button
 *							Set this to >0 to have button_check()
 *							automatically acknowledge the button
 *							read. If set to 0, you will need to
 *							call button_acknowledge() explicitely
 *		void *private_data	Pointer to internal button struct
 *****************************************************************/
extern struct debounce_button {

	char *button_pin;
	uint8_t auto_acknowledge_button;
	void *private_data;

};

typedef enum {
	BUTTON_PRESS_NONE,
	BUTTON_PRESS_SHORT,
	BUTTON_PRESS_LONG,
} button_press_t;


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
extern struct debounce_button *debounce_init(struct debounce_button *);

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
 *
 * This function will, apart from returning the button state, also
 * acknowledge a button press if auto_acknowledge_button was set
 *********************************************************************/

extern button_press_t button_check(struct debounce_button *);

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
extern void button_acknowledge(struct debounce_button *);


