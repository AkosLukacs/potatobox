#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdprint.h"


void CmdPrint(void) {

	uart_cmd_t *cmd = GetCmdPointer();

	uint8_t stackId = cmd->currentArgs[ARG_2] - KEY_ZERO;
	if (cmd->argsNum) {
		if (cmd->argsNum >= CMD_ARG_TRIGGER) {
			if (cmd->currentArgs[ARG_1] == 'c') {
				if (stackId <= CMD_STACK_MAX){
					PrintStack(stackId);
				}
				else {
					UARTprintf(CMD_INVALID_STACK, stackId);
				}
			}
			else {
				UARTprintf(CMD_INVALID_ARG);
			}
		}
		else {
			switch(cmd->currentArgs[ARG_1]) {

				case 'h':
				CmdDisplayHelp();
				break;

				case 'c':
				PrintCurrentCommand();
				break;

				case 'u':
				PrintCurrentUart();
				break;

				case 't':
				PrintCurrentTime();
				break;

				case 's':
				PrintCurrentStack();
				break;

				case 'a':
				PrintWholeStack();
				break;

				case 'k':
				PrintRegisteredCommands();
				break;
			}
		}
	}
	else {
		UARTprintf(CMD_NO_ARGS_ERROR);
	}
}

void PrintWholeStack(void) {
	int k;

	for (k = 0; k <= COMMAND_STACK_DEPTH -1; k++) {
	PrintStack(k);		
	}
}

void PrintCurrentCommand(void) {
	uart_cmd_t *cmd = GetCmdPointer();
	uint8_t args = cmd->argsNum;
	uint8_t i = 0;
	
	UARTprintf("\nEnviromental variables for CMD interface:\n");
	UARTprintf("- Current command code: %d\n",cmd->currentCmd);	
	UARTprintf("- Current command id: %d\n",cmd->cmdIdx);			
	UARTprintf("- Command Found: %s\n",cmd->commandFound ? "true" : "false");
	UARTprintf("- Number of arguments: %d\n",cmd->argsNum);
	UARTprintf("- Arguments: ");
	if (args) {
		while (args != 0) {
			UARTprintf("-%c ", cmd->currentArgs[i]);
			i++;
			args--;
		}
		UARTprintf("\n");
	}
	UARTprintf("- Execution complete: %s\n",cmd->ExecComplete ? "true" : "false");
	UARTprintf("eof\n");
}

void PrintCurrentStack(void) {
	stack_info_t *stackPtr = GetStackPointer();
	UARTprintf("\nEviromental variables for Stack interface:\n");
	UARTprintf("- Stack location 0x%x\n", stackPtr);
	UARTprintf("- Stack table index: %d \n", stackPtr->stackIdx);
	UARTprintf("- Stack execution index: %d \n", stackPtr->startIdx);
	UARTprintf("- Stack store enabled: %s \n", stackPtr->saveCommands ? "true" : "false");
	UARTprintf("- Stack execution enabled: %s \n", stackPtr->execFromStack ? "true" : "false");
	UARTprintf("eof\n");
}

void PrintCurrentUart(void) {
	uart_info_t *uart = GetUartPointer();
	UARTprintf("\nEnviromental variables for UART interface:\n");
	UARTprintf("- Last char in buffer: %c\n",uart->currentChar);	
	UARTprintf("- Bytes recieved: %d\n",uart->rxCount);
	UARTprintf("- Command position in buffer: %d\n",uart->cmdPos);
	UARTprintf("- Last Uart Interrupt status 0x%x\n",uart->lastIntStatus);
	UARTprintf("- Last command found? : %s\n",uart->cmdFound ? "true" : "false");
	UARTprintf("- Led Flag: %s\n",uart->ledFlag ? "true" : "false");
	UARTprintf("- Enter Flag: %s\n",uart->enterFlag ? "true" : "false");
	UARTprintf("- Process Busy: %s\n",uart->procBusy ? "true" : "false");
	UARTprintf("- Display Debug info: %s\n",uart->displayStats ? "true" : "false");
	UARTprintf("- Bytes on Rx (session): %d bytes\n",uart->bytesTotal);
	UARTprintf("eof\n");
}

void PrintCurrentTime(void) {
	timing_info_t *timePtr = GetTimerPointer();
	UARTprintf("\nRecent timing information:\n");
	UARTprintf("- Elapsed seconds: %d \n", timePtr->seconds);
	UARTprintf("- Elapsed minutes: %d \n", timePtr->minutes);
	UARTprintf("- Elapsed hours: %d \n", timePtr->hours);
	UARTprintf("- Elapsed days: %d \n", timePtr->days);
	UARTprintf("- Command execution start: %d ticks \n", timePtr->cmdStart);
	UARTprintf("- Command execution stop : %d ticks \n", timePtr->cmdStop);
	UARTprintf("- Command execution time : %d uS \n", timePtr->elapsedTime);
	UARTprintf("eof\n");

}

//*****************************************************************************
// Prints info on registered commands
// TODO: add addresses to functions. Must add a command first that returns
// a pointer to a function table;
//*****************************************************************************
void PrintRegisteredCommands(void) {
	uart_cmd_t *cmd = GetCmdPointer();
	uint16_t *cmdTable = cmd->cmdTblPtr;
	uint8_t i = 0;
	bool validCmd = true;	

	UARTprintf ("\nRegistered commands:\n");
	UARTprintf("  Id\tCode\tAddress\n");
	while (validCmd || i <= MAX_COMMANDS - 1) {
		validCmd = cmdTable[i] > 0 ? true : false;
		if (validCmd) {
			UARTprintf("- %04d\t%04d\t0x%x\n", i, cmdTable[i], &cmdTable[i]);
		}
		i++;
	}
	UARTprintf("%03d elements found.\n");
	UARTprintf("Table end.\n");
}

//*****************************************************************************
// Prints command info from stack
//
//*****************************************************************************
void PrintStack (uint8_t position) {
	uart_cmd_t *stack = GetUartStackPointer();

	uint8_t args = stack[position].argsNum;
	uint8_t i = 0;

	// check if valid command exists, skip overwise
	if (stack[position].commandFound){
		if (position <= COMMAND_STACK_DEPTH - 1) {
			UARTprintf("\nStack info begin: \n");
			UARTprintf("-Position: %d\n", position);
			UARTprintf("-Command code: %d\n",stack[position].currentCmd);
			UARTprintf("-Command table id: %d\n",stack[position].cmdIdx);		
			UARTprintf("-Command table addr: 0x%x\n"
					,stack[position].cmdTblPtr);		
			UARTprintf("-Command found: %s\n"
					,stack[position].commandFound ? "true" : "false");
			UARTprintf("-Number of arguments: %d\n"
					,stack[position].argsNum);
			if (args) {
				UARTprintf("-Arguments: ");
				while (args != 0) {
					UARTprintf("%c ", stack[position].currentArgs[i]);
					i++;
					args--;
				}
				UARTprintf("\n");
			}
			UARTprintf("Stack info end.\n");
		}
		else {
			UARTprintf("\nInvalid stack location requested: %d", position);
		}
	}
}

void CmdDisplayHelp(void) {
	UARTprintf(CMD_PRINT_HEADER);
	UARTprintf(CMD_PRINT_LINE_1);
	UARTprintf(CMD_PRINT_LINE_2);
	UARTprintf(CMD_PRINT_LINE_3);
	UARTprintf(CMD_PRINT_LINE_4);
	UARTprintf(CMD_PRINT_LINE_5);
	UARTprintf(CMD_PRINT_LINE_6);
	UARTprintf(CMD_PRINT_LINE_7);
	UARTprintf(CMD_PRINT_LINE_8);
	UARTprintf(CMD_PRINT_FOOTER);
}
