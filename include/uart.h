#ifndef UART_H
#define UART_H

#include "lareos.h"

#define UART0_DR        (UART0_BASE + 0x00)
#define UART0_FR        (UART0_BASE + 0x18)
#define UART0_IBRD      (UART0_BASE + 0x24)
#define UART0_FBRD      (UART0_BASE + 0x28)
#define UART0_LCRH      (UART0_BASE + 0x2C)
#define UART0_CR        (UART0_BASE + 0x30)
#define UART0_IFLS      (UART0_BASE + 0x34)
#define UART0_IMSC      (UART0_BASE + 0x38)
#define UART0_RIS       (UART0_BASE + 0x3C)
#define UART0_MIS       (UART0_BASE + 0x40)
#define UART0_ICR       (UART0_BASE + 0x44)

#define UART_FR_RXFE    (1 << 4)
#define UART_FR_TXFF    (1 << 5)
#define UART_FR_BUSY    (1 << 3)

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *s);
void uart_puthex(uint64_t val);
void uart_putint(int64_t val);
void uart_putuint(uint64_t val);
bool uart_has_data(void);

#endif
