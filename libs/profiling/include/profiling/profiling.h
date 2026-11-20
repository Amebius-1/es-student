#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PROFILING_COUNT 5

typedef struct {
    const char *name;
    uint32_t count;
    double mean;
    uint32_t max;
} profiling_result_t;

#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif

#if PROFILING_ENABLED

void profiling_stopwatch_init(uint32_t id, const char *name);
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);
#else

#define profiling_stopwatch_init(id, name) ((void)0)
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#define profiling_get(id, result) (false)
#define profiling_reset() ((void)0)
#endif
