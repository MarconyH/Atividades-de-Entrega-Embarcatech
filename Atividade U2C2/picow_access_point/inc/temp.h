#ifndef TEMP_H
#define TEMP_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

void initialize_adc();
float convert_to_celsius(uint16_t raw);
float get_temperature();

#endif