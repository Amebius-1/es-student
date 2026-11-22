#include "button-task/button-task.h"
#include "platform/platform.h"
#include "systime/systime.h"

#include <stddef.h>

#define DEBOUNCE_TIME_US 20000
#define LONG_PRESS_TIME_US 1000000

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASING
} button_state_t;

typedef struct {
    button_state_t state;
    uint32_t state_start_us;
    uint32_t press_count;
    button_task_callback_t on_press_cb;
    button_task_callback_t on_long_press_cb;
    bool long_reported;
} button_t;

static button_t buttons[PLATFORM_BUTTON_COUNT];

void button_task_init(uint8_t button_id, button_task_callback_t on_press, button_task_callback_t on_long_press)
{
    if (button_id >= PLATFORM_BUTTON_COUNT) return;
    
    buttons[button_id].state = BUTTON_STATE_RELEASED;
    buttons[button_id].press_count = 0;
    buttons[button_id].on_press_cb = on_press;
    buttons[button_id].on_long_press_cb = on_long_press;
    buttons[button_id].long_reported = false;
}

void button_task_handle(void)
{
    uint32_t now_us = (uint32_t)systime_us();

    for (uint8_t i = 0; i < PLATFORM_BUTTON_COUNT; i++) {
        bool is_pressed = platform_button_read(i);

        switch (buttons[i].state)
        {
            case BUTTON_STATE_RELEASED:
                if (is_pressed) {
                    buttons[i].state_start_us = now_us;
                    buttons[i].state = BUTTON_STATE_PRESSING;
                }
                break;

            case BUTTON_STATE_PRESSING:
                if (!is_pressed) {
                    buttons[i].state = BUTTON_STATE_RELEASED;
                } else if (now_us - buttons[i].state_start_us >= DEBOUNCE_TIME_US) {
                    buttons[i].state = BUTTON_STATE_PRESSED;
                    buttons[i].state_start_us = now_us;
                    
                    if (buttons[i].on_long_press_cb == NULL) {
                        buttons[i].press_count++;
                        if (buttons[i].on_press_cb != NULL) buttons[i].on_press_cb();
                    }
                }
                break;

            case BUTTON_STATE_PRESSED:
                if (!is_pressed) {
                    buttons[i].state = BUTTON_STATE_RELEASING;
                    buttons[i].state_start_us = now_us;
                } else if (!buttons[i].long_reported && (now_us - buttons[i].state_start_us >= LONG_PRESS_TIME_US)) {
                    if (buttons[i].on_long_press_cb != NULL) buttons[i].on_long_press_cb();
                    buttons[i].long_reported = true;
                }
                break;

            case BUTTON_STATE_RELEASING:
                if (is_pressed) {
                    buttons[i].state = BUTTON_STATE_PRESSED;
                } else if (now_us - buttons[i].state_start_us >= DEBOUNCE_TIME_US) {
                    buttons[i].state = BUTTON_STATE_RELEASED;
                    
                    if (buttons[i].on_long_press_cb != NULL && !buttons[i].long_reported) {
                        buttons[i].press_count++;
                        if (buttons[i].on_press_cb != NULL) buttons[i].on_press_cb();
                    }
                    buttons[i].long_reported = false;
                }
                break;
        }
    }
}


bool button_task_is_pressed(uint8_t button_id)
{
    if (button_id >= PLATFORM_BUTTON_COUNT) return false;
    return (buttons[button_id].state == BUTTON_STATE_PRESSED || buttons[button_id].state == BUTTON_STATE_RELEASING);
}

uint32_t button_task_get_press_count(uint8_t button_id)
{
    if (button_id >= PLATFORM_BUTTON_COUNT) return 0;
    return buttons[button_id].press_count;
}
