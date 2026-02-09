#ifndef POWER_H
#define POWER_H

#include "lareos.h"

#define PM_RSTC         (PM_BASE + 0x1C)
#define PM_RSTS         (PM_BASE + 0x20)
#define PM_WDOG         (PM_BASE + 0x24)
#define PM_PASSWORD     0x5A000000

#define POWER_PROFILE_MAX       0
#define POWER_PROFILE_BALANCED  1
#define POWER_PROFILE_POWERSAVE 2

typedef struct {
    uint32_t cpu_temp;
    uint32_t cpu_max_temp;
    uint32_t arm_clock;
    uint32_t arm_max_clock;
    uint32_t arm_min_clock;
    uint32_t core_clock;
    uint32_t core_voltage;
    uint32_t arm_memory;
    uint32_t board_revision;
    uint64_t board_serial;
    uint8_t  current_profile;
} system_power_t;

void power_init(void);
void power_set_profile(uint8_t profile);
system_power_t power_get_status(void);
void power_reboot(void);
void power_shutdown(void);
uint32_t power_get_temp(void);
uint32_t power_get_arm_clock(void);
void power_set_arm_clock(uint32_t rate_hz);
const char *power_get_profile_name(uint8_t profile);

#endif
