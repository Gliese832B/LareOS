#include "mailbox.h"

volatile uint32_t __attribute__((aligned(16))) mbox[36];

bool mailbox_call(uint8_t channel) {
    uint32_t r = ((uint32_t)((uint64_t)&mbox) & ~0xF) | (channel & 0xF);

    while (mmio_read(MBOX_STATUS) & MBOX_FULL) {}
    mmio_write(MBOX_WRITE, r);

    while (1) {
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY) {}
        if (mmio_read(MBOX_READ) == r) {
            return mbox[1] == MBOX_RESPONSE;
        }
    }
}

uint32_t mailbox_get_board_revision(void) {
    mbox[0] = 7 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETREVISION;
    mbox[3] = 4;
    mbox[4] = 0;
    mbox[5] = 0;
    mbox[6] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return mbox[5];
    }
    return 0;
}

uint64_t mailbox_get_serial(void) {
    mbox[0] = 8 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETSERIAL;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = 0;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return ((uint64_t)mbox[6] << 32) | mbox[5];
    }
    return 0;
}

uint32_t mailbox_get_arm_memory(void) {
    mbox[0] = 8 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETMEMORY;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = 0;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return mbox[6];
    }
    return 0;
}

uint32_t mailbox_get_temperature(void) {
    mbox[0] = 8 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETTEMP;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = 0;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return mbox[6];
    }
    return 0;
}

uint32_t mailbox_get_max_temperature(void) {
    mbox[0] = 8 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETMAXTEMP;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = 0;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return mbox[6];
    }
    return 0;
}

uint32_t mailbox_get_clock_rate(uint32_t clock_id) {
    mbox[0] = 8 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETCLOCKRATE;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = clock_id;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return mbox[6];
    }
    return 0;
}

uint32_t mailbox_get_max_clock_rate(uint32_t clock_id) {
    mbox[0] = 8 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_GETMAXCLOCK;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = clock_id;
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;

    if (mailbox_call(MBOX_CH_PROP)) {
        return mbox[6];
    }
    return 0;
}

bool mailbox_set_clock_rate(uint32_t clock_id, uint32_t rate) {
    mbox[0] = 9 * 4;
    mbox[1] = 0;
    mbox[2] = MBOX_TAG_SETCLOCKRATE;
    mbox[3] = 12;
    mbox[4] = 0;
    mbox[5] = clock_id;
    mbox[6] = rate;
    mbox[7] = 0;
    mbox[8] = MBOX_TAG_LAST;

    return mailbox_call(MBOX_CH_PROP);
}
