#include "uart.h"
#include "gpio.h"

void uart_init(void) {
    mmio_write(UART0_CR, 0);

    gpio_init();

    mmio_write(UART0_ICR, 0x7FF);
    mmio_write(UART0_IBRD, 26);
    mmio_write(UART0_FBRD, 3);
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(char c) {
    while (mmio_read(UART0_FR) & UART_FR_TXFF) {}
    mmio_write(UART0_DR, c);
}

char uart_getc(void) {
    while (mmio_read(UART0_FR) & UART_FR_RXFE) {}
    return mmio_read(UART0_DR) & 0xFF;
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

bool uart_has_data(void) {
    return !(mmio_read(UART0_FR) & UART_FR_RXFE);
}

void uart_puthex(uint64_t val) {
    const char hex[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uart_putc(hex[(val >> i) & 0xF]);
    }
}

void uart_putint(int64_t val) {
    char buf[21];
    int i = 0;
    bool neg = false;

    if (val < 0) {
        neg = true;
        val = -val;
    }

    if (val == 0) {
        uart_putc('0');
        return;
    }

    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    if (neg) uart_putc('-');

    while (i > 0) {
        uart_putc(buf[--i]);
    }
}

void uart_putuint(uint64_t val) {
    char buf[21];
    int i = 0;

    if (val == 0) {
        uart_putc('0');
        return;
    }

    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    while (i > 0) {
        uart_putc(buf[--i]);
    }
}
