#include "uart_driver.h"
#include <stdarg.h>
#include <stdio.h>

/* LM3S6965 UART0 registers */
#define UART0_BASE          0x4000C000UL
#define UART_DR             (*(volatile uint32_t *)(UART0_BASE + 0x000))
#define UART_FR             (*(volatile uint32_t *)(UART0_BASE + 0x018))
#define UART_IBRD           (*(volatile uint32_t *)(UART0_BASE + 0x024))
#define UART_FBRD           (*(volatile uint32_t *)(UART0_BASE + 0x028))
#define UART_LCRH           (*(volatile uint32_t *)(UART0_BASE + 0x02C))
#define UART_CTL            (*(volatile uint32_t *)(UART0_BASE + 0x030))

/* SYSCTL registers */
#define SYSCTL_RCGC1        (*(volatile uint32_t *)(0x400FE104UL))
#define SYSCTL_RCGC2        (*(volatile uint32_t *)(0x400FE108UL))

/* Flag Register bits */
#define UART_FR_TXFF        (1U << 5)
#define UART_FR_RXFE        (1U << 4)
#define UART_FR_BUSY        (1U << 3)

/* Control register bits */
#define UART_CTL_UARTEN     (1U << 0)
#define UART_CTL_TXE        (1U << 8)
#define UART_CTL_RXE        (1U << 9)

/* Line control: 8N1, FIFO enable */
#define UART_LCRH_WLEN_8    (0x3U << 5)
#define UART_LCRH_FEN       (1U << 4)

/* Baud rate: 115200 @ 12MHz system clock
 * BRD = 12000000 / (16 * 115200) = 6.5104...
 * IBRD = 6, FBRD = round(0.5104 * 64) = 33 */
#define UART_IBRD_115200    6
#define UART_FBRD_115200    33

void uart_init(void)
{
    /* Enable UART0 and GPIOA clocks */
    SYSCTL_RCGC1 |= (1U << 0);
    SYSCTL_RCGC2 |= (1U << 0);

    /* Disable UART before configuration */
    UART_CTL &= ~UART_CTL_UARTEN;

    /* Wait for end of transmission */
    while (UART_FR & UART_FR_BUSY);

    /* Set baud rate */
    UART_IBRD = UART_IBRD_115200;
    UART_FBRD = UART_FBRD_115200;

    /* 8-bit, no parity, 1 stop bit, FIFO enabled */
    UART_LCRH = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    /* Enable UART, TX, RX */
    UART_CTL = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

void uart_putchar(char c)
{
    while (UART_FR & UART_FR_TXFF);
    UART_DR = (uint32_t)c;
}

void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n')
            uart_putchar('\r');
        uart_putchar(*str++);
    }
}

void uart_printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    uart_puts(buf);
}
