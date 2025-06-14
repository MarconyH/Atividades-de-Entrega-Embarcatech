#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

#define LED_GPIO 13
static char html_response[1024];

// Função para ler a temperatura interna
float read_temperature() {
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096.0f;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

// Gera HTML com estado do LED e temperatura
void build_html_response(bool led_on, float temperature) {
    snprintf(html_response, sizeof(html_response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<!DOCTYPE html><html><head><title>Pico W Control</title></head><body>"
        "<h1>Controle do LED</h1>"
        "<p>LED está %s</p>"
        "<p><a href=\"/led/on\">Ligar</a> | <a href=\"/led/off\">Desligar</a></p>"
        "<p>Temperatura: %.2f &deg;C</p>"
        "</body></html>", led_on ? "Ligado" : "Desligado", temperature);
}

// Callback correto conforme esperado por tcp_recv()
err_t handle_http(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (!p) return ERR_OK;

    char *request = malloc(p->tot_len + 1);
    pbuf_copy_partial(p, request, p->tot_len, 0);
    request[p->tot_len] = '\0';

    printf("Requisição recebida: %s\n", request);

    static bool led_on = false;
    if (strstr(request, "GET /led/on")) {
        gpio_put(LED_GPIO, true);
        led_on = true;
    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_GPIO, false);
        led_on = false;
    }

    float temperature = read_temperature();
    build_html_response(led_on, temperature);

    tcp_write(pcb, html_response, strlen(html_response), TCP_WRITE_FLAG_COPY);

    free(request);
    pbuf_free(p);
    return ERR_OK;
}

// Aceita conexões
err_t accept_callback(void *arg, struct tcp_pcb *pcb, err_t err) {
    tcp_recv(pcb, handle_http);  // Corrigido: função com assinatura correta
    return ERR_OK;
}

int main() {
    stdio_init_all();
    sleep_ms(2000); // Aguardar terminal abrir

    printf("Inicializando servidor HTTP...\n");

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_WORLDWIDE)) {  // Corrigido
        printf("Erro ao iniciar Wi-Fi\n");
        return -1;
    }

    cyw43_arch_enable_ap_mode("picow_test", "password", CYW43_AUTH_WPA2_AES_PSK);
    printf("Access Point criado: SSID = 'picow_test'\n");

    adc_init();
    adc_set_temp_sensor_enabled(true);

    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar socket TCP.\n");
        return -1;
    }

    tcp_bind(pcb, IP_ADDR_ANY, 80);
    pcb = tcp_listen_with_backlog(pcb, 1);
    tcp_accept(pcb, accept_callback);

    while (true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }

    return 0;
}