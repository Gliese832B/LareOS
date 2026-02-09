#ifndef TASK_H
#define TASK_H

#include "lareos.h"

#define MAX_TASKS           16
#define TASK_STACK_SIZE     16384
#define TASK_NAME_LEN       32

#define TASK_UNUSED         0
#define TASK_READY          1
#define TASK_RUNNING        2
#define TASK_SLEEPING       3
#define TASK_BLOCKED        4
#define TASK_ZOMBIE         5

typedef struct {
    uint64_t x[31];
    uint64_t sp;
    uint64_t elr;
    uint64_t spsr;
} cpu_context_t;

typedef void (*task_entry_t)(void *arg);

typedef struct {
    cpu_context_t context;
    uint8_t state;
    uint32_t id;
    char name[TASK_NAME_LEN];
    uint8_t *stack_base;
    uint32_t stack_size;
    uint64_t sleep_until;
    uint64_t created_at;
    uint64_t cpu_ticks;
    uint8_t priority;
    task_entry_t entry;
    void *arg;
} task_t;

void task_init(void);
int task_create(const char *name, task_entry_t entry, void *arg, uint8_t priority);
void task_yield(void);
void task_exit(void);
void task_sleep_ms(uint32_t ms);
int task_kill(uint32_t id);
task_t *task_get_current(void);
task_t *task_get_list(void);
int task_get_count(void);
uint64_t task_schedule(uint64_t current_sp);
void task_entry_wrapper(void);

extern void context_switch(cpu_context_t *old_ctx, cpu_context_t *new_ctx);

#endif
