#ifndef BUZZER_H
#define BUZZER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

void pwm_init_buzzer(uint pin, uint frequency);
void stop_buzzer(uint pin);
void start_buzzer(uint pin);
void beep_buzzer(uint pin, uint duration);


#endif