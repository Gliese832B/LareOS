#include "timer.h"
#include "irq.h"

static volatile uint64_t system_ticks = 0;

void timer_init(void) {
    system_ticks = 0;
    uint32_t cur = mmio_read(TIMER_CLO);
    mmio_write(TIMER_C1, cur + TICK_INTERVAL);
    irq_enable(IRQ_TIMER1);
}

uint64_t timer_get_ticks(void) {
    uint32_t hi = mmio_read(TIMER_CHI);
    uint32_t lo = mmio_read(TIMER_CLO);
    if (hi != mmio_read(TIMER_CHI)) {
        hi = mmio_read(TIMER_CHI);
        lo = mmio_read(TIMER_CLO);
    }
    return ((uint64_t)hi << 32) | lo;
}

void timer_sleep(uint32_t ms) {
    uint64_t target = timer_get_ticks() + (uint64_t)ms * 1000;
    while (timer_get_ticks() < target) {
        asm volatile("nop");
    }
}

void timer_handle_irq(void) {
    system_ticks++;
    uint32_t cur = mmio_read(TIMER_CLO);
    mmio_write(TIMER_C1, cur + TICK_INTERVAL);
    mmio_write(TIMER_CS, TIMER_CS_M1);
}

uint64_t timer_get_uptime_seconds(void) {
    return system_ticks / 100;
}
