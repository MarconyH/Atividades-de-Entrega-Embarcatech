#include "temp.h"

// Converte o valor bruto do ADC (12 bits) para temperatura em graus Celsius
float convert_to_celsius(uint16_t raw) {
    const float conversion_factor = 3.3f / (1 << 12); // Fator de conversão para 3.3V e 12 bits
    float voltage = raw * conversion_factor;          // Converte valor para tensão
    return 27.0f - (voltage - 0.706f) / 0.001721f;     // Fórmula do datasheet do RP2040
}

void initialize_adc() {
    // Inicializa o ADC e habilita o sensor de temperatura interno
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); // Canal 4 é o sensor de temperatura interna do RP2040
}

float get_temperature()
{
    return convert_to_celsius(adc_read());
}