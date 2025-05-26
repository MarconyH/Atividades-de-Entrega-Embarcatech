#include "led.h"

void initialize_leds(uint *pin, int pin_count) {
    for (int i = 0; i < pin_count; i++) {
        gpio_init(pin[i]);              // Initialize the pin
        gpio_set_dir(pin[i], GPIO_OUT); // Set the pin as output
    }
}

void turn_on_led(uint pin) {
    gpio_put(pin, 1); // Set the pin high to turn on the LED
}

void turn_off_led(uint pin) {
    gpio_put(pin, 0); // Set the pin low to turn off the LED
}

void toggle_led(uint pin) {
    gpio_put(pin, !gpio_get(pin)); // Toggle the LED state
}

