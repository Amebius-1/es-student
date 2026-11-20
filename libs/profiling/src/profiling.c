#include "profiling/profiling.h"
#include "pico/stdlib.h"

#if PROFILING_ENABLED

typedef struct {
    const char *name;
    uint32_t start_us;
    uint64_t total_us;
    uint32_t count;
    uint32_t max;
} stopwatch_t;

static stopwatch_t stopwatches[PROFILING_COUNT];

void profiling_stopwatch_init(uint32_t id, const char *name)
{
    if (id >= PROFILING_COUNT) return;
    stopwatches[id].name = name;
    stopwatches[id].total_us = 0;
    stopwatches[id].count = 0;
    stopwatches[id].max = 0;
}

void profiling_start(uint32_t id)
{
    if (id >= PROFILING_COUNT) return;
    stopwatches[id].start_us = time_us_32();
}

void profiling_stop(uint32_t id)
{
    if (id >= PROFILING_COUNT) return;
    
    uint32_t now_us = time_us_32();
    uint32_t diff_us = now_us - stopwatches[id].start_us;

    stopwatches[id].total_us += diff_us;
    stopwatches[id].count++;
    
    if (diff_us > stopwatches[id].max) {
        stopwatches[id].max = diff_us;
    }
}

bool profiling_get(uint32_t id, profiling_result_t *result)
{
    if (id >= PROFILING_COUNT) return false;
    if (stopwatches[id].count == 0) return false;

    result->name = stopwatches[id].name;
    result->count = stopwatches[id].count;
    result->mean = (double)stopwatches[id].total_us / stopwatches[id].count;
    result->max = stopwatches[id].max;
    
    return true;
}

void profiling_reset(void)
{
    for (int i = 0; i < PROFILING_COUNT; i++) {
        stopwatches[i].total_us = 0;
        stopwatches[i].count = 0;
        stopwatches[i].max = 0;
    }
}

#endif
