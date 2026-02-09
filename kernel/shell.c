#include "shell.h"
#include "uart.h"
#include "string.h"
#include "timer.h"
#include "mm.h"
#include "power.h"
#include "fb.h"
#include "mailbox.h"

#define MAX_COMMANDS 32

static shell_command_t commands[MAX_COMMANDS];
static int command_count = 0;
static char history[SHELL_HISTORY_SIZE][SHELL_MAX_CMD_LEN];
static int history_count = 0;
static int history_pos = 0;

static void cmd_help(int argc, char **argv);
static void cmd_clear(int argc, char **argv);
static void cmd_info(int argc, char **argv);
static void cmd_status(int argc, char **argv);
static void cmd_mem(int argc, char **argv);
static void cmd_temp(int argc, char **argv);
static void cmd_freq(int argc, char **argv);
static void cmd_uptime(int argc, char **argv);
static void cmd_profile(int argc, char **argv);
static void cmd_reboot(int argc, char **argv);
static void cmd_shutdown(int argc, char **argv);
static void cmd_benchmark(int argc, char **argv);
static void cmd_echo(int argc, char **argv);
static void cmd_history(int argc, char **argv);
static void cmd_color(int argc, char **argv);
static void cmd_peekpoke(int argc, char **argv);

static void print_banner(void) {
    uart_puts("\n\033[36m");
    uart_puts("  _                    ___  ____  \n");
    uart_puts(" | |   __ _ _ __ ___ / _ \\/ ___| \n");
    uart_puts(" | |  / _` | '__/ _ \\ | | \\___ \\ \n");
    uart_puts(" | |_| (_| | | |  __/ |_| |___) |\n");
    uart_puts(" |____\\__,_|_|  \\___|\\___/|____/ \n");
    uart_puts("\033[0m\n");
    uart_puts(" \033[1mLareOS v1.0.0\033[0m (Falcon) | Raspberry Pi\n");
    uart_puts(" Type \033[32mhelp\033[0m for available commands.\n\n");
}

static int parse_args(char *cmd, char **argv) {
    int argc = 0;
    bool in_arg = false;

    while (*cmd && argc < SHELL_MAX_ARGS) {
        if (*cmd == ' ' || *cmd == '\t') {
            *cmd = '\0';
            in_arg = false;
        } else if (!in_arg) {
            argv[argc++] = cmd;
            in_arg = true;
        }
        cmd++;
    }
    return argc;
}

void shell_register_command(const char *name, const char *desc, shell_cmd_fn handler) {
    if (command_count >= MAX_COMMANDS) return;
    commands[command_count].name = name;
    commands[command_count].description = desc;
    commands[command_count].handler = handler;
    command_count++;
}

void shell_init(void) {
    command_count = 0;
    history_count = 0;
    history_pos = 0;

    shell_register_command("help",      "Show available commands",     cmd_help);
    shell_register_command("clear",     "Clear screen",                cmd_clear);
    shell_register_command("info",      "Show system information",     cmd_info);
    shell_register_command("status",    "Show system status",          cmd_status);
    shell_register_command("mem",       "Show memory usage",           cmd_mem);
    shell_register_command("temp",      "Show CPU temperature",        cmd_temp);
    shell_register_command("freq",      "Show CPU frequency",          cmd_freq);
    shell_register_command("uptime",    "Show system uptime",          cmd_uptime);
    shell_register_command("profile",   "Set performance profile",     cmd_profile);
    shell_register_command("benchmark", "Run performance benchmark",   cmd_benchmark);
    shell_register_command("echo",      "Print text",                  cmd_echo);
    shell_register_command("history",   "Show command history",        cmd_history);
    shell_register_command("color",     "Test color output",           cmd_color);
    shell_register_command("peek",      "Read memory address",         cmd_peekpoke);
    shell_register_command("reboot",    "Reboot system",               cmd_reboot);
    shell_register_command("shutdown",  "Shutdown system",             cmd_shutdown);
}

void shell_run(void) {
    char cmd[SHELL_MAX_CMD_LEN];
    int pos;

    print_banner();

    while (1) {
        uart_puts("\033[36mlareos\033[0m:\033[33m~\033[0m$ ");
        pos = 0;

        while (1) {
            char c = uart_getc();

            if (c == '\r' || c == '\n') {
                uart_puts("\r\n");
                break;
            }

            if (c == 127 || c == 8) {
                if (pos > 0) {
                    pos--;
                    uart_puts("\b \b");
                }
                continue;
            }

            if (c == 3) {
                uart_puts("^C\r\n");
                pos = 0;
                break;
            }

            if (c >= 32 && pos < SHELL_MAX_CMD_LEN - 1) {
                cmd[pos++] = c;
                uart_putc(c);
            }
        }

        cmd[pos] = '\0';
        if (pos == 0) continue;

        if (history_count < SHELL_HISTORY_SIZE) {
            strcpy(history[history_count++], cmd);
        } else {
            for (int i = 1; i < SHELL_HISTORY_SIZE; i++) {
                strcpy(history[i - 1], history[i]);
            }
            strcpy(history[SHELL_HISTORY_SIZE - 1], cmd);
        }

        char *argv[SHELL_MAX_ARGS];
        int argc = parse_args(cmd, argv);
        if (argc == 0) continue;

        bool found = false;
        for (int i = 0; i < command_count; i++) {
            if (strcmp(argv[0], commands[i].name) == 0) {
                commands[i].handler(argc, argv);
                found = true;
                break;
            }
        }

        if (!found) {
            uart_puts("\033[31mUnknown command: \033[0m");
            uart_puts(argv[0]);
            uart_puts("\n");
        }
    }
}

static void cmd_help(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("\033[1mLareOS Commands:\033[0m\n\n");
    for (int i = 0; i < command_count; i++) {
        uart_puts("  \033[32m");
        uart_puts(commands[i].name);
        uart_puts("\033[0m");
        int pad = 14 - strlen(commands[i].name);
        while (pad-- > 0) uart_putc(' ');
        uart_puts(commands[i].description);
        uart_putc('\n');
    }
    uart_putc('\n');
}

static void cmd_clear(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("\033[2J\033[H");
}

static void cmd_info(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    system_power_t pwr = power_get_status();

    uart_puts("\033[1m\033[36mLareOS System Information\033[0m\n");
    uart_puts("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    uart_puts("  OS:         LareOS v1.0.0 (Falcon)\n");
    uart_puts("  Arch:       AArch64\n");

    uart_puts("  Board Rev:  0x");
    uart_puthex(pwr.board_revision);
    uart_putc('\n');

    uart_puts("  Serial:     0x");
    uart_puthex(pwr.board_serial);
    uart_putc('\n');

    uart_puts("  ARM Memory: ");
    uart_putuint(pwr.arm_memory / (1024 * 1024));
    uart_puts(" MB\n");

    uart_puts("  ARM Clock:  ");
    uart_putuint(pwr.arm_clock / 1000000);
    uart_puts(" MHz\n");

    uart_puts("  Core Clock: ");
    uart_putuint(pwr.core_clock / 1000000);
    uart_puts(" MHz\n");

    uart_puts("  CPU Temp:   ");
    uart_putuint(pwr.cpu_temp / 1000);
    uart_puts(".");
    uart_putuint((pwr.cpu_temp % 1000) / 100);
    uart_puts(" C\n");

    uart_puts("  Profile:    ");
    uart_puts(power_get_profile_name(pwr.current_profile));
    uart_putc('\n');

    uart_puts("  EL:         ");
    uart_putuint(get_el());
    uart_putc('\n');

    uart_puts("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

static void cmd_status(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    system_power_t pwr = power_get_status();
    mem_info_t mem = mm_get_info();

    uart_puts("\033[1mSystem Status\033[0m\n");

    uart_puts("  CPU:  ");
    uart_putuint(pwr.arm_clock / 1000000);
    uart_puts(" MHz (");
    uart_puts(power_get_profile_name(pwr.current_profile));
    uart_puts(")\n");

    uart_puts("  Temp: ");
    uint32_t t = pwr.cpu_temp / 1000;
    if (t > 70) uart_puts("\033[31m");
    else if (t > 55) uart_puts("\033[33m");
    else uart_puts("\033[32m");
    uart_putuint(t);
    uart_puts(" C\033[0m\n");

    uart_puts("  Mem:  ");
    uart_putuint(mem.used / 1024);
    uart_puts(" KB / ");
    uart_putuint(mem.total / 1024);
    uart_puts(" KB (");
    uart_putuint(mem.pages_used * 100 / mem.pages_total);
    uart_puts("%%)\n");

    uart_puts("  Up:   ");
    uint64_t s = timer_get_uptime_seconds();
    uart_putuint(s / 3600);
    uart_puts("h ");
    uart_putuint((s % 3600) / 60);
    uart_puts("m ");
    uart_putuint(s % 60);
    uart_puts("s\n");
}

static void cmd_mem(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    mem_info_t info = mm_get_info();

    uart_puts("\033[1mMemory Usage\033[0m\n");
    uart_puts("  Total:      ");
    uart_putuint(info.total / 1024);
    uart_puts(" KB\n");
    uart_puts("  Used:       ");
    uart_putuint(info.used / 1024);
    uart_puts(" KB\n");
    uart_puts("  Free:       ");
    uart_putuint(info.free / 1024);
    uart_puts(" KB\n");
    uart_puts("  Pages:      ");
    uart_putuint(info.pages_used);
    uart_puts(" / ");
    uart_putuint(info.pages_total);
    uart_putc('\n');

    uart_puts("  [");
    uint32_t pct = info.pages_used * 100 / info.pages_total;
    uint32_t bars = pct * 30 / 100;
    for (uint32_t i = 0; i < 30; i++) {
        uart_putc(i < bars ? '#' : '-');
    }
    uart_puts("] ");
    uart_putuint(pct);
    uart_puts("%%\n");
}

static void cmd_temp(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uint32_t temp = mailbox_get_temperature();
    uint32_t max_temp = mailbox_get_max_temperature();
    uart_puts("CPU Temperature: ");
    uart_putuint(temp / 1000);
    uart_puts(".");
    uart_putuint((temp % 1000) / 100);
    uart_puts(" C (max: ");
    uart_putuint(max_temp / 1000);
    uart_puts(" C)\n");
}

static void cmd_freq(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("ARM:  ");
    uart_putuint(mailbox_get_clock_rate(CLOCK_ID_ARM) / 1000000);
    uart_puts(" MHz (max: ");
    uart_putuint(mailbox_get_max_clock_rate(CLOCK_ID_ARM) / 1000000);
    uart_puts(")\n");
    uart_puts("Core: ");
    uart_putuint(mailbox_get_clock_rate(CLOCK_ID_CORE) / 1000000);
    uart_puts(" MHz\n");
}

static void cmd_uptime(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uint64_t s = timer_get_uptime_seconds();
    uart_puts("Uptime: ");
    uart_putuint(s / 3600);
    uart_puts("h ");
    uart_putuint((s % 3600) / 60);
    uart_puts("m ");
    uart_putuint(s % 60);
    uart_puts("s\n");
}

static void cmd_profile(int argc, char **argv) {
    if (argc < 2) {
        uart_puts("Usage: profile <max|balanced|powersave>\n");
        uart_puts("Current: ");
        system_power_t pwr = power_get_status();
        uart_puts(power_get_profile_name(pwr.current_profile));
        uart_putc('\n');
        return;
    }

    if (strcmp(argv[1], "max") == 0) {
        power_set_profile(POWER_PROFILE_MAX);
        uart_puts("\033[32mProfile set to MAX\033[0m\n");
    } else if (strcmp(argv[1], "balanced") == 0) {
        power_set_profile(POWER_PROFILE_BALANCED);
        uart_puts("\033[32mProfile set to BALANCED\033[0m\n");
    } else if (strcmp(argv[1], "powersave") == 0) {
        power_set_profile(POWER_PROFILE_POWERSAVE);
        uart_puts("\033[32mProfile set to POWERSAVE\033[0m\n");
    } else {
        uart_puts("\033[31mUnknown profile: ");
        uart_puts(argv[1]);
        uart_puts("\033[0m\n");
    }
}

static void cmd_benchmark(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("\033[1mLareOS Benchmark\033[0m\n");
    uart_puts("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    uart_puts("[1/3] CPU Integer...\n");
    uint64_t start = timer_get_ticks();
    volatile uint64_t sum = 0;
    for (volatile uint64_t i = 0; i < 10000000; i++) {
        sum += i * i;
    }
    uint64_t cpu_time = timer_get_ticks() - start;
    uart_puts("  Time: ");
    uart_putuint(cpu_time / 1000);
    uart_puts(" ms | Score: ");
    uart_putuint(10000000000ULL / (cpu_time + 1));
    uart_putc('\n');

    uart_puts("[2/3] Memory...\n");
    void *block = kmalloc(65536);
    start = timer_get_ticks();
    if (block) {
        for (int i = 0; i < 100; i++) {
            memset(block, i, 65536);
        }
    }
    uint64_t mem_time = timer_get_ticks() - start;
    if (block) kfree(block);
    uart_puts("  Time: ");
    uart_putuint(mem_time / 1000);
    uart_puts(" ms | Score: ");
    uart_putuint(6400000000ULL / (mem_time + 1));
    uart_putc('\n');

    uart_puts("[3/3] Alloc/Free...\n");
    start = timer_get_ticks();
    for (int i = 0; i < 10000; i++) {
        void *p = kmalloc(64);
        if (p) kfree(p);
    }
    uint64_t alloc_time = timer_get_ticks() - start;
    uart_puts("  Time: ");
    uart_putuint(alloc_time / 1000);
    uart_puts(" ms | Score: ");
    uart_putuint(10000000000ULL / (alloc_time + 1));
    uart_putc('\n');

    uart_puts("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    uint64_t total = 10000000000ULL / (cpu_time + 1) + 6400000000ULL / (mem_time + 1) + 10000000000ULL / (alloc_time + 1);
    uart_puts("\033[1mTotal Score: \033[36m");
    uart_putuint(total / 3);
    uart_puts("\033[0m\n");
}

static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) uart_putc(' ');
        uart_puts(argv[i]);
    }
    uart_putc('\n');
}

static void cmd_history(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    for (int i = 0; i < history_count; i++) {
        uart_puts("  ");
        uart_putuint(i + 1);
        uart_puts("  ");
        uart_puts(history[i]);
        uart_putc('\n');
    }
}

static void cmd_color(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("\033[30m BLACK  \033[0m ");
    uart_puts("\033[31m RED    \033[0m ");
    uart_puts("\033[32m GREEN  \033[0m ");
    uart_puts("\033[33m YELLOW \033[0m\n");
    uart_puts("\033[34m BLUE   \033[0m ");
    uart_puts("\033[35m PURPLE \033[0m ");
    uart_puts("\033[36m CYAN   \033[0m ");
    uart_puts("\033[37m WHITE  \033[0m\n");
}

static void cmd_peekpoke(int argc, char **argv) {
    if (argc < 2) {
        uart_puts("Usage: peek <address>\n");
        return;
    }

    uint64_t addr = 0;
    const char *s = argv[1];
    if (s[0] == '0' && s[1] == 'x') s += 2;

    while (*s) {
        addr <<= 4;
        if (*s >= '0' && *s <= '9') addr |= *s - '0';
        else if (*s >= 'a' && *s <= 'f') addr |= *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'F') addr |= *s - 'A' + 10;
        s++;
    }

    uint32_t val = *(volatile uint32_t*)addr;
    uart_puts("[");
    uart_puthex(addr);
    uart_puts("] = ");
    uart_puthex(val);
    uart_putc('\n');
}

static void cmd_reboot(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("Rebooting...\n");
    timer_sleep(500);
    power_reboot();
}

static void cmd_shutdown(int argc, char **argv) {
    UNUSED(argc); UNUSED(argv);
    uart_puts("Shutting down...\n");
    timer_sleep(500);
    power_shutdown();
}
