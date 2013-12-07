#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdqueue.h"


void CmdQueue (void) {
	uart_cmd_t *cmd = GetCmdPointer();
	stack_info_t *stack = GetStackPointer();
	uint8_t startAddr = 0;
	uint8_t stopAddr  = 0;

	if (cmd->argsNum < 2) {
		switch(GetArgument(0)){
			case 'h': CmdQueueHelp();
			break;

			case 'e': 
				SaveCommands(false);
				UARTprintf(STACK_SAVE_OFF);
				QueueExecFromStack(0, COMMAND_STACK_DEPTH, false); 
			break;

			case 'd': 
				SaveCommands(false); 
				UARTprintf(STACK_SAVE_OFF);
			break;

			case 's': 
				SaveCommands(true); 
				UARTprintf(STACK_SAVE_ON);
				break;

			case 'r':
				// since stack is uint8_t setting
				// to max will cause overflow on next iteration
				// setting it to 0, will cause it to increment
				// to 1 on nex iteration, which is wrong. 
				stack->stackIdx = 255;	
				UARTprintf(STACK_RESET);
				break;

			case 'f': 
				SaveCommands(false);
				UARTprintf(STACK_SAVE_OFF);
				// TODO: Check if stack contains data, before
				// allowing to execute from stack!
				QueueExecFromStack(0, COMMAND_STACK_DEPTH, true);
			break;

			case 'z':
				SaveCommands(false);
				QueueDeleteStack();
			break;

			case 'b':
				QueueCancelExecFromStack();
			break;
		}
	}
	else if (cmd->argsNum == 2) {
		startAddr = GetArgument(1) - KEY_ZERO;
		stopAddr = COMMAND_STACK_DEPTH - 1;

		if (startAddr > stopAddr) {
			UARTprintf(START_ADDR_ERROR, startAddr);
		}
		else {
			UARTprintf(EXECUTION_START, startAddr);
			QueueExecFromStack(startAddr, COMMAND_STACK_DEPTH, false);
		}
	}
	else if (cmd->argsNum > 2) {
		startAddr = GetArgument(1) - KEY_ZERO;
		stopAddr = GetArgument(2) - KEY_ZERO;
		if (startAddr > stopAddr){
			UARTprintf(START_ADDR_ERROR, startAddr);	
		}
		else if (stopAddr > COMMAND_STACK_DEPTH - 1 ){
			UARTprintf(STOP_ADDR_ERROR, stopAddr);
		}
		else {
			UARTprintf(EXECUTION_START_STOP, startAddr, stopAddr);
			QueueExecFromStack(startAddr, stopAddr, false);
		}
	}
}

void QueueDeleteStack (void) {
	uart_cmd_t *p = GetUartStackPointer();
	stack_info_t *s = GetStackPointer();

	uint8_t i, k;

	for (i = 0; i <= COMMAND_STACK_DEPTH - 1; i++) {
		p[i].currentCmd = 0;
		p[i].cmdIdx = 0;
		p[i].argsNum = 0;

		for (k = 0; k <= MAX_ARGS - 1; k++) {
			p[i].currentArgs[k] = 0;	
		}

		p[i].commandFound = false;
		p[i].ExecComplete = true;

	}

	// set stack location to zero
	s->stackIdx = 0;

	
	UARTprintf(STACK_DELETE, i);
}

void QueueExecFromStack(uint8_t startAddr, uint8_t stopAddr, bool save) {
	stack_info_t *stack = GetStackPointer();
	stack->startIdx = startAddr;
	stack->stopIdx = stopAddr;
	stack->execFromStack = true;
	stack->saveCommands = false;
	stack->contExecution = save;

}

void QueueCancelExecFromStack (void) {
	stack_info_t *stack = GetStackPointer();
	if (stack->execFromStack) {
		stack->contExecution = false;
		stack->execFromStack = false;	
		UARTprintf(EXECUTION_BREAK, stack->currIdx);			
	} else {
		UARTprintf(EXECUTION_BREAK_INF);
	}
}

void CmdQueueHelp (void) {
	UARTprintf(CMD_QUEUE_HEADER);
	UARTprintf(CMD_QUEUE_LINE_1);
	UARTprintf(CMD_QUEUE_LINE_2);
	UARTprintf(CMD_QUEUE_LINE_3);
	UARTprintf(CMD_QUEUE_LINE_4);
	UARTprintf(CMD_QUEUE_LINE_5);
	UARTprintf(CMD_QUEUE_LINE_6);
	UARTprintf(CMD_QUEUE_LINE_7);
	UARTprintf(CMD_QUEUE_LINE_8);
	UARTprintf(CMD_QUEUE_LINE_9);
	UARTprintf(CMD_QUEUE_LINE_10);
	UARTprintf(CMD_QUEUE_FOOTER);
}
