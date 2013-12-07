#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdhelp.h"


void CmdHelp(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdPrintHelp();
			break;
		}
	}
	else {
		UARTprintf(CMD_ZERO_ARGS);
		UARTprintf(CMD_DEF_HELP);
	}

}

void CmdPrintHelp(void) {
	UARTprintf(CMD_HELP_HEADER);
	UARTprintf(CMD_HELP_LINE_1);
	UARTprintf(CMD_HELP_LINE_2);
	UARTprintf(CMD_HELP_LINE_3);
	UARTprintf(CMD_HELP_LINE_4);
	UARTprintf(CMD_HELP_LINE_5);
	UARTprintf(CMD_HELP_LINE_6);
	UARTprintf(CMD_HELP_LINE_7);
	UARTprintf(CMD_HELP_LINE_8);
	UARTprintf(CMD_HELP_LINE_9);
	UARTprintf(CMD_HELP_LINE_10);
	UARTprintf(CMD_HELP_LINE_11);
	UARTprintf(CMD_HELP_LINE_12);
	UARTprintf(CMD_HELP_LINE_13);
	UARTprintf(CMD_HELP_LINE_14);
	UARTprintf(CMD_HELP_LINE_15);
	UARTprintf(CMD_HELP_LINE_16);
	UARTprintf(CMD_HELP_LINE_17);
	UARTprintf(CMD_HELP_LINE_18);
	UARTprintf(CMD_HELP_LINE_19);
	UARTprintf(CMD_HELP_LINE_20);
	UARTprintf(CMD_HELP_FOOTER);
}
