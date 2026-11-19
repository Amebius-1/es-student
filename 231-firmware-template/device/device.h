#pragma once

#include <stdint.h>

#define DEVICE_NAME "es-fw-template"
#define FIRMWARE_VERSION "1.2.0"

#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/Amebius-1/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

typedef struct {
    const char *board;
    char serial[17];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    const char *sdk_version;
} device_info_t;

void device_get_info(device_info_t *info);
