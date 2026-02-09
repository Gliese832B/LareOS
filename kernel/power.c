#include "power.h"
#include "mailbox.h"
#include "uart.h"
#include "timer.h"

static system_power_t sys_power;

void power_init(void) {
    sys_power.board_revision = mailbox_get_board_revision();
    sys_power.board_serial = mailbox_get_serial();
    sys_power.arm_memory = mailbox_get_arm_memory();
    sys_power.cpu_max_temp = mailbox_get_max_temperature();
    sys_power.arm_max_clock = mailbox_get_max_clock_rate(CLOCK_ID_ARM);
    sys_power.arm_min_clock = mailbox_get_max_clock_rate(CLOCK_ID_ARM);
    sys_power.current_profile = POWER_PROFILE_BALANCED;

    power_set_profile(POWER_PROFILE_BALANCED);
}

void power_set_profile(uint8_t profile) {
    switch (profile) {
        case POWER_PROFILE_MAX:
            mailbox_set_clock_rate(CLOCK_ID_ARM, sys_power.arm_max_clock);
            mailbox_set_clock_rate(CLOCK_ID_CORE, 500000000);
            break;

        case POWER_PROFILE_BALANCED:
            mailbox_set_clock_rate(CLOCK_ID_ARM, sys_power.arm_max_clock * 3 / 4);
            mailbox_set_clock_rate(CLOCK_ID_CORE, 400000000);
            break;

        case POWER_PROFILE_POWERSAVE:
            mailbox_set_clock_rate(CLOCK_ID_ARM, sys_power.arm_min_clock);
            mailbox_set_clock_rate(CLOCK_ID_CORE, 250000000);
            break;
    }
    sys_power.current_profile = profile;
}

system_power_t power_get_status(void) {
    sys_power.cpu_temp = mailbox_get_temperature();
    sys_power.arm_clock = mailbox_get_clock_rate(CLOCK_ID_ARM);
    sys_power.core_clock = mailbox_get_clock_rate(CLOCK_ID_CORE);
    return sys_power;
}

void power_reboot(void) {
    uint32_t val;
    val = mmio_read(PM_RSTS);
    val &= ~0xFFFFFAAA;
    mmio_write(PM_RSTS, PM_PASSWORD | val);
    mmio_write(PM_WDOG, PM_PASSWORD | 10);
    mmio_write(PM_RSTC, PM_PASSWORD | 0x20);
    while (1) {}
}

void power_shutdown(void) {
    uint32_t val;
    val = mmio_read(PM_RSTS);
    val |= 0x555;
    mmio_write(PM_RSTS, PM_PASSWORD | val);
    mmio_write(PM_WDOG, PM_PASSWORD | 10);
    mmio_write(PM_RSTC, PM_PASSWORD | 0x20);
    while (1) {}
}

uint32_t power_get_temp(void) {
    return mailbox_get_temperature() / 1000;
}

uint32_t power_get_arm_clock(void) {
    return mailbox_get_clock_rate(CLOCK_ID_ARM) / 1000000;
}

void power_set_arm_clock(uint32_t rate_hz) {
    mailbox_set_clock_rate(CLOCK_ID_ARM, rate_hz);
}

const char *power_get_profile_name(uint8_t profile) {
    switch (profile) {
        case POWER_PROFILE_MAX:       return "max";
        case POWER_PROFILE_BALANCED:  return "balanced";
        case POWER_PROFILE_POWERSAVE: return "powersave";
        default:                      return "unknown";
    }
}
