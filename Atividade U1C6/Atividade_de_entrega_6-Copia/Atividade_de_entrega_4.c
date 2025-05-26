#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

#define green_led 11
#define blue_led 12
#define red_led 13

//função que ativa as mensagens e leds quando capta a mensagem e ressoa o eco
void MensageAction(const char* count);

//função que configura e inicializa os leds, por padrão desligados
void InitLeds();

int main() {
    // Inicializa o USB
    stdio_init_all();

    //inicializando componentes visuais
    InitLeds();

    // Aguarda o USB ser montado
    // Verifica se o host (PC) conectou-se ao dispositivo CDC
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }
    // Informa via terminal serial que a conexão foi detectada
    printf("USB conectado!\n");

    // Loop principal: ecoa o que receber
    while (true) {
        if (tud_cdc_available()) { // Verifica se há dados disponíveis vindos do host (PC)
            uint8_t buf[64]; // Declara um buffer de 64 bytes
            uint32_t count = tud_cdc_read(buf, sizeof(buf) -1); // Lê os dados recebidos via USB para esse buffer
            buf[count] = '\0';

            tud_cdc_write(buf, count); // Escreve os mesmos dados de volta ao host, efetivamente fazendo um eco(imprime a palavra inteira)
            tud_cdc_write_flush();
            MensageAction(buf);   //quando recebe comando, atende e executa o eco
        }
        tud_task(); // Executa tarefas USB
    }

    return 0;
}

void MensageAction(const char* count){
    printf("---\nMensagem recebida!\n---");

    if(strcmp(count, "vermelho") == 0){  //strcmp verifica se duas strings são iguais
        printf("vermelho\n");
        gpio_put(red_led, true);
        sleep_ms(1000);
        gpio_put(red_led, false);
    }
    else if(strcmp(count, "verde")  == 0){  //se strings são iguais, retorna 0
        printf("verde\n");
        gpio_put(green_led, true);
        sleep_ms(1000);
        gpio_put(green_led, false);
    }
    else if(strcmp(count, "azul")  == 0){  // caso seja verdadeiro, executa o eco e pisca o led de cor correspondente por um segundo
        printf("azul\n");
        gpio_put(blue_led, true);
        sleep_ms(1000);
        gpio_put(blue_led, false);
    }
    else{
        printf("Comando desconhecido: %s\n", count); // Mostra o comando recebido para debug
    }
}

void InitLeds(){
    gpio_init(green_led);
    gpio_init(blue_led);
    gpio_init(red_led);

    gpio_set_dir(green_led, GPIO_OUT);
    gpio_set_dir(blue_led, GPIO_OUT);
    gpio_set_dir(red_led, GPIO_OUT);

    gpio_put(green_led, false);
    gpio_put(blue_led, false);
    gpio_put(red_led, false);

}