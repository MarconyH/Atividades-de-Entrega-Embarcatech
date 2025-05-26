#include "buzzer.h"

void pwm_init_buzzer(uint pin, uint frequency) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (frequency * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

void start_buzzer(uint pin) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);
}

void stop_buzzer(uint pin) {
    pwm_set_gpio_level(pin, 0);
}

void beep_buzzer(uint pin, uint duration) {
    // Iniciar o buzzer
    start_buzzer(pin);

    // Aguardar a duração especificada
    sleep_ms(duration);

    // Parar o buzzer
    stop_buzzer(pin);
}
