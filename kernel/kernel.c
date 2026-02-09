#include "lareos.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "irq.h"
#include "mm.h"
#include "mailbox.h"
#include "fb.h"
#include "power.h"
#include "shell.h"
#include "string.h"

static void boot_log(const char *msg) {
    uart_puts("\033[32m[  OK]\033[0m  ");
    uart_puts(msg);
    uart_puts("\n");
}

static void boot_sequence(void) {
    uart_puts("\n\033[36m");
    uart_puts("  ██╗      █████╗ ██████╗ ███████╗ ██████╗ ███████╗\n");
    uart_puts("  ██║     ██╔══██╗██╔══██╗██╔════╝██╔═══██╗██╔════╝\n");
    uart_puts("  ██║     ███████║██████╔╝█████╗  ██║   ██║███████╗\n");
    uart_puts("  ██║     ██╔══██║██╔══██╗██╔══╝  ██║   ██║╚════██║\n");
    uart_puts("  ███████╗██║  ██║██║  ██║███████╗╚██████╔╝███████║\n");
    uart_puts("  ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝\n");
    uart_puts("\033[0m");
    uart_puts("              v1.0.0 (Falcon) | AArch64\n\n");

    boot_log("UART initialized");

    mm_init();
    boot_log("Memory manager initialized");

    irq_init();
    boot_log("Interrupt controller initialized");

    timer_init();
    boot_log("System timer initialized");

    power_init();
    system_power_t pwr = power_get_status();
    boot_log("Power management initialized");

    uart_puts("\033[32m[  OK]\033[0m  ARM Clock: ");
    uart_putuint(pwr.arm_clock / 1000000);
    uart_puts(" MHz\n");

    uart_puts("\033[32m[  OK]\033[0m  ARM Memory: ");
    uart_putuint(pwr.arm_memory / (1024 * 1024));
    uart_puts(" MB\n");

    uart_puts("\033[32m[  OK]\033[0m  CPU Temp: ");
    uart_putuint(pwr.cpu_temp / 1000);
    uart_puts(" C\n");

    mem_info_t mem = mm_get_info();
    uart_puts("\033[32m[  OK]\033[0m  Heap: ");
    uart_putuint(mem.total / 1024);
    uart_puts(" KB (");
    uart_putuint(mem.pages_total);
    uart_puts(" pages)\n");

    uart_puts("\033[32m[  OK]\033[0m  Exception Level: EL");
    uart_putuint(get_el());
    uart_puts("\n");

    if (fb_init(FB_DEFAULT_WIDTH, FB_DEFAULT_HEIGHT, FB_DEFAULT_DEPTH)) {
        framebuffer_t *fbi = fb_get_info();
        fb_clear(COLOR_DARK);
        fb_puts(20, 20, "LareOS v1.0.0", COLOR_CYAN, COLOR_DARK);
        fb_puts(20, 40, "Falcon | AArch64", COLOR_WHITE, COLOR_DARK);

        fb_puts(20, 70, "CPU:", COLOR_GRAY, COLOR_DARK);
        fb_puts(20, 90, "MEM:", COLOR_GRAY, COLOR_DARK);

        fb_draw_progress_bar(60, 70, 200, 12, 50, COLOR_GREEN, COLOR_SURFACE);
        fb_draw_progress_bar(60, 90, 200, 12, mem.pages_used * 100 / mem.pages_total, COLOR_ACCENT, COLOR_SURFACE);

        uart_puts("\033[32m[  OK]\033[0m  Framebuffer: "); 
        uart_putuint(fbi->width);
        uart_puts("x");
        uart_putuint(fbi->height);
        uart_puts("x");
        uart_putuint(fbi->depth);
        uart_puts("\n");
    } else {
        uart_puts("\033[33m[WARN]\033[0m  Framebuffer not available\n");
    }

    boot_log("Boot complete");

    uart_puts("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

void kernel_main(void) {
    uart_init();
    boot_sequence();

    shell_init();
    shell_run();
}
