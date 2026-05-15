#include <stdint.h>

/* Provided by linker script */
extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss,  _ebss;

extern int main(void);

/* Forward declarations */
void Reset_Handler(void);
static void Default_Handler(void);

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));

/* FreeRTOS handlers — defined by port */
void SVC_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

/* Vector table */
__attribute__((section(".isr_vector")))
static void (* const vector_table[])(void) = {
    (void (*)(void))&_estack,   /* Initial stack pointer */
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,                 /* Reserved */
    SVC_Handler,
    0, 0,                       /* Reserved */
    PendSV_Handler,
    SysTick_Handler,
};

void Reset_Handler(void)
{
    /* Copy .data from flash to SRAM */
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *dst++ = *src++;

    /* Zero-fill .bss */
    dst = &_sbss;
    while (dst < &_ebss)
        *dst++ = 0;

    main();

    for (;;);
}

static void Default_Handler(void)
{
    for (;;);
}
