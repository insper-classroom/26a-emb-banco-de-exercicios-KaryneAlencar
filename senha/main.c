#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Definições de Pinos
const int LED_PIN_R = 2;
const int LED_PIN_G = 9;
const int LED_PIN_P = 26;

const int BTN_Y = 21;
const int BTN_B = 27;
const int BTN_G = 28;
const int BTN_W = 17;

// Variável volátil para comunicação entre ISR e Main
// -1: Nenhum botão pressionado | 0-3: Índice do botão
volatile int botao_detectado = -1;

// ISR Ultra-rápida: Sem loops, sem delays, sem printf
void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_Y) botao_detectado = 0;
    else if (gpio == BTN_B) botao_detectado = 1;
    else if (gpio == BTN_G) botao_detectado = 2;
    else if (gpio == BTN_W) botao_detectado = 3;
}

// Função auxiliar para o LED roxo (fora da ISR)
void sinalizar_pressao() {
    gpio_put(LED_PIN_P, 1);
    sleep_ms(100); 
    gpio_put(LED_PIN_P, 0);
}

int main() {
    // Inicialização de LEDs
    int leds[] = {LED_PIN_R, LED_PIN_G, LED_PIN_P};
    for(int i = 0; i < 3; i++) {
        gpio_init(leds[i]);
        gpio_set_dir(leds[i], GPIO_OUT);
    }

    // Configuração de Botões e Interrupções
    int botoes[] = {BTN_Y, BTN_B, BTN_G, BTN_W};
    for(int i = 0; i < 4; i++) {
        gpio_init(botoes[i]);
        gpio_set_dir(botoes[i], GPIO_IN);
        gpio_pull_up(botoes[i]);
        // Registra o callback apenas uma vez, habilita para os outros
        if (i == 0) gpio_set_irq_enabled_with_callback(botoes[i], GPIO_IRQ_EDGE_FALL, true, &btn_callback);
        else gpio_set_irq_enabled(botoes[i], GPIO_IRQ_EDGE_FALL, true);
    }

    int senha_mestra[4];
    int tentativa[4];

    // FASE 1: Gravar Senha
    for (int i = 0; i < 4; i++) {
        while (botao_detectado == -1) { tight_loop_contents(); }
        
        senha_mestra[i] = botao_detectado;
        botao_detectado = -1; // Limpa para a próxima leitura
        sinalizar_pressao();
    }

    // LED Verde 300ms: Senha configurada
    gpio_put(LED_PIN_G, 1);
    sleep_ms(300);
    gpio_put(LED_PIN_G, 0);

    // FASE 2: Loop de Verificação
    while (1) {
        for (int i = 0; i < 4; i++) {
            while (botao_detectado == -1) { tight_loop_contents(); }
            
            tentativa[i] = botao_detectado;
            botao_detectado = -1;
            sinalizar_pressao();
        }

        // Validação
        bool acertou = true;
        for (int i = 0; i < 4; i++) {
            if (tentativa[i] != senha_mestra[i]) {
                acertou = false;
                break;
            }
        }

        // Feedback visual (300ms conforme requisito)
        int led_resultado = acertou ? LED_PIN_G : LED_PIN_R;
        gpio_put(led_resultado, 1);
        sleep_ms(300);
        gpio_put(led_resultado, 0);
    }
}