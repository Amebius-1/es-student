#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PI_STATE_NOT_STARTED,
    PI_STATE_RUNNING,
    PI_STATE_READY
} pi_state_t;

void pi_task_init(void);
void pi_task_handle(void);

bool pi_task_start(uint32_t terms);
bool pi_task_get_result(double *result, uint32_t *time_ms);

pi_state_t pi_task_get_state(void);
