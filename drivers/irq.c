#include "irq.h"
#include "timer.h"
#include "uart.h"
#include "task.h"

extern void *exception_vector_table;

void irq_init(void) {
    disable_irq();
    put_exception_vector(&exception_vector_table);
    mmio_write(IRQ_DISABLE_1, 0xFFFFFFFF);
    mmio_write(IRQ_DISABLE_2, 0xFFFFFFFF);
    mmio_write(IRQ_DISABLE_BASIC, 0xFFFFFFFF);
    enable_irq();
}

void irq_enable(uint32_t irq) {
    if (irq < 32) {
        mmio_write(IRQ_ENABLE_1, irq);
    } else {
        mmio_write(IRQ_ENABLE_2, irq >> 5);
    }
}

void irq_disable(uint32_t irq) {
    if (irq < 32) {
        mmio_write(IRQ_DISABLE_1, irq);
    } else {
        mmio_write(IRQ_DISABLE_2, irq >> 5);
    }
}

uint64_t handle_irq_schedule(uint64_t sp) {
    uint32_t pending = mmio_read(IRQ_PENDING_1);

    if (pending & IRQ_TIMER1) {
        timer_handle_irq();
        sp = task_schedule(sp);
    }

    return sp;
}

uint64_t handle_sync_svc(uint64_t sp) {
    uint64_t *frame = (uint64_t *)sp;
    uint64_t elr = frame[31];
    uint32_t esr;
    asm volatile("mrs %0, esr_el1" : "=r"(esr));

    uint32_t ec = (esr >> 26) & 0x3F;

    if (ec == 0x15) {
        sp = task_schedule(sp);
    } else {
        uart_puts("[PANIC] Synchronous exception at ");
        uart_puthex(elr);
        uart_puts(" ESR=");
        uart_puthex(esr);
        uart_puts("\n");
        while (1) {}
    }

    return sp;
}

void handle_fiq(void) {
}

void handle_serror(void) {
    uart_puts("[PANIC] SError exception\n");
    while (1) {}
}
