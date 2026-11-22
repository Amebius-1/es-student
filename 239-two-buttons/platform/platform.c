#include "platform/platform.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BUTTON_0_PIN 15
#define BUTTON_1_PIN 14

void platform_init(void)
{
    stdio_init_all();
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    gpio_init(BUTTON_0_PIN);
    gpio_set_dir(BUTTON_0_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_0_PIN);
    
    gpio_init(BUTTON_1_PIN);
    gpio_set_dir(BUTTON_1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_1_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(PICO_DEFAULT_LED_PIN, on);
}

bool platform_button_read(uint8_t button_id)
{
    if (button_id == 0) return !gpio_get(BUTTON_0_PIN);
    if (button_id == 1) return !gpio_get(BUTTON_1_PIN);
    return false;
}
