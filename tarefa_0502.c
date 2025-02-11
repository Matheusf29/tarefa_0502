#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PINO 11
//#define PINO 22  // GPIO utilizada para o controle do servo
#define PWM_FREQ 50   // Frequência do PWM em Hz (50Hz = período de 20ms)

// Função para configurar o PWM na GPIO especificada (pin 22)
void configure_pwm(uint pin, float duty_cycle_us) {
    // Calcula o período do PWM em microssegundos
    float period_us = 1000000.0 / PWM_FREQ;

    // Inicializa o slice e o canal
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurando o clock do PWM
    pwm_set_clkdiv(slice_num, 125.0); // 125 MHz

    // Configurando o duty cicle
    uint16_t top = (uint16_t)(125000000 / (PWM_FREQ * 125)); // TOP para 50Hz
    pwm_set_wrap(slice_num, top);

    uint16_t level = (uint16_t)((duty_cycle_us / period_us) * top);
    pwm_set_gpio_level(pin, level);

    // Ativa o PWM
    pwm_set_enabled(slice_num, true);
}

// Função para ajustar suavemente o ângulo do servo
void move_suave(uint pin, float start_us, float end_us, uint step_us, uint delay_ms) {
    if (start_us < end_us) {
        for (float duty = start_us; duty <= end_us; duty += step_us) {
            configure_pwm(pin, duty);
            sleep_ms(delay_ms);
        }
    } else {
        for (float duty = start_us; duty >= end_us; duty -= step_us) {
            configure_pwm(pin, duty);
            sleep_ms(delay_ms);
        }
    }
}

void inic_pino(){
    // Define o servo para 180 graus
    configure_pwm(PINO, 2400);  
    sleep_ms(5000);

    // Define o servo para 90 graus
    configure_pwm(PINO, 1470);  
    sleep_ms(5000);

    // Define o servo para 0 graus
    configure_pwm(PINO, 500); 
    sleep_ms(5000);
}

int main() {
    stdio_init_all();
    gpio_set_function(PINO, GPIO_FUNC_PWM);  // Configura o GPIO do servo

    printf("Controle de Servo\n");
    sleep_ms(50); // Delay para podermos ver a mudança na posição do servo

    inic_pino();   // Função para realizar os 3 primeiros passos

    while (true) {
        move_suave(PINO, 500, 2400, 5, 10); // Movimento de 0 para 180 graus
        
        move_suave(PINO, 2400, 500, 5, 10); // Movimento de 180 para 0 graus
    }

    return 0;
}