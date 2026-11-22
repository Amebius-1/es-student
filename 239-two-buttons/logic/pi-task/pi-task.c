#include "pi-task.h"
#include "systime/systime.h"

// Расчёт: 1 000 000 членов занимают 4.7 с (4 700 000 мкс).
// Один член = 4.7 мкс. Для порога 0.5 мс (500 мкс) нужно 500 / 4.7 ≈ 106 членов.
// Берём 100, чтобы гарантированно уложиться в 0.5 мс.
#define PI_TERMS_PER_CALL 100

static pi_state_t state = PI_STATE_NOT_STARTED;

static uint32_t pi_n;
static uint32_t pi_k;
static double pi_sum;
static double pi_sign;
static double pi_result;

static uint32_t start_us;
static uint32_t end_us;

void pi_task_init(void)
{
    state = PI_STATE_NOT_STARTED;
}

bool pi_task_start(uint32_t terms)
{
    if (state == PI_STATE_RUNNING) {
        return false;
    }

    pi_n = terms;
    pi_k = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_result = 0.0;

    start_us = (uint32_t)systime_us();
    state = PI_STATE_RUNNING;

    return true;
}

void pi_task_handle(void)
{
    if (state != PI_STATE_RUNNING) {
        return;
    }

    uint32_t end = pi_k + PI_TERMS_PER_CALL;
    
    if (end < pi_k) {
        end = pi_n;
    }
    if (end > pi_n) {
        end = pi_n;
    }

    for (; pi_k < end; pi_k++) {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k == pi_n) {
        pi_result = 4.0 * pi_sum;
        end_us = (uint32_t)systime_us();
        state = PI_STATE_READY;
    }
}

bool pi_task_get_result(double *result, uint32_t *time_ms)
{
    if (state != PI_STATE_READY) {
        return false;
    }

    *result = pi_result;
    *time_ms = (end_us - start_us) / 1000;
    return true;
}

pi_state_t pi_task_get_state(void)
{
    return state;
}
