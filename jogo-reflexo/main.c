#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int LED_PIN_G = 5;
const int LED_PIN_Y = 9;
const int LED_PIN_R = 13;
const int BTN_PIN_G = 28;
const int BTN_PIN_Y = 26;
const int BTN_PIN_R = 20;

volatile bool inicia = false;
volatile int botao_pressionado = -1; // como vou precisar verificar se o botao ja foi precionado comparando com uma sequencia ja estabelida, isso é melhor q fazer flag

// 0 = verde, 1 = amarelo, 2 = vermelho
void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){
        if(gpio == BTN_PIN_G){
            inicia = true;
            botao_pressionado = 0;
        }
        if(gpio == BTN_PIN_R){
            botao_pressionado = 2;
        }
        if(gpio == BTN_PIN_Y){
            botao_pressionado = 1;
        }
        
    }
}

void acender_led(int cor){

    if(cor == 0){
        gpio_put(LED_PIN_G, 1);
    }
    if(cor == 1){
        gpio_put(LED_PIN_Y, 1);
    }
    if(cor == 2){
        gpio_put(LED_PIN_R, 1);
    }
    sleep_ms(300);
    gpio_put(LED_PIN_G, 0);
    gpio_put(LED_PIN_Y, 0);
    gpio_put(LED_PIN_R, 0);
    sleep_ms(300);
}

int main() {
    stdio_init_all();

    // LED verde
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    // LED amarelo
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    // LED vermelho
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    // Botão verde
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G); // ativa pull-up interno

    // Botão amarelo
    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);

    // Botão vermelho
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true);

    // 0 = verde, 1 = amarelo, 2 = vermelho
    const char sequencia[] = {1,0,2,1,0,1,2,1,0,1};
    while (true) {
        if(inicia){
            int pontos = 0;
            for(int nivel = 1; nivel<=10; nivel++){ //nivel
                for(int i=0; i < nivel; i++){
                    acender_led(sequencia[i]);
                }
                for(int j=0; j < nivel; j++){
                    botao_pressionado = -1;
                    while(botao_pressionado == -1);
                    if(botao_pressionado != sequencia[j]){
                        printf("Points %d\n", pontos);
                        inicia = false;
                        break;
                    }
                }
                if(!inicia){
                    break;
                }

                pontos++;
                sleep_ms(500);
            }

        }
        inicia = false;
    }
}
