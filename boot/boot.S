.section ".text.boot"

.global _start

_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, primary_cpu
    b       hang

primary_cpu:
    adr     x0, _start
    mov     sp, x0

    ldr     x1, =__bss_start
    ldr     x2, =__bss_end
bss_clear:
    cmp     x1, x2
    b.ge    bss_done
    str     xzr, [x1], #8
    b       bss_clear
bss_done:

    bl      kernel_main
    b       hang

hang:
    wfe
    b       hang

.global put_exception_vector
put_exception_vector:
    msr     vbar_el1, x0
    ret

.global get_el
get_el:
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    ret

.global enable_irq
enable_irq:
    msr     daifclr, #2
    ret

.global disable_irq
disable_irq:
    msr     daifset, #2
    ret

.global delay_cycles
delay_cycles:
    subs    x0, x0, #1
    bne     delay_cycles
    ret

.macro save_all_regs
    sub     sp, sp, #272
    stp     x0, x1, [sp, #16 * 0]
    stp     x2, x3, [sp, #16 * 1]
    stp     x4, x5, [sp, #16 * 2]
    stp     x6, x7, [sp, #16 * 3]
    stp     x8, x9, [sp, #16 * 4]
    stp     x10, x11, [sp, #16 * 5]
    stp     x12, x13, [sp, #16 * 6]
    stp     x14, x15, [sp, #16 * 7]
    stp     x16, x17, [sp, #16 * 8]
    stp     x18, x19, [sp, #16 * 9]
    stp     x20, x21, [sp, #16 * 10]
    stp     x22, x23, [sp, #16 * 11]
    stp     x24, x25, [sp, #16 * 12]
    stp     x26, x27, [sp, #16 * 13]
    stp     x28, x29, [sp, #16 * 14]
    mrs     x21, elr_el1
    mrs     x22, spsr_el1
    stp     x30, x21, [sp, #16 * 15]
    str     x22, [sp, #16 * 16]
.endm

.macro restore_all_regs
    ldr     x22, [sp, #16 * 16]
    ldp     x30, x21, [sp, #16 * 15]
    msr     spsr_el1, x22
    msr     elr_el1, x21
    ldp     x28, x29, [sp, #16 * 14]
    ldp     x26, x27, [sp, #16 * 13]
    ldp     x24, x25, [sp, #16 * 12]
    ldp     x22, x23, [sp, #16 * 11]
    ldp     x20, x21, [sp, #16 * 10]
    ldp     x18, x19, [sp, #16 * 9]
    ldp     x16, x17, [sp, #16 * 8]
    ldp     x14, x15, [sp, #16 * 7]
    ldp     x12, x13, [sp, #16 * 6]
    ldp     x10, x11, [sp, #16 * 5]
    ldp     x8, x9, [sp, #16 * 4]
    ldp     x6, x7, [sp, #16 * 3]
    ldp     x4, x5, [sp, #16 * 2]
    ldp     x2, x3, [sp, #16 * 1]
    ldp     x0, x1, [sp, #16 * 0]
    add     sp, sp, #272
.endm

.balign 0x800
.global exception_vector_table
exception_vector_table:
    b       sync_handler
    .balign 0x80
    b       irq_handler
    .balign 0x80
    b       fiq_handler
    .balign 0x80
    b       serror_handler
    .balign 0x80

    b       sync_handler
    .balign 0x80
    b       irq_handler
    .balign 0x80
    b       fiq_handler
    .balign 0x80
    b       serror_handler
    .balign 0x80

    b       sync_handler
    .balign 0x80
    b       irq_handler
    .balign 0x80
    b       fiq_handler
    .balign 0x80
    b       serror_handler
    .balign 0x80

    b       sync_handler
    .balign 0x80
    b       irq_handler
    .balign 0x80
    b       fiq_handler
    .balign 0x80
    b       serror_handler
    .balign 0x80

sync_handler:
    save_all_regs
    mov     x0, sp
    bl      handle_sync_svc
    mov     sp, x0
    restore_all_regs
    eret

irq_handler:
    save_all_regs
    mov     x0, sp
    bl      handle_irq_schedule
    mov     sp, x0
    restore_all_regs
    eret

fiq_handler:
    bl      handle_fiq
    eret

serror_handler:
    bl      handle_serror
    eret

.global context_switch
context_switch:
    stp     x19, x20, [x0, #16 * 0]
    stp     x21, x22, [x0, #16 * 1]
    stp     x23, x24, [x0, #16 * 2]
    stp     x25, x26, [x0, #16 * 3]
    stp     x27, x28, [x0, #16 * 4]
    stp     x29, x30, [x0, #16 * 5]
    mov     x9, sp
    str     x9, [x0, #16 * 6]

    ldp     x19, x20, [x1, #16 * 0]
    ldp     x21, x22, [x1, #16 * 1]
    ldp     x23, x24, [x1, #16 * 2]
    ldp     x25, x26, [x1, #16 * 3]
    ldp     x27, x28, [x1, #16 * 4]
    ldp     x29, x30, [x1, #16 * 5]
    ldr     x9, [x1, #16 * 6]
    mov     sp, x9

    ret
