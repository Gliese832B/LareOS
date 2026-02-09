#include "gpio.h"

void gpio_set_function(uint32_t pin, uint32_t function) {
    uint64_t reg = GPFSEL0 + (pin / 10) * 4;
    uint32_t shift = (pin % 10) * 3;
    uint32_t val = mmio_read(reg);
    val &= ~(7 << shift);
    val |= (function << shift);
    mmio_write(reg, val);
}

void gpio_set(uint32_t pin) {
    if (pin < 32) {
        mmio_write(GPSET0, 1 << pin);
    } else {
        mmio_write(GPSET1, 1 << (pin - 32));
    }
}

void gpio_clear(uint32_t pin) {
    if (pin < 32) {
        mmio_write(GPCLR0, 1 << pin);
    } else {
        mmio_write(GPCLR1, 1 << (pin - 32));
    }
}

uint32_t gpio_get(uint32_t pin) {
    if (pin < 32) {
        return (mmio_read(GPLEV0) >> pin) & 1;
    } else {
        return (mmio_read(GPLEV1) >> (pin - 32)) & 1;
    }
}

void gpio_set_pull(uint32_t pin, uint32_t pull) {
    mmio_write(GPPUD, pull);
    delay(150);
    if (pin < 32) {
        mmio_write(GPPUDCLK0, 1 << pin);
    } else {
        mmio_write(GPPUDCLK1, 1 << (pin - 32));
    }
    delay(150);
    mmio_write(GPPUD, 0);
    if (pin < 32) {
        mmio_write(GPPUDCLK0, 0);
    } else {
        mmio_write(GPPUDCLK1, 0);
    }
}

void gpio_init(void) {
    gpio_set_function(14, GPIO_FUNC_ALT0);
    gpio_set_function(15, GPIO_FUNC_ALT0);
    gpio_set_pull(14, GPIO_PULL_NONE);
    gpio_set_pull(15, GPIO_PULL_NONE);
}
