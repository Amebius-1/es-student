#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PLATFORM_BUTTON_LED    0
#define PLATFORM_BUTTON_PERIOD 1

#define PLATFORM_BUTTON_COUNT  2

void platform_init(void);

void platform_led_set(bool on);

bool platform_button_read(uint8_t button_id);
