/************************************************************
 * DEBOUNCE_COUNTs
 *
 * Number of 10ms slices before a button press is counted as
 * short, resp. long
 ************************************************************/

#define DEBOUNCE_COUNT_SHORT	10
#define DEBOUNCE_COUNT_LONG		100
#define DEBOUNCE_COUNT_MID		(DEBOUNCE_COUNT_LONG - DEBOUNCE_COUNT_SHORT) / 2 

// This struct is just to have a nice name for the function
// arguments below, instead of having to use void *
struct debounce_button {

	void *private_data;

};

typedef enum {
	BUTTON_PRESS_NONE,
	BUTTON_PRESS_SHORT,
	BUTTON_PRESS_LONG,
} button_press_t;

extern struct debounce_button *debounce_init(char *button_pin);
extern button_press_t button_check(struct debounce_button *);
extern uint8_t button_acknowledge(struct debounce_button *);


