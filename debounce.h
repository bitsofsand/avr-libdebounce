#define DEBOUNCE_COUNT_SHORT	10	// Number of 10ms slices after which a button press is registered as a short push
#define DEBOUNCE_COUNT_LONG		100	// Number of 10ms slices after which a button press is registered as a long push
#define DEBOUNCE_COUNT_MID		(DEBOUNCE_COUNT_LONG - DEBOUNCE_COUNT_SHORT) / 2 

struct debounce_button {

	volatile uint8_t *port;
	uint8_t pin;
	void *private_data;

};

typedef enum {
	BUTTON_PRESS_NONE,
	BUTTON_PRESS_SHORT,
	BUTTON_PRESS_LONG,
} button_press_t;

extern struct debounce_button *debounce_init(struct debounce_button *);
extern button_press_t button_check(struct debounce_button *);
extern uint8_t button_acknowledge(struct debounce_button *);


