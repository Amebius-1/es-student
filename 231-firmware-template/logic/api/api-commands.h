#pragma once

#include "stdio-text-protocol/stdio-text-protocol.h"
#include <stdint.h>

typedef void (*api_callback_t)(const command_t *command);

typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

extern const api_command_t api_commands[];
extern const uint32_t api_command_count;
