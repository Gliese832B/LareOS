#ifndef IRQ_H
#define IRQ_H

#include "lareos.h"

#define IRQ_BASIC_PENDING   (IRQ_BASE + 0x00)
#define IRQ_PENDING_1       (IRQ_BASE + 0x04)
#define IRQ_PENDING_2       (IRQ_BASE + 0x08)
#define IRQ_FIQ_CTRL        (IRQ_BASE + 0x0C)
#define IRQ_ENABLE_1        (IRQ_BASE + 0x10)
#define IRQ_ENABLE_2        (IRQ_BASE + 0x14)
#define IRQ_ENABLE_BASIC    (IRQ_BASE + 0x18)
#define IRQ_DISABLE_1       (IRQ_BASE + 0x1C)
#define IRQ_DISABLE_2       (IRQ_BASE + 0x20)
#define IRQ_DISABLE_BASIC   (IRQ_BASE + 0x24)

#define IRQ_TIMER1          (1 << 1)
#define IRQ_TIMER3          (1 << 3)
#define IRQ_UART            (1 << 25)

void irq_init(void);
void irq_enable(uint32_t irq);
void irq_disable(uint32_t irq);
void handle_irq(void);
void handle_sync(void);
void handle_fiq(void);
void handle_serror(void);

#endif
