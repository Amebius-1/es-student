#include "stdio-text-protocol/stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>

static char line[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint32_t line_length = 0;
static bool is_truncated = false;
static command_t cmd;

void stdio_text_protocol_init(void)
{
    line_length = 0;
    is_truncated = false;
}

const command_t *stdio_text_protocol_handle(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    if (symbol == 0x08 || symbol == 0x7F)
    {
        if (line_length > 0)
        {
            line_length--;
            printf("\b \b");
            
            if (line_length < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1) 
            {
                is_truncated = false;
            }
        }
        return NULL;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        printf("\n");

        if (line_length == 0)
        {
            return NULL;
        }

        line[line_length] = '\0';

        cmd.name = NULL;
        cmd.argc = 0;
        cmd.truncated = is_truncated;

        char *p = line;
        while (*p != '\0')
        {
            while (*p == ' ')
            {
                *p = '\0';
                p++;
            }
            
            if (*p == '\0') break;

            if (cmd.name == NULL)
            {
                cmd.name = p;
            }
            else if (cmd.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS)
            {
                cmd.argv[cmd.argc] = p;
                cmd.argc++;
            }
            else
            {
                cmd.truncated = true;
            }

            while (*p != '\0' && *p != ' ')
            {
                p++;
            }
        }

        line_length = 0;
        is_truncated = false;

        return &cmd;
    }

    if (symbol >= 0x20 && symbol <= 0x7e)
    {
        if (line_length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
        {
            line[line_length] = (char)symbol;
            line_length++;
            putchar(symbol);
        }
        else
        {
            is_truncated = true;
        }
    }

    return NULL;
}
