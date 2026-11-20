#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "api-commands.h"

#include "device/device.h"
#include "profiling/profiling.h"
#include "systime/systime.h"

#include "firmware.h"

#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"

#include "pico/stdlib.h"

static bool parse_u32(const char *text, uint32_t *value)
{
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}

static void cmd_info(const command_t *command)
{
    (void)command;
    
    device_info_t info;
    device_get_info(&info);

    printf("name: %s\n", FIRMWARE_NAME);
    printf("version: %s\n", FIRMWARE_VERSION);
    printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);

    printf("board: %s\n", info.board);
    printf("serial: %s\n", info.serial);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", (unsigned)info.manufacturer, (unsigned)info.part, (unsigned)info.revision);
    printf("pico-sdk: %s\n", info.sdk_version);
}

static void cmd_uptime(const command_t *command)
{
    (void)command;
    printf("uptime: %llu ms\n", systime_ms());
}

static void cmd_pi_start(const command_t *command)
{
    uint32_t terms = 1000000;

    if (command->argc > 1) {
        printf("error: usage pi_start [<terms>]\n");
        return;
    }

    if (command->argc == 1) {
        if (!parse_u32(command->argv[0], &terms)) {
            printf("error: usage pi_start [<terms>]\n");
            return;
        }
        if (terms == 0) {
            printf("error: terms must be greater than 0\n");
            return;
        }
    }

    if (!pi_task_start(terms)) {
        printf("error: pi is busy\n");
        return;
    }

    printf("pi: started, %u terms\n", (unsigned)terms);
}

static void cmd_pi(const command_t *command)
{
    (void)command;
    double result;
    uint32_t time_ms;

    if (pi_task_get_result(&result, &time_ms)) {
        printf("pi: %.9f in %u ms\n", result, (unsigned)time_ms);
    } else {
        pi_state_t state = pi_task_get_state();
        if (state == PI_STATE_RUNNING) {
            printf("pi: running\n");
        } else {
            printf("pi: not started\n");
        }
    }
}

static void print_led_state(void)
{
    led_state_t state = led_task_get_state();
    const char *state_str = "off";
    
    if (state == LED_STATE_ON) {
        state_str = "on";
    } else if (state == LED_STATE_BLINK) {
        state_str = "blink";
    }
    
    printf("led: %s, period %u ms\n", state_str, (unsigned)led_task_get_period_ms());
}

static void cmd_led_enable(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

static void cmd_led_disable(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

static void cmd_led_blink(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

static void cmd_led_period(const command_t *command)
{
    uint32_t period_ms;

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    print_led_state();
}

static void cmd_button(const command_t *command)
{
    (void)command;
    const char *state_str = button_task_is_pressed() ? "pressed" : "released";
    printf("button: %s, presses %u\n", state_str, (unsigned)button_task_get_press_count());
}

static void cmd_profiling(const command_t *command)
{
    (void)command;

    #if !PROFILING_ENABLED
        printf("profiling is disabled in this build\n");
        return;
    #endif
    
    profiling_result_t res;
    
    for (uint32_t i = 0; i < PROFILING_COUNT; i++) {
        if (profiling_get(i, &res)) {
            printf("%-12s count %-8u mean %.2f us  max %u us\n", res.name, (unsigned)res.count, res.mean, (unsigned)res.max);
        }
    }
}

static void cmd_profiling_reset(const command_t *command)
{
    (void)command;
    profiling_reset();
    printf("profiling reset\n");
}

const api_command_t api_commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime },
    { "led_enable", "turn led on", cmd_led_enable },
    { "led_disable", "turn led off", cmd_led_disable },
    { "led_blink", "turn led blink", cmd_led_blink },
    { "led_period", "set blink period: led_period <period_ms>", cmd_led_period },
    { "button", "button state and press count", cmd_button },
    { "pi_start", "start pi calculation: pi_start [<terms>]", cmd_pi_start },
    { "pi", "pi calculation result or status", cmd_pi },
    { "profiling", "print stopwatch results", cmd_profiling },
    { "profiling_reset", "reset stopwatch results", cmd_profiling_reset },
};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);
