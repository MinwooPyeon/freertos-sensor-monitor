#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>

void uart_init(void);
void uart_putchar(char c);
void uart_puts(const char *str);
void uart_printf(const char *fmt, ...);

#endif /* UART_DRIVER_H */
