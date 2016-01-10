
struct debounce_init {

};

struct debounce_data {

};

typedef enum {
	BUTTON_PRESS_SHORT,
	BUTTON_PRESS_LONG,
} button_press_t;

struct debounce_data *debounce_init(struct debounce_init *);
button_press_t button_check(struct debounce_data *);
uint8_t button_acknowledge(struct debounce_data *);
