#ifndef __CMDHELP_H__
#define __CMDHELP_H__



#define CMD_HELP_HEADER		"\nAvailable commands:\n"
#define CMD_HELP_LINE_1		"exit  -> terminates program.\n"
#define CMD_HELP_LINE_2		"adc   -> analog to digital conversion.\n"
#define CMD_HELP_LINE_3		"uart  -> communication interface.\n"
#define CMD_HELP_LINE_4		"gpio  -> general purpose io access.  \n"
#define CMD_HELP_LINE_5		"help  -> displays this help.         \n"
#define CMD_HELP_LINE_6		"time  -> timekeeping functionality.  \n"
#define CMD_HELP_LINE_7		"print -> debug output functionality. \n"
#define CMD_HELP_LINE_8		"stat  -> displays device information \n"
#define CMD_HELP_LINE_9		"queue -> command execution queue.    \n"
#define CMD_HELP_LINE_10	"map   -> shows peripheral map.       \n"
#define CMD_HELP_LINE_11	"clear -> clears terminal.            \n"
#define CMD_HELP_LINE_12	"pool  -> system wide variable        \n"
#define CMD_HELP_LINE_13	"jump  -> branching conditions        \n"
#define CMD_HELP_LINE_14	"                                     \n"
#define CMD_HELP_LINE_15	"proc  -> process scheduler.\n"
#define CMD_HELP_LINE_16	"                         \n"
#define CMD_HELP_LINE_17	"Use command -h to get command specific help.\n"
#define CMD_HELP_LINE_18	"Note: Some commands are not yet implemented.\n"
#define CMD_HELP_LINE_19	"Note: if command is called without arguments\n"
#define CMD_HELP_LINE_20	"      add a space after command name.\n"
#define CMD_HELP_FOOTER		"eof\n"


void CmdHelp(void);
void CmdPrintHelp(void);


#endif
