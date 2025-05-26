#ifndef BUZZER_H
#define BUZZER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER 10

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100



void pwm_init_buzzer(uint pin);
uint64_t chama_alarme(alarm_id_t id, void *user_data);
uint64_t desliga_alarme(alarm_id_t id, void *user_data);
void iniciar_alarme(uint pin_led);
void parar_alarme(uint pin_led);

#endif


