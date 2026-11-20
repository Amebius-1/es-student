#include "led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "systime/systime.h"

static led_state_t led_state = LED_STATE_OFF;
static uint32_t current_period_ms = 1000;
static uint64_t half_period_us = 500000;
static uint64_t last_toggle_us = 0;

void led_task_init(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    led_task_set_period_ms(1000);
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    switch (led_state) {
        case LED_STATE_OFF:
            gpio_put(PICO_DEFAULT_LED_PIN, false);
            break;
        case LED_STATE_ON:
            gpio_put(PICO_DEFAULT_LED_PIN, true);
            break;
        case LED_STATE_BLINK:
            last_toggle_us = 0;
            break;
    }
}

led_state_t led_task_get_state(void)
{
    return led_state;
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    if (period_ms == 0) {
        return false;
    }

    current_period_ms = period_ms;
    half_period_us = (uint64_t)period_ms * 1000 / 2;
    return true;
}

uint32_t led_task_get_period_ms(void)
{
    return current_period_ms;
}

void led_task_handle(void)
{
    switch (led_state) {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            break;
        case LED_STATE_BLINK:
            if (systime_period_elapsed(&last_toggle_us, half_period_us)) {
                gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
            }
            break;
    }
}

void led_task_next_state(void)
{
    switch (led_state) {
        case LED_STATE_OFF:   led_task_set_state(LED_STATE_ON); break;
        case LED_STATE_ON:    led_task_set_state(LED_STATE_BLINK); break;
        case LED_STATE_BLINK: led_task_set_state(LED_STATE_OFF); break;
    }
}
