#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdtemplate.h"

void CmdTemplate(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdTemplateHelp();
			break;
		}
	}
	else {
		UARTprintf(CMD_ZERO_ARGS);
		UARTprintf(CMD_DEF_TEMPLATE);
	}

}

void CmdTemplateHelp(void) {
	UARTprintf(CMD_TEMPLATE_HEADER);
	UARTprintf(CMD_TEMPLATE_LINE_1);
	UARTprintf(CMD_TEMPLATE_FOOTER);
}
