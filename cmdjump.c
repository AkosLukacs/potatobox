#include <stdint.h>
#include <stdbool.h>
#include "cmdlib.h"
#include "utils\uartstdio.h"

uint8_t branchIndex = 0;
uint8_t branchValue = 0;

void CmdJump(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdJumpHelp();
			break;

			case 'v':
			SetBranchValue();
			break;

			case 'i':
			SetBranchIndex();
			break;

			case 'l':
			CompareAndBranch(TYPE_LARGER);
			break;

			case 's':
			CompareAndBranch(TYPE_SMALLER);
			break;

			case 'e':
			CompareAndBranch(TYPE_EQUAL);
			break;

			case 'n':
			CompareAndBranch(TYPE_NOT_EQUAL);
			break;

			case 'd':
			PrintBranchingInfo();
		}
	}
	else {
		UARTprintf(CMD_NO_ARGS_ERROR);
	}

}

void PrintBranchingInfo(void) {
	UARTprintf(BRANCHING_INFO, branchValue, branchIndex);
}

void SetBranchValue(void) {
	uart_cmd_t *c = GetCmdPointer();
	stack_info_t *s = GetStackPointer();
	uint8_t value  = 0;

	// get branching value from arguments
	value = ConvertArgToHex(GetArgument(1)) << 4;
	value |= ConvertArgToHex(GetArgument(2));

	// check the number of arguments and bounds
	if (c->argsNum == 3 && value <= MAX_BRANCHING_VALUE) {
		branchValue = value;
		UARTprintf(BRANCHING_VALUE, branchValue);
	} else {
		UARTprintf(INVALID_ARGUMENTS, GetArgument(1), GetArgument(2));
	}
}

void SetBranchIndex(void) {
	uart_cmd_t *c = GetCmdPointer();
	stack_info_t *s = GetStackPointer();
	uint8_t index  = 0;

	// get branching index from arguments
	index = ConvertArgToHex(GetArgument(1)) << 4;
	index |= ConvertArgToHex(GetArgument(2));


	// check the number of arguments and bounds
	if (c->argsNum == 3 && index <= MAX_BRANCHING_VALUE) {
		branchIndex = index;
		UARTprintf(BRANCHING_VALUE, index);
	} else {
		UARTprintf(CMD_INVALID_INDEX, index, MAX_BRANCHING_VALUE);
	}
}

void CompareAndBranch(uint8_t type) {
	uart_cmd_t *c = GetCmdPointer();
	stack_info_t *s = GetStackPointer();
	uint8_t value = GetGlobalVariableValue();

	// check if scripting
	if (s->saveCommands) {
		UARTprintf(CMD_JUMP_RECORDED);
	} else {
		UARTprintf(CMD_NO_EXEC_SCRIPT);
	}

	// check if we executing from stack
	if (!s->execFromStack) {
		UARTprintf(CMD_JUMP_SKIPPED);
		return;
	}

	// perform compare and jump
	if (type == TYPE_EQUAL && branchValue == value) {
		s->currIdx = branchIndex -1;
		UARTprintf(BRANCHING_TO, branchIndex);
		return;
	}

	if (type == TYPE_NOT_EQUAL && branchValue != value){
		s->currIdx = branchIndex -1;
		UARTprintf(BRANCHING_TO, branchIndex);
		return;
	}

	if (type == TYPE_SMALLER && branchValue < value) {
		s->currIdx = branchIndex -1;
		UARTprintf(BRANCHING_TO, branchIndex);
		return;
	}

	if (type == TYPE_LARGER && branchValue > value) {
		s->currIdx = branchIndex -1;
		UARTprintf(BRANCHING_TO, branchIndex);
		return;		
	} else {
		UARTprintf(NO_BRANCHING);
	}
}

void CmdJumpHelp(void) {
	UARTprintf(CMD_JUMP_HEADER);
	UARTprintf(CMD_JUMP_LINE_1);
	UARTprintf(CMD_JUMP_LINE_2);
	UARTprintf(CMD_JUMP_LINE_3);
	UARTprintf(CMD_JUMP_LINE_4);
	UARTprintf(CMD_JUMP_LINE_5);
	UARTprintf(CMD_JUMP_LINE_6);
	UARTprintf(CMD_JUMP_LINE_7);
	UARTprintf(CMD_JUMP_LINE_8);
	UARTprintf(CMD_JUMP_LINE_9);
	UARTprintf(CMD_JUMP_FOOTER);
}
