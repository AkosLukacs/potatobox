#ifndef __CMDCLEAR_H__
#define __CMDCLEAR_H__

#define CMD_CLEAR_HEADER		"\nAvailable commands:\n"
#define CMD_CLEAR_LINE_1		"clear          -> Clears terminal.\n"
#define CMD_CLEAR_FOOTER		"eof"

//
// ANSI ESC Sequences for terminal control.
//
#define CLS     "\033[2J"
#define TBLUE   "\033[37;44m"
#define TNORM   "\033[0m"


void CmdClear (void);
void CmdClearHelp (void);

#endif
