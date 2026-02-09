#ifndef MAILBOX_H
#define MAILBOX_H

#include "lareos.h"

#define MBOX_READ       (MBOX_BASE + 0x00)
#define MBOX_POLL       (MBOX_BASE + 0x10)
#define MBOX_SENDER     (MBOX_BASE + 0x14)
#define MBOX_STATUS     (MBOX_BASE + 0x18)
#define MBOX_CONFIG     (MBOX_BASE + 0x1C)
#define MBOX_WRITE      (MBOX_BASE + 0x20)

#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000
#define MBOX_RESPONSE   0x80000000

#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LED     4
#define MBOX_CH_BTN     5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_PROP    8

#define MBOX_TAG_GETSERIAL      0x00010004
#define MBOX_TAG_GETMODEL       0x00010001
#define MBOX_TAG_GETREVISION    0x00010002
#define MBOX_TAG_GETMEMORY      0x00010005
#define MBOX_TAG_GETTEMP        0x00030006
#define MBOX_TAG_GETMAXTEMP     0x0003000A
#define MBOX_TAG_GETCLOCKRATE   0x00030002
#define MBOX_TAG_GETMAXCLOCK    0x00030004
#define MBOX_TAG_GETMINCLOCK    0x00030007
#define MBOX_TAG_SETCLOCKRATE   0x00038002
#define MBOX_TAG_GETVOLTAGE     0x00030003
#define MBOX_TAG_GETFBRES       0x00040003
#define MBOX_TAG_SETFBRES       0x00048003
#define MBOX_TAG_SETFBVRES      0x00048004
#define MBOX_TAG_SETFBDEPTH     0x00048005
#define MBOX_TAG_SETFBPXORDER   0x00048006
#define MBOX_TAG_ALLOCFB        0x00040001
#define MBOX_TAG_GETPITCH       0x00040008
#define MBOX_TAG_SETPOWER       0x00028001
#define MBOX_TAG_LAST           0

#define CLOCK_ID_EMMC   1
#define CLOCK_ID_UART   2
#define CLOCK_ID_ARM    3
#define CLOCK_ID_CORE   4

bool mailbox_call(uint8_t channel);
uint32_t mailbox_get_board_revision(void);
uint64_t mailbox_get_serial(void);
uint32_t mailbox_get_arm_memory(void);
uint32_t mailbox_get_temperature(void);
uint32_t mailbox_get_max_temperature(void);
uint32_t mailbox_get_clock_rate(uint32_t clock_id);
uint32_t mailbox_get_max_clock_rate(uint32_t clock_id);
bool mailbox_set_clock_rate(uint32_t clock_id, uint32_t rate);

extern volatile uint32_t __attribute__((aligned(16))) mbox[36];

#endif
