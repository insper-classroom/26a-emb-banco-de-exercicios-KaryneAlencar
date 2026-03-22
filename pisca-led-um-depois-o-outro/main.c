#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

const int LED_PIN_Y = 10;
const int LED_PIN_B = 14;

const int BTN_PIN_Y = 26;
const int BTN_PIN_B = 19;

volatile bool btn_b_press = false;
volatile bool btn_y_press = false;
volatile bool alarme_y = false;
volatile bool alarme_b = false;
volatile bool pisca_y = false;
volatile bool pisca_b = false;

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;
}

bool timer_b_callback(repeating_timer_t *rt) {
    pisca_b = true;
    return true;
}
//periodo, alarme, quando a flag tiver ativa, passou 1s, entao ativa ela no if btnr_press

int64_t alarm_y_callback(alarm_id_t id, void *user_data) {
    alarme_y = true;
    return 0;
}
int64_t alarm_b_callback(alarm_id_t id, void *user_data) {
    alarme_b = true;
    return 0;
}

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){
        if(gpio == BTN_PIN_B){
            btn_b_press = true;
        }
        if(gpio == BTN_PIN_Y){
            btn_y_press = true;
        }  
    }
}

int main()
{
    stdio_init_all();
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_put(LED_PIN_Y, 0);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);
    
    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true);

    repeating_timer_t timer_b;
    repeating_timer_t timer_y;

    bool led_estado_y = false;
    bool led_estado_b = false;

    //azul = 1, amarelo =2
    int estado = 0;
    while (1) {
        if(btn_b_press){
            btn_b_press = false;
            estado = 1;
            add_alarm_in_ms(2000, alarm_b_callback, NULL, false);
            add_repeating_timer_ms(500, timer_b_callback, NULL, &timer_b);
        }

        if(btn_y_press){
            btn_y_press = false;
            estado = 2;
            add_alarm_in_ms(1000, alarm_y_callback, NULL, false);
            add_repeating_timer_ms(200, timer_y_callback, NULL, &timer_y);
        }

        if(pisca_b){
            pisca_b = false;
            led_estado_b = !led_estado_b;
            gpio_put(LED_PIN_B, led_estado_b);
        }
        if(alarme_b){
            gpio_put(LED_PIN_B, 0);
            alarme_b = false;
            cancel_repeating_timer(&timer_b);
            if(estado == 1){
                add_alarm_in_ms(1000, alarm_y_callback, NULL, false);
                add_repeating_timer_ms(200, timer_y_callback, NULL, &timer_y);
            }
        }
        if(pisca_y){
            pisca_y = false;
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
        }
        if(alarme_y){
            gpio_put(LED_PIN_Y, 0);
            alarme_y = false;
            cancel_repeating_timer(&timer_y); 
            if(estado == 2){
                add_alarm_in_ms(1000, alarm_b_callback, NULL, false);
                add_repeating_timer_ms(200, timer_b_callback, NULL, &timer_b);
            }   
        }
    }
        
    return 0;
}