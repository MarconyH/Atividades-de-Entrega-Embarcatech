#include <stdio.h>
#include "pico/stdlib.h"
#include "neopixel_driver.h"
#include "testes_cores.h"
#include "efeitos.h"
#include "efeito_curva_ar.h"
#include "numeros_neopixel.h"
#include <time.h>
#include <stdlib.h>
#include "pico/time.h" // Garante acesso a time_us_32()

#define BUTTON_A 5

volatile bool raffle_active = false; // Variável para controlar o sorteio

// Inicializa o sistema e a matriz NeoPixel
void setup()
{
    stdio_init_all();
    sleep_ms(1000); // Aguarda conexão USB (opcional)

    // Inicializa Botão A
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    npInit(LED_PIN);     // Inicializa matriz NeoPixel
    srand(time_us_32()); // Semente para aleatoriedade
}

// Sorteia número inteiro entre [min, max]
int sorteia_entre(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// Exibe o número sorteado de 1 a 6
void mostrar_numero_sorteado(int numero)
{
    switch (numero)
    {
    case 1:
        mostrar_numero_1();
        break;
    case 2:
        mostrar_numero_2();
        break;
    case 3:
        mostrar_numero_3();
        break;
    case 4:
        mostrar_numero_4();
        break;
    case 5:
        mostrar_numero_5();
        break;
    case 6:
        mostrar_numero_6();
        break;
    }
}

void isr_botoes(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_A)
    {
        raffle_active = true; // Ativa o sorteio quando o botão é pressionado
    }
}

int main()
{
    setup();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &isr_botoes);
    while (true)
    {
        if (raffle_active)
        {
            raffle_active = false; // Reseta o estado do sorteio
            int vezes = sorteia_entre(100, 500); // Loop entre 10 e 50 execuções
            printf("Mostrando %d números aleatórios...\n", vezes);

            for (int i = 0; i < vezes; i++)
            {
                int n = sorteia_entre(1, 6);
                printf("Número sorteado: %d\n", n);
                mostrar_numero_sorteado(n);
                sleep_ms(10);
            }
            sleep_ms(1000); // Aguarda 1 segundo antes de permitir novo sorteio
            printf("Sorteio concluído. Pressione o botão A para sortear novamente.\n");
        }
    }

    return 0;
}