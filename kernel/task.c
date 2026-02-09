#include "task.h"
#include "mm.h"
#include "string.h"
#include "timer.h"
#include "uart.h"

static task_t tasks[MAX_TASKS];
static int current_task = 0;
static uint32_t next_id = 1;
static volatile bool scheduler_enabled = false;

void task_init(void) {
    memset(tasks, 0, sizeof(tasks));

    tasks[0].state = TASK_RUNNING;
    tasks[0].id = next_id++;
    strncpy(tasks[0].name, "kernel", TASK_NAME_LEN);
    tasks[0].stack_base = NULL;
    tasks[0].stack_size = 0;
    tasks[0].created_at = timer_get_ticks();
    tasks[0].priority = 5;

    current_task = 0;
    scheduler_enabled = true;
}

void task_entry_wrapper(void) {
    task_t *task = &tasks[current_task];
    if (task->entry) {
        task->entry(task->arg);
    }
    task_exit();
}

int task_create(const char *name, task_entry_t entry, void *arg, uint8_t priority) {
    int slot = -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_UNUSED) {
            slot = i;
            break;
        }
    }
    if (slot < 0) return -1;

    uint8_t *stack = (uint8_t *)page_alloc(TASK_STACK_SIZE / PAGE_SIZE);
    if (!stack) return -1;

    memset(stack, 0, TASK_STACK_SIZE);

    task_t *task = &tasks[slot];
    memset(task, 0, sizeof(task_t));

    task->state = TASK_READY;
    task->id = next_id++;
    strncpy(task->name, name ? name : "unnamed", TASK_NAME_LEN);
    task->stack_base = stack;
    task->stack_size = TASK_STACK_SIZE;
    task->created_at = timer_get_ticks();
    task->priority = priority;
    task->entry = entry;
    task->arg = arg;

    uint64_t stack_top = (uint64_t)stack + TASK_STACK_SIZE;
    stack_top &= ~0xFULL;

    task->context.sp = stack_top;
    task->context.elr = (uint64_t)task_entry_wrapper;
    task->context.spsr = 0x345;
    task->context.x[0] = (uint64_t)arg;
    task->context.x[29] = 0;
    task->context.x[30] = (uint64_t)task_entry_wrapper;

    return (int)task->id;
}

static int find_next_task(void) {
    int start = current_task;
    int next = start;

    for (int i = 0; i < MAX_TASKS; i++) {
        next = (start + 1 + i) % MAX_TASKS;
        if (tasks[next].state == TASK_SLEEPING) {
            if (timer_get_ticks() >= tasks[next].sleep_until) {
                tasks[next].state = TASK_READY;
            }
        }
        if (tasks[next].state == TASK_READY) {
            return next;
        }
    }

    if (tasks[start].state == TASK_RUNNING || tasks[start].state == TASK_READY) {
        return start;
    }

    return 0;
}

uint64_t task_schedule(uint64_t current_sp) {
    if (!scheduler_enabled) return current_sp;

    tasks[current_task].context.sp = current_sp;
    tasks[current_task].cpu_ticks++;

    if (tasks[current_task].state == TASK_RUNNING) {
        tasks[current_task].state = TASK_READY;
    }

    int next = find_next_task();

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_ZOMBIE && tasks[i].stack_base) {
            page_free(tasks[i].stack_base, tasks[i].stack_size / PAGE_SIZE);
            tasks[i].stack_base = NULL;
            tasks[i].state = TASK_UNUSED;
        }
    }

    current_task = next;
    tasks[current_task].state = TASK_RUNNING;

    return tasks[current_task].context.sp;
}

void task_yield(void) {
    asm volatile("svc #0");
}

void task_exit(void) {
    disable_irq();
    tasks[current_task].state = TASK_ZOMBIE;
    enable_irq();
    task_yield();
    while (1) { asm volatile("wfe"); }
}

void task_sleep_ms(uint32_t ms) {
    disable_irq();
    tasks[current_task].state = TASK_SLEEPING;
    tasks[current_task].sleep_until = timer_get_ticks() + (uint64_t)ms * 1000;
    enable_irq();
    task_yield();
}

int task_kill(uint32_t id) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id == id && tasks[i].state != TASK_UNUSED) {
            if (i == 0) return -1;
            tasks[i].state = TASK_ZOMBIE;
            return 0;
        }
    }
    return -1;
}

task_t *task_get_current(void) {
    return &tasks[current_task];
}

task_t *task_get_list(void) {
    return tasks;
}

int task_get_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state != TASK_UNUSED) count++;
    }
    return count;
}
