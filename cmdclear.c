

#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdclear.h"

void CmdClear(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdClearHelp();
			break;
		}
	}
	else {
		UARTprintf(CLS);
	}

}

void CmdClearHelp(void) {
	UARTprintf(CMD_CLEAR_HEADER);
	UARTprintf(CMD_CLEAR_LINE_1);
	UARTprintf(CMD_CLEAR_FOOTER);
}
