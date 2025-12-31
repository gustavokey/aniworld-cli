#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED

#include <sys/wait.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define STRING_COLOR(s, c) ANSI_COLOR_##c s ANSI_COLOR_RESET

#define cmd(...) cmd_run((char *[]){ __VA_ARGS__, NULL })

extern inline int cmd_run(char *cmd[]);

#endif // CMD_H_INCLUDED
