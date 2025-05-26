#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "inc/ssd1306.h"
#include "inc/led.h"
#include "inc/buzzer.h"

char red[] = "vermelho";
char green[] = "verde";
char blue[] = "azul";
char buzzer[] = "som";

bool equal_to(char *str1, char *str2, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}

void blink_led(uint pin)
{
    turn_on_led(pin);  // Liga o LED
    sleep_ms(1000);    // Aguarda 1 segundo
    turn_off_led(pin); // Desliga o LED
}

char *get_string(uint8_t *buf, uint32_t count)
{
    char *str = malloc(count + 1); // Aloca memória para a string
    if (str == NULL)
    {
        return NULL; // Retorna NULL se a alocação falhar
    }
    for (int i = 0; i < count; i++)
    {
        str[i] = buf[i]; // Copia os dados do buffer para a string
    }
    str[count] = '\0';       // Adiciona o terminador nulo
    return str;
}

int main()
{
    // Inicializa o USB
    stdio_init_all();

    pwm_init_buzzer(10, 20000); // Inicializa o buzzer no pino 15 com frequência de 1000 Hz
    initialize_leds((uint[]){13, 12, 11}, 3); // Inicializa os LEDs nos pinos 13, 12 e 11
    init_display(14, 15);
    // Aguarda o USB ser montado
    // Verifica se o host (PC) conectou-se ao dispositivo CDC
    while (!tud_cdc_connected())
    {
        sleep_ms(100);
    }
    // Informa via terminal serial que a conexão foi detectada
    printf("USB conectado!\n");
    uint pin = 0;
    // Loop principal: ecoa o que receber
    while (true)
    {
        if (tud_cdc_available())
        {                                                    // Verifica se há dados disponíveis vindos do host (PC)
            uint8_t buf[64];                                 // Declara um buffer de 64 bytes
            uint32_t count = tud_cdc_read(buf, sizeof(buf)); // Lê os dados recebidos via USB para esse buffer
            print_text(get_string(buf, count)); // Imprime os dados recebidos no terminal serial
            // tud_cdc_write(buf, count);
            // tud_cdc_write_flush();

            if (equal_to((char *)buf, red, count) == true)
            {
                blink_led(13); // Liga o LED vermelho
            }
            else if (equal_to((char *)buf, blue, count) == true)
            {
                blink_led(12); // Liga o LED azul
            }
            else if (equal_to((char *)buf, green, count) == true)
            {
                blink_led(11); // Liga o LED verde
            }
            else if (equal_to((char *)buf, buzzer, count) == true)
            {
                beep_buzzer(10, 1000); // Liga o buzzer
            }
        }
        tud_task(); // Executa tarefas USB
    }

    return 0;
}
