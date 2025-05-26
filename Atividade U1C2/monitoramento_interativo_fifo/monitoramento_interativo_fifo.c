#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define RED_LED 13
#define BLUE_LED 12
#define GREEN_LED 11

#define JOYSTICK_X 27

#define BUZZER_PIN 10
#define BUZZER_FREQUENCY 100

int64_t period_ms = 2000;

volatile uint16_t flag_state = 1; 

const int joystick_max = 4086;

void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep
void beep(uint pin) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);
}

void setup()
{
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);

    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);

    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);

    adc_init();
    adc_gpio_init(JOYSTICK_X);

    pwm_init_buzzer(BUZZER_PIN);
    
}

int64_t callback_alarm(alarm_id_t id, void *user_data)
{
    multicore_fifo_push_blocking(flag_state);
    return period_ms;
}

void core1_main() {
    while (true) {
        uint32_t recebido = multicore_fifo_pop_blocking();
        switch (recebido)
        {
        case 1:
            gpio_put(GREEN_LED, 1);
            gpio_put(RED_LED, 0);
            gpio_put(BLUE_LED, 0);
            pwm_set_gpio_level(BUZZER_PIN, 0);
            break;
        
        case 2:
            gpio_put(BLUE_LED, 1);
            gpio_put(RED_LED, 0);
            gpio_put(GREEN_LED, 0);
            pwm_set_gpio_level(BUZZER_PIN, 0);
            break;

        case 3:
            gpio_put(RED_LED, 1);
            beep(BUZZER_PIN);
            gpio_put(GREEN_LED, 0);
            gpio_put(BLUE_LED, 0);
            break;
        
        default:
            printf("Error\n");
            break;
        }
    }
}

int main() {
    stdio_init_all();

    setup();
    
    sleep_ms(2000);

    add_alarm_in_ms(period_ms, callback_alarm, NULL, true);

    multicore_launch_core1(core1_main);
    uint joystick;
    adc_select_input(1);

    while(1)
    {
        joystick = adc_read();
        if (joystick > 0 && joystick < joystick_max/3)
        {
            flag_state = 1;
        }
        else if (joystick >= joystick_max/3 && joystick < joystick_max * 2/3)
        {
            flag_state = 2;
        }
        else if (joystick >= joystick_max * 2/3)
        {
            flag_state = 3;
        }
    }

    return 0;
}