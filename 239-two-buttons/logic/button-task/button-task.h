#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*button_task_callback_t)(void);

void button_task_init(uint8_t button_id, button_task_callback_t on_press, button_task_callback_t on_long_press);
void button_task_handle(void);

bool button_task_is_pressed(uint8_t button_id);
uint32_t button_task_get_press_count(uint8_t button_id);
