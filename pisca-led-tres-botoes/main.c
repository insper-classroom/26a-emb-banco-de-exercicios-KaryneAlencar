#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const int BTN_PIN_G = 18;
const int BTN_PIN_Y = 28;
const int BTN_PIN_B = 22;

const int LED_PIN_G = 14;
const int LED_PIN_Y = 6;
const int LED_PIN_B = 10;

volatile bool btn_y_press = false;
volatile bool btn_b_solto = false;
volatile bool btn_g_press = false;
volatile bool btn_g_solto = false;

void btn_callback(uint gpio, uint32_t events) {
    if(gpio == BTN_PIN_Y && events == 0x4){
      btn_y_press = true;
    }
    if(gpio == BTN_PIN_B && events == 0x8){
      btn_b_solto = true;
    }
    if(gpio == BTN_PIN_G){
      if(events == 0x4){
        btn_g_press = true;
      }
      if(events == 0x8){
        btn_g_solto = true;
      }
    }
}

int main() {
  stdio_init_all();
  gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, true,
                                       &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled_with_callback(BTN_PIN_B, GPIO_IRQ_EDGE_RISE, true,
                                       &btn_callback);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

  bool estado_led = false;
  bool ativo_y = false;
  bool ativo_b = false;
  bool ativo_g = false;
  
  while (true) {
    estado_led = !estado_led;
    sleep_ms(200);

    if(btn_y_press){
      btn_y_press = false;
      ativo_y = !ativo_y;
    }
    if(btn_b_solto){
      btn_b_solto = false;
      ativo_b = !ativo_b;
    }
    if(btn_g_press){
      btn_g_press = false;
      ativo_g = !ativo_g;
    }
    if(btn_g_solto){
      btn_g_solto = false;
      ativo_g = false;
    }
    gpio_put(LED_PIN_Y, ativo_y ? estado_led : 0);
    gpio_put(LED_PIN_B, ativo_b ? estado_led : 0);
    gpio_put(LED_PIN_G, ativo_g ? estado_led : 0);
    
  }
}