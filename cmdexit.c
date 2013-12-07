#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdexit.h"


void CmdExit (void) {
	uart_cmd_t *cmdPtr = GetCmdPointer();
	if (cmdPtr->argsNum) {
		switch (*cmdPtr->currentArgs) {
			case 'h':
			CmdExitHelp();
			break;
		}
	}
	else {
		UARTprintf(CMD_EXIT_INFO);
		SysCtlDelay(SysCtlClockGet()/5);
		UARTprintf(CMD_EXIT_TICK);
		SysCtlDelay(SysCtlClockGet()/5);
		UARTprintf(CMD_EXIT_TICK);
		SysCtlDelay(SysCtlClockGet()/5);
		UARTprintf(CMD_EXIT_TICK);
		UARTprintf("\n");
		SysCtlReset();
	}

}


void CmdExitHelp (void) {
	UARTprintf(CMD_EXIT_HEADER);
	UARTprintf(CMD_EXIT_LINE_1);
	UARTprintf(CMD_EXIT_LINE_2);
	UARTprintf(CMD_EXIT_FOOTER);
}
