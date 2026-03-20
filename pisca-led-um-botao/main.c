
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

const int BTN_PIN = 28;
const int LED_PIN_Y = 5;
const int LED_PIN_B = 9;

volatile bool btn_press = false;
volatile bool pisca_y = false;
volatile bool pisca_b = false;
volatile bool alarme = false;

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;
}

bool timer_b_callback(repeating_timer_t *rt) {
    pisca_b = true;
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;
}

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){
        btn_press = true;
        
    }

}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_put(LED_PIN_Y, 0);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);
    
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    repeating_timer_t timer_b;
    repeating_timer_t timer_y;

    bool led_estado_y = false;
    bool led_estado_b = false;

    while (true) {
        if(btn_press){
            btn_press = false;
            add_alarm_in_ms(5000, alarm_callback, NULL, false);
            add_repeating_timer_ms(500, timer_b_callback, NULL, &timer_b);
            add_repeating_timer_ms(150, timer_y_callback, NULL, &timer_y);
        }
        
        if(pisca_y){
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
            pisca_y = false;
        }
        if(pisca_b){
            led_estado_b = !led_estado_b;
            gpio_put(LED_PIN_B, led_estado_b);
            pisca_b = false;
        }
        if(alarme){
            alarme = false;
            gpio_put(LED_PIN_B, 0);
            gpio_put(LED_PIN_Y, 0);
            cancel_repeating_timer(&timer_y);
            cancel_repeating_timer(&timer_b);
        }

    

    }
}
