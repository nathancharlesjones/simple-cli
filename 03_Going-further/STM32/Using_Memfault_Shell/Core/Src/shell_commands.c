#include "shell.h"
#include "led.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static const sShellCommand s_shell_commands[] = {
  {"on", on, "Turn on Blinky"},
  {"off", off, "Turn off Blinky"},
  {"dc", dc, "Get or set the duty cycle"},
  {"freq", freq, "Get or set the frequency"},
  {"help", shell_help_handler, "Lists all commands"},
};

const sShellCommand *const g_shell_commands = s_shell_commands;
const size_t g_num_shell_commands = ARRAY_SIZE(s_shell_commands);
