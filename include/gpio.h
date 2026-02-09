#ifndef GPIO_H
#define GPIO_H

#include "lareos.h"

#define GPFSEL0         (GPIO_BASE + 0x00)
#define GPFSEL1         (GPIO_BASE + 0x04)
#define GPFSEL2         (GPIO_BASE + 0x08)
#define GPFSEL3         (GPIO_BASE + 0x0C)
#define GPFSEL4         (GPIO_BASE + 0x10)
#define GPFSEL5         (GPIO_BASE + 0x14)
#define GPSET0          (GPIO_BASE + 0x1C)
#define GPSET1          (GPIO_BASE + 0x20)
#define GPCLR0          (GPIO_BASE + 0x28)
#define GPCLR1          (GPIO_BASE + 0x2C)
#define GPLEV0          (GPIO_BASE + 0x34)
#define GPLEV1          (GPIO_BASE + 0x38)
#define GPPUD           (GPIO_BASE + 0x94)
#define GPPUDCLK0       (GPIO_BASE + 0x98)
#define GPPUDCLK1       (GPIO_BASE + 0x9C)

#define GPIO_FUNC_INPUT  0
#define GPIO_FUNC_OUTPUT 1
#define GPIO_FUNC_ALT0   4
#define GPIO_FUNC_ALT1   5
#define GPIO_FUNC_ALT2   6
#define GPIO_FUNC_ALT3   7
#define GPIO_FUNC_ALT4   3
#define GPIO_FUNC_ALT5   2

#define GPIO_PULL_NONE   0
#define GPIO_PULL_DOWN   1
#define GPIO_PULL_UP     2

void gpio_set_function(uint32_t pin, uint32_t function);
void gpio_set(uint32_t pin);
void gpio_clear(uint32_t pin);
uint32_t gpio_get(uint32_t pin);
void gpio_set_pull(uint32_t pin, uint32_t pull);
void gpio_init(void);

#endif
