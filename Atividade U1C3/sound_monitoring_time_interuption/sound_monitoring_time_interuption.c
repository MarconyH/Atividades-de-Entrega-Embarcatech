/*
Nome: Marcony Henrique Bento Souza
Email: marconyhenrique321@gmail.com
Matrícula: 20251RSE.MTC0089
*/

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"

#include "build/ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Definição do microfone e pino.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

#define ADC_CLOCK_DIV 96.f // Divisor do clock do ADC. O clock do ADC é 48MHz, então 48MHz/96 = 500kHz.

#define SAMPLES 200 // Número de amostras que serão feitas do ADC.

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

bool loud = true;

bool read_mic(struct repeating_timer *t) {
  uint16_t mic = adc_read();

  if (mic < 2100) {
    loud = false;
  } else {
    loud = true;
  }

  return true; // Retorna true para continuar o timer.
}

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin) {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

void npSmiley() {
    npSetLED(16, 0, 255, 0);
    npSetLED(18, 0, 255, 0);
  
    npSetLED(5, 0, 255, 0);
    npSetLED(1, 0, 255, 0);
    npSetLED(2, 0, 255, 0);
    npSetLED(3, 0, 255, 0);
    npSetLED(9, 0, 255, 0);
}

void npSmileySad() {
    npSetLED(16, 255, 0, 0);
    npSetLED(18, 255, 0, 0);
  
    npSetLED(0, 255, 0, 0);
    npSetLED(6, 255, 0, 0);
    npSetLED(7, 255, 0, 0);
    npSetLED(8, 255, 0, 0);
    npSetLED(4, 255, 0, 0);
}

int main() {
  stdio_init_all();

  // Delay para o usuário abrir o monitor serial...
  sleep_ms(5000);

  // Preparação da matriz de LEDs.
  printf("Preparando NeoPixel...");
  
  npInit(LED_PIN);

  // Preparação do ADC.
  printf("Preparando ADC...\n");

  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  printf("ADC Configurado!\n\n");

  printf("Configurando Interrupção...\n");

  repeating_timer_t timer;

  add_repeating_timer_ms(100, read_mic, NULL, &timer); // Inicia o timer para ler o microfone a cada 100ms.

  printf("Interrupção configurada!\n\n");

  printf("Testando ADC...\n");

  printf("ADC: %d\n", adc_read());

  printf("ADC Testado!\n\n");

  printf("Configuracoes completas!\n");

  printf("\n----\nIniciando loop...\n----\n");
  bool prev_loud = loud;
  while (true) {

    
    // Limpa a matriz de LEDs.
    npClear();
    if (!loud) {
      npSmiley();
      printf("Ambiente Silencioso\n");
      if (prev_loud) {
        sleep_ms(500); // Espera 1 segundo para evitar piscar.
      }
    } else{
      npSmileySad();
      printf("Ambiente Barulhento\n");
      if (!prev_loud) {
        sleep_ms(500); // Espera 1 segundo para evitar piscar.
      }
    }
    prev_loud = loud;
    // Atualiza a matriz.
    npWrite();
    sleep_ms(100); // Espera 100ms para evitar piscar.
  }
}
