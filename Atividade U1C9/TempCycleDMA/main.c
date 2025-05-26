/**
 * ------------------------------------------------------------
 *  Arquivo: main.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Ciclo principal do sistema embarcado, baseado em um
 *      executor cíclico com 3 tarefas principais:
 *
 *      Tarefa 1 - Leitura da temperatura via DMA (meio segundo)
 *      Tarefa 2 - Exibição da temperatura e tendência no OLED
 *      Tarefa 3 - Análise da tendência da temperatura
 *
 *      O sistema utiliza watchdog para segurança, terminal USB
 *      para monitoramento e display OLED para visualização direta.
 *
 *
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"

#include "setup.h"
#include "tarefa1_temp.h"
#include "tarefa2_display.h"
#include "tarefa3_tendencia.h"
#include "tarefa4_controla_neopixel.h"
#include "neopixel_driver.h"
#include "testes_cores.h"
#include "pico/stdio_usb.h"

bool tarefa_3_callback(struct repeating_timer *t);
bool tarefa_5_callback(struct repeating_timer *t);
bool tarefa_4_callback(struct repeating_timer *t);
bool tarefa_2_callback(struct repeating_timer *t);
bool tarefa_1_callback(struct repeating_timer *t);
void core1_main();

volatile bool tarefa1_pronta = false;
volatile bool tarefa2_pronta = false;
volatile bool tarefa3_pronta = false;
volatile bool tarefa4_pronta = false;
volatile bool tarefa5_pronta = false;
float media = 0.0;
int execution_order[5] = {1, 5, 3, 4, 2}; // Ordem de execução das tarefas
int order_index = 0;
tendencia_t tend = TENDENCIA_ESTÁVEL;
absolute_time_t ini_tarefa1, fim_tarefa1, ini_tarefa2, fim_tarefa2, ini_tarefa3, fim_tarefa3, ini_tarefa4, fim_tarefa4;

int main()
{
        setup(); // Inicializações: ADC, DMA, interrupções, OLED, etc.

        sleep_ms(1000); // Aguarda estabilização do sistema

        multicore_launch_core1(core1_main);

        repeating_timer_t tarefa1, tarefa2, tarefa3, tarefa4, tarefa5;

        add_repeating_timer_ms(-250, tarefa_1_callback, NULL, &tarefa1);
        add_repeating_timer_ms(-500, tarefa_2_callback, NULL, &tarefa2);
        add_repeating_timer_ms(-750, tarefa_3_callback, NULL, &tarefa3);
        add_repeating_timer_ms(-1000, tarefa_4_callback, NULL, &tarefa4);
        add_repeating_timer_ms(-1250, tarefa_5_callback, NULL, &tarefa5);
        // Ativa o watchdog com timeout de 2 segundos
        // watchdog_enable(2000, 1);

        while (true)
        {
                // --- Cálculo dos tempos de execução ---
                int64_t tempo1_us = absolute_time_diff_us(ini_tarefa1, fim_tarefa1);
                int64_t tempo2_us = absolute_time_diff_us(ini_tarefa2, fim_tarefa2);
                int64_t tempo3_us = absolute_time_diff_us(ini_tarefa3, fim_tarefa3);
                int64_t tempo4_us = absolute_time_diff_us(ini_tarefa4, fim_tarefa4);

                // --- Exibição no terminal ---
                printf("Temperatura: %.2f °C | T1: %.3fs | T2: %.3fs | T3: %.3fs | T4: %.3fs | Tendência: %s\n",
                       media,
                       tempo1_us / 1e6,
                       tempo2_us / 1e6,
                       tempo3_us / 1e6,
                       tempo4_us / 1e6,
                       tendencia_para_texto(tend));

                sleep_ms(1000); // Aguarda próximo ciclo
        }

        return 0;
}

void core1_main()
{
        while (1)
        {
                int tarefa_atual = execution_order[order_index];

                if (tarefa_atual == 1 && tarefa1_pronta)
                {
                        ini_tarefa1 = get_absolute_time();
                        media = tarefa1_obter_media_temp(&cfg_temp, DMA_TEMP_CHANNEL);
                        fim_tarefa1 = get_absolute_time();
                        tarefa1_pronta = false;
                        // Atualiza ordem
                        order_index = (order_index + 1) % 5;
                }
                else if (tarefa_atual == 2 && tarefa2_pronta)
                {
                        ini_tarefa2 = get_absolute_time();
                        tarefa2_exibir_oled(media, tend);
                        fim_tarefa2 = get_absolute_time();
                        tarefa2_pronta = false;
                        // Atualiza ordem
                        order_index = (order_index + 1) % 5;
                }
                else if (tarefa_atual == 3 && tarefa3_pronta)
                {
                        ini_tarefa3 = get_absolute_time();
                        tend = tarefa3_analisa_tendencia(media);
                        fim_tarefa3 = get_absolute_time();
                        tarefa3_pronta = false;
                        // Atualiza ordem
                        order_index = (order_index + 1) % 5;
                }
                else if (tarefa_atual == 4 && tarefa4_pronta)
                {
                        ini_tarefa4 = get_absolute_time();
                        tarefa4_matriz_cor_por_tendencia(tend);
                        fim_tarefa4 = get_absolute_time();
                        tarefa4_pronta = false;
                        // Atualiza ordem
                        order_index = (order_index + 1) % 5;
                }
                else if (tarefa_atual == 5 && tarefa5_pronta)
                {
                        while (media < 1)
                        {
                                npSetAll(COR_BRANCA);
                                npWrite();
                                sleep_ms(1000);
                                npClear();
                                npWrite();
                                sleep_ms(1000);
                        }
                        tarefa5_pronta = false;
                        // Atualiza ordem
                        order_index = (order_index + 1) % 5;
                }
                else
                {
                        // Se nenhuma tarefa estiver pronta, pode dormir ou fazer outra coisa
                        sleep_ms(10);
                        continue;
                }
        }
}

/*******************************/
bool tarefa_1_callback(struct repeating_timer *t)
{
        tarefa1_pronta = true;
        return true;
}

bool tarefa_2_callback(struct repeating_timer *t)
{
        tarefa2_pronta = true;
        return true;
}

bool tarefa_3_callback(struct repeating_timer *t)
{
        tarefa3_pronta = true;
        return true;
}

bool tarefa_4_callback(struct repeating_timer *t)
{
        tarefa4_pronta = true;
        return true;
}

bool tarefa_5_callback(struct repeating_timer *t)
{
        tarefa5_pronta = true;
        return true;
}
