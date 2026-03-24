#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

// Pinos
const int LED_PIN_R = 2;
const int LED_PIN_G = 9;
const int LED_PIN_P = 26;

const int BTN_PINS[] = {21, 27, 28, 17}; // Y, B, G, W
const int BTN_COUNT = 4;

// Flags de interrupção
volatile int pino_pressionado = -1;

void btn_callback(uint gpio, uint32_t events) {
    // Identifica qual botão foi pressionado
    for (int i = 0; i < BTN_COUNT; i++) {
        if (gpio == BTN_PINS[i]) {
            pino_pressionado = i;
            break;
        }
    }
}

void pisca_roxo() {
    gpio_put(LED_PIN_P, 1);
    sleep_ms(100); // Feedback rápido para o usuário
    gpio_put(LED_PIN_P, 0);
}

int main() {
    stdio_init_all();

    // Setup LEDs
    int leds[] = {LED_PIN_R, LED_PIN_G, LED_PIN_P};
    for(int i=0; i<3; i++) {
        gpio_init(leds[i]);
        gpio_set_dir(leds[i], GPIO_OUT);
        gpio_put(leds[i], 0);
    }

    // Setup Botões com Interrupção
    for (int i = 0; i < BTN_COUNT; i++) {
        gpio_init(BTN_PINS[i]);
        gpio_set_dir(BTN_PINS[i], GPIO_IN);
        gpio_pull_up(BTN_PINS[i]);
        // O callback é compartilhado por todos os pinos
        gpio_set_irq_enabled_with_callback(BTN_PINS[i], GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    }

    int senha_definida[4];
    int senha_testada[4];

    // 1. Fase de Configuração (Acontece uma vez ao ligar)
    for (int i = 0; i < 4; i++) {
        while (pino_pressionado == -1) { tight_loop_contents(); }
        
        senha_definida[i] = pino_pressionado;
        pisca_roxo();
        pino_pressionado = -1; // Reset flag
    }
    
    // LED Verde 300ms indicando que a senha foi salva
    gpio_put(LED_PIN_G, 1);
    sleep_ms(300);
    gpio_put(LED_PIN_G, 0);

    // 2. Loop de Verificação
    while (1) {
        for (int i = 0; i < 4; i++) {
            while (pino_pressionado == -1) { tight_loop_contents(); }
            
            senha_testada[i] = pino_pressionado;
            pisca_roxo();
            pino_pressionado = -1;
        }

        // Verifica senha
        bool correta = true;
        for (int i = 0; i < 4; i++) {
            if (senha_testada[i] != senha_definida[i]) {
                correta = false;
                break;
            }
        }

        if (correta) {
            gpio_put(LED_PIN_G, 1);
            sleep_ms(300);
            gpio_put(LED_PIN_G, 0);
        } else {
            gpio_put(LED_PIN_R, 1);
            sleep_ms(300);
            gpio_put(LED_PIN_R, 0);
        }
    }
}