#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const int LED_PIN_R = 2;
const int LED_PIN_G = 9;
const int LED_PIN_P = 26;

const int BTN_PIN_Y = 21;
const int BTN_PIN_B = 27;
const int BTN_PIN_G = 28;
const int BTN_PIN_W = 17;

volatile bool btn_b_press = false;
volatile bool btn_y_press = false;
volatile bool btn_g_press = false;
volatile bool btn_w_press = false;

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){
        if(gpio == BTN_PIN_B){
            btn_b_press = true;
        }
        if(gpio == BTN_PIN_Y){
            btn_y_press = true;
        }  
        if(gpio == BTN_PIN_W){
            btn_w_press = true;
        } 
        if(gpio == BTN_PIN_G){
            btn_g_press = true;
        } 
    }
}


int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(LED_PIN_P);
    gpio_set_dir(LED_PIN_P, GPIO_OUT);
    gpio_put(LED_PIN_P, 0);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);
    
    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BTN_PIN_W);
    gpio_set_dir(BTN_PIN_W, GPIO_IN);
    gpio_pull_up(BTN_PIN_W);
    gpio_set_irq_enabled(BTN_PIN_W, GPIO_IRQ_EDGE_FALL, true);

    //recebe uma senha - atribuir cada cor a um número
//se o tamanho passar de 4, usuário ja deve por a senha
//led roxo sempre acende
//led vere acende se acertar(300ms)
//led vermelho se errar(300ms)

    int senha_definida[4];
    int senha_testada[4];
    bool rodando = false;
    bool perdeu = false;

    while (1) {
        if(!rodando){
            rodando = true;
            for(int i = 0; i < 4; i++){
                while(!btn_b_press && !btn_g_press && !btn_y_press && !btn_w_press){
                    tight_loop_contents(); // espera
                }
                if(btn_b_press){
                    gpio_put(LED_PIN_P, 1);
                    sleep_ms(50);
                    gpio_put(LED_PIN_P, 0);
                    btn_b_press = false;
                    senha_definida[i]=1;
                }
                if(btn_g_press){
                    gpio_put(LED_PIN_P, 1);
                    sleep_ms(50);
                    gpio_put(LED_PIN_P, 0);
                    btn_g_press = false;
                    senha_definida[i]=0;
                }
                if(btn_y_press){
                    gpio_put(LED_PIN_P, 1);
                    sleep_ms(50);
                    gpio_put(LED_PIN_P, 0);
                    btn_y_press = false;
                    senha_definida[i]=2;
                }
                if(btn_w_press){
                    gpio_put(LED_PIN_P, 1);
                    sleep_ms(50);
                    gpio_put(LED_PIN_P, 0);
                    btn_w_press = false;
                    senha_definida[i]=3;
                }
            }
            gpio_put(LED_PIN_G, 1);
            sleep_ms(50);
            gpio_put(LED_PIN_G, 0);
        }
        
        for(int i = 0; i < 4; i++){
            while(!btn_b_press && !btn_g_press && !btn_y_press && !btn_w_press){
                tight_loop_contents(); // espera
            }
            if(btn_b_press){
                gpio_put(LED_PIN_P, 1);
                sleep_ms(50);
                gpio_put(LED_PIN_P, 0);
                btn_b_press = false;
                senha_testada[i]=1;
            }
            if(btn_g_press){
                gpio_put(LED_PIN_P, 1);
                sleep_ms(50);
                gpio_put(LED_PIN_P, 0);
                btn_g_press = false;
                senha_testada[i]=0;
            }
            if(btn_y_press){
                gpio_put(LED_PIN_P, 1);
                sleep_ms(50);
                gpio_put(LED_PIN_P, 0);
                btn_y_press = false;
                senha_testada[i]=2;
            }
            if(btn_w_press){
                gpio_put(LED_PIN_P, 1);
                sleep_ms(50);
                gpio_put(LED_PIN_P, 0);
                btn_w_press = false;
                senha_testada[i]=3;
            }
        }


        
        perdeu = false;
        for(int i = 0; i < 4; i ++){
            if(senha_definida[i] != senha_testada[i]){
                perdeu = true;
            }    
        }
        

        if(perdeu){
            gpio_put(LED_PIN_R, 1);
            sleep_ms(50);
            gpio_put(LED_PIN_R, 0);
        } 
        else{
            gpio_put(LED_PIN_G, 1);
            sleep_ms(50);
            gpio_put(LED_PIN_G, 0);
        }

    }
}
