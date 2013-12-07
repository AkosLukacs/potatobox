#ifndef __CMDEXIT_H__
#define __CMDEXIT_H__

#define CMD_EXIT_HEADER		"\nAvailable commands:\n"
#define CMD_EXIT_LINE_1		"exit          -> Forces a device wide reset.\n"
#define CMD_EXIT_LINE_2		"exit -h       -> Displays this help.\n"
#define CMD_EXIT_FOOTER		"eof"

#define CMD_EXIT_INFO		"\nReseting device: "
#define CMD_EXIT_TICK		"."

void CmdExit (void);
void CmdExitHelp (void);

#endif
