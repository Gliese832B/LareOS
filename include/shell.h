#ifndef SHELL_H
#define SHELL_H

#include "lareos.h"

#define SHELL_MAX_CMD_LEN   256
#define SHELL_MAX_ARGS      16
#define SHELL_HISTORY_SIZE  16

typedef void (*shell_cmd_fn)(int argc, char **argv);

typedef struct {
    const char *name;
    const char *description;
    shell_cmd_fn handler;
} shell_command_t;

void shell_init(void);
void shell_run(void);
void shell_register_command(const char *name, const char *desc, shell_cmd_fn handler);

#endif
