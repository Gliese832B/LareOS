#ifndef LAREOS_H
#define LAREOS_H

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;
typedef uint64_t            size_t;
typedef int64_t             ssize_t;
typedef int32_t             bool;

#define true  1
#define false 0
#define NULL  ((void*)0)

#define LAREOS_VERSION_MAJOR 1
#define LAREOS_VERSION_MINOR 0
#define LAREOS_VERSION_PATCH 0
#define LAREOS_CODENAME      "Falcon"

#ifdef RPI4
    #define MMIO_BASE       0xFE000000
#else
    #define MMIO_BASE       0x3F000000
#endif

#define GPIO_BASE           (MMIO_BASE + 0x00200000)
#define UART0_BASE          (MMIO_BASE + 0x00201000)
#define UART1_BASE          (MMIO_BASE + 0x00215000)
#define MBOX_BASE           (MMIO_BASE + 0x0000B880)
#define TIMER_BASE          (MMIO_BASE + 0x00003000)
#define IRQ_BASE            (MMIO_BASE + 0x0000B200)
#define PM_BASE             (MMIO_BASE + 0x00100000)
#define EMMC_BASE           (MMIO_BASE + 0x00300000)

#define UNUSED(x)           ((void)(x))
#define ALIGN(x, a)         (((x) + ((a) - 1)) & ~((a) - 1))
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof((x)[0]))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

static inline void mmio_write(uint64_t reg, uint32_t val) {
    *(volatile uint32_t*)reg = val;
}

static inline uint32_t mmio_read(uint64_t reg) {
    return *(volatile uint32_t*)reg;
}

static inline void delay(uint32_t count) {
    while (count--) {
        asm volatile("nop");
    }
}

extern void enable_irq(void);
extern void disable_irq(void);
extern uint64_t get_el(void);
extern void put_exception_vector(void* addr);
extern void delay_cycles(uint64_t cycles);

#endif
