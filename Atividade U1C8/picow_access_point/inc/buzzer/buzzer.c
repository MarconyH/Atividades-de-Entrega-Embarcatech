#include "buzzer.h"

alarm_id_t turn_on_alarm = 0;
alarm_id_t turn_off_alarm = 0;
volatile bool turn_on = false;

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin)
{
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

uint64_t chama_alarme(alarm_id_t id, void *user_data)
{
    if (turn_on)
    {
        uint pin_led = (uint)(uintptr_t)user_data;

        if (pin_led != 0)
        { // caso pino seja diferente de zero, aciona pin_led=true;
            gpio_put(pin_led, true);
        }

        pwm_set_gpio_level(BUZZER, 2048); // Liga o buzzer
        turn_off_alarm = add_alarm_in_ms(500, (alarm_callback_t)desliga_alarme, (void *)(uintptr_t)pin_led, true);
    }
    return 0;
}

uint64_t desliga_alarme(alarm_id_t id, void *user_data)
{
    uint pin_led = (uint)(uintptr_t)user_data;
    if (pin_led != 0)
    {
        gpio_put(pin_led, false);
    }

    pwm_set_gpio_level(BUZZER, 0);
    turn_on_alarm = add_alarm_in_ms(500, (alarm_callback_t)chama_alarme, (void *)(uintptr_t)pin_led, true);
    return 0;
}

void iniciar_alarme(uint pin_led)
{
    turn_on = true;
    turn_on_alarm = add_alarm_in_ms(1000, (alarm_callback_t)chama_alarme, (void *)(uintptr_t)pin_led, true);
}

void parar_alarme(uint pin_led)
{
    cancel_alarm(turn_off_alarm);
    cancel_alarm(turn_on_alarm);
    turn_on = false;
    add_alarm_in_ms(0, (alarm_callback_t)desliga_alarme, (void *)(uintptr_t)pin_led, true);
}
