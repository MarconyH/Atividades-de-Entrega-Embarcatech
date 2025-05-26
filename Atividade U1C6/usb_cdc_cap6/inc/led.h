#ifndef LED_H
#define LED_H
#include <stdlib.h>
#include "pico/stdlib.h"

void initialize_leds(uint *pin, int pin_count);
void turn_on_led(uint pin);
void turn_off_led(uint pin);
void toggle_led(uint pin);

#endif