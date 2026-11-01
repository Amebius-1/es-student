#include "memory.h"

#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"

#include "command.h"
#include <stdlib.h>

#include "device.h"

#include "led.h"

int main(void);

uint32_t data_variable = 100;
uint32_t bss_variable;

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    uintptr_t boot2_size = (uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__;
    uintptr_t text_size  = (uintptr_t)&__etext - (uintptr_t)&__boot2_end__;
    uintptr_t data_size  = (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__;
    uintptr_t bss_size   = (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__;

    uintptr_t image_size = (uintptr_t)&__flash_binary_end - (uintptr_t)&__flash_binary_start;
    uintptr_t flash_free_size = (XIP_BASE + PICO_FLASH_SIZE_BYTES) - (uintptr_t)&__flash_binary_end;

    uintptr_t ram_used_size = data_size + bss_size;
    uintptr_t heap_size = (uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__;
    uintptr_t stack_size = (uintptr_t)&__StackTop - (uintptr_t)&__StackBottom;

    printf("%-10s %-10s %-10s %8s\n", "area", "start", "end", "size");

    row("flash", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("sram", SRAM_BASE, SRAM_END);
    row("rom", ROM_BASE, ROM_BASE + 16 * 1024);

    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    row("free", (uintptr_t)&__flash_binary_end, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);
    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext + data_size);

    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    printf("\ntotal\n");

    printf("  flash image  %7u = boot2 %u + text %u + data %u\n", 
           (unsigned)image_size, (unsigned)boot2_size, (unsigned)text_size, (unsigned)data_size);
           
    printf("  flash free   %7u of %u\n", 
           (unsigned)flash_free_size, (unsigned)PICO_FLASH_SIZE_BYTES);
           
    printf("  ram used     %7u = data %u + bss %u\n", 
           (unsigned)ram_used_size, (unsigned)data_size, (unsigned)bss_size);
           
    printf("  ram free     %7u for heap and %u for stack\n", 
           (unsigned)heap_size, (unsigned)stack_size);
}

void fw_info(void)
{
    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    uint16_t *fw_info_code = (uint16_t *)((uintptr_t)&fw_info & ~1u);

    data_variable++;
    bss_variable++;

    printf("%-20s %-10s %-10s\n", "object", "address", "value");

    printf("%-20s 0x%08x 0x%04x\n", "main", (unsigned)(uintptr_t)&main, *main_code);
    printf("%-20s 0x%08x 0x%04x\n", "fw_info", (unsigned)(uintptr_t)&fw_info, *fw_info_code);
    
    printf("%-20s 0x%08x\n", "commands", (unsigned)(uintptr_t)commands);

    for (uint i = 0; i < command_count; i++)
    {
       printf("- %-18s 0x%08x\n", commands[i].name, (unsigned)(uintptr_t)commands[i].handler);
    }

    printf("%-20s 0x%08x %s\n", "DEVICE_PROJECT", (unsigned)(uintptr_t)DEVICE_PROJECT, DEVICE_PROJECT);
    printf("%-20s 0x%08x %s\n", "DEVICE_BOARD", (unsigned)(uintptr_t)DEVICE_BOARD, DEVICE_BOARD);

    printf("%-20s 0x%08x %u\n", "data_variable", (unsigned)(uintptr_t)&data_variable, data_variable);
    printf("%-20s 0x%08x %u\n", "bss_variable", (unsigned)(uintptr_t)&bss_variable, bss_variable);
    printf("%-20s 0x%08x %u\n", "stack_variable", (unsigned)(uintptr_t)&stack_variable, stack_variable);
    
    if (heap_variable != NULL)
    {
        printf("%-20s 0x%08x %u\n", "heap_variable", (unsigned)(uintptr_t)heap_variable, *heap_variable);
    }

    free(heap_variable);
}

void boot_info(void)
{
    #define VECTOR_TABLE 0x10000100
    const uint32_t *vectors = (const uint32_t *)VECTOR_TABLE;
    
    uint32_t stack_top = vectors[0];
    uint32_t reset_handler = vectors[1];

    volatile uint32_t *gpio_in = (volatile uint32_t *)0xd0000004;
    
    uint32_t level = (*gpio_in >> led_pin()) & 1u; 

    printf("%-15s 0x%08x\n", "vector table", VECTOR_TABLE);
    printf("%-15s 0x%08x\n", "  stack top", stack_top);
    printf("%-15s 0x%08x\n", "  reset", reset_handler);
    printf("%-15s 0x%08x\n", "  reset (even)", reset_handler & ~1u);
    printf("%-15s 0x%08x\n", "gpio in", (unsigned)(uintptr_t)gpio_in);
    printf("%-15s %u\n", "  led bit", level);
    printf("%-15s %u\n", "  gpio_get", gpio_get(led_pin()));
}