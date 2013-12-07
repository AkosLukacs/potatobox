#ifndef __CMDPRINT_H__
#define __CMDPRINT_H__

#define CMD_PRINT_HEADER		"\nAvailable commands:\n"
#define CMD_PRINT_LINE_1		"print -h      -> Displays this help.\n"
#define CMD_PRINT_LINE_2		"print -c      -> Displays current command info.\n"
#define CMD_PRINT_LINE_3		"print -c -0   -> Displays previous command info ( max 9) .\n"
#define CMD_PRINT_LINE_4		"print -u      -> Displays uart command info.\n"
#define CMD_PRINT_LINE_5		"print -t      -> Displays timing info.\n"
#define CMD_PRINT_LINE_6		"print -s      -> Displays stack info.\n"
#define CMD_PRINT_LINE_7		"print -a      -> Displays all commands on stack.\n"
#define CMD_PRINT_LINE_8		"print -k      -> Displays table of registered commands.\n"
#define CMD_PRINT_FOOTER		"eof"

#define CMD_NO_ARGS_ERROR		"\nNo arguments, use command -h for help\n"
#define CMD_INVALID_STACK		"\nInvalid stack location, requested: %d\n"
#define CMD_INVALID_ARG			"\nInvalid argument requested.\n"

#define CMD_ARG_TRIGGER 2
#define CMD_STACK_MIN 0
#define CMD_STACK_MAX 9

void PrintCurrentCommand(void);
void PrintCurrentUart(void);
void PrintCurrentTime(void);
void PrintCurrentStack(void);
void PrintWholeStack(void);
void PrintRegisteredCommands(void);

void CmdPrint(void);
void CmdDisplayHelp(void);

#endif
