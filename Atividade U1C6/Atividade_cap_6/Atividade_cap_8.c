#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13

// Protótipos
static void configurar_leds(void);
static void tratar_comando(const char *comando);

int main(void) {
    // Inicializa USB e LEDs
    stdio_init_all();
    configurar_leds();

    // Aguarda até que a conexão USB esteja ativa
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }
    printf("Conexão USB estabelecida.\n");

    // Loop principal
    while (true) {
        if (tud_cdc_available()) {
            uint8_t buffer[64] = {0};
            uint32_t bytes_recebidos = tud_cdc_read(buffer, sizeof(buffer) - 1);
            buffer[bytes_recebidos] = '\0';

            // Ecoa de volta ao host
            tud_cdc_write(buffer, bytes_recebidos);
            tud_cdc_write_flush();

            tratar_comando((const char *)buffer);
        }
        tud_task();
    }

    return 0;
}

// Configura os pinos dos LEDs como saída e os desliga
static void configurar_leds(void) {
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, false);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, false);

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, false);
}

// Interpreta o comando recebido e executa a ação correspondente
static void tratar_comando(const char *comando) {
    printf("[INFO] Comando recebido: %s\n", comando);

    if (strcmp(comando, "vermelho") == 0) {
        printf("Ativando LED VERMELHO\n");
        gpio_put(LED_VERMELHO, true);
        sleep_ms(1000);
        gpio_put(LED_VERMELHO, false);
    } else if (strcmp(comando, "verde") == 0) {
        printf("Ativando LED VERDE\n");
        gpio_put(LED_VERDE, true);
        sleep_ms(1000);
        gpio_put(LED_VERDE, false);
    } else if (strcmp(comando, "azul") == 0) {
        printf("Ativando LED AZUL\n");
        gpio_put(LED_AZUL, true);
        sleep_ms(1000);
        gpio_put(LED_AZUL, false);
    } else {
        printf("Comando não reconhecido: '%s'\n", comando);
    }
}
