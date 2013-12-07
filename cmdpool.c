#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils\uartstdio.h"
#include "cmdlib.h"

//*****************************************************************************
//*****************************************************************************
variable_info_t genericVariable[MAX_VARIABLES];

//*****************************************************************************
// holds index of working variable
//*****************************************************************************
uint8_t variableIdx = 0;

//*****************************************************************************
// Mappings for bargraph
//*****************************************************************************
uint8_t bargraphSingle[BARGRAPH_SIZE] = {1, 2, 4, 8, 16, 32, 64, 128};
uint8_t bargraphFilled[BARGRAPH_SIZE] = {1, 3, 7, 15, 31, 63, 127, 255};

//*****************************************************************************
//*****************************************************************************
void CmdPool(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();
	static bool init = true;
	bool ret = false;
	uint8_t i = 0;

	uint8_t var = 0;

	// get second argument
	uint8_t argS = (uint8_t)(cmdPtr->currentArgs[ARG_2] - KEY_ZERO);

	// check if second argument is numeric char, false if alphachar
	ret = isNumericChar(cmdPtr->currentArgs[ARG_2]);

	if (init) {
		// Initialize variables
		for (i = 0; i < MAX_VARIABLES; i++) {
			genericVariable[i].writeEnable = true;
		}
		// Set default access variable
		variableIdx = 0;
		// disble init
		init = false;
		UARTprintf(CMD_VARIABLE_INIT);
	}

	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdPoolHelp();
			break;

			case 'i':
			if (ret) {
				var = IncrementVariable(argS);
				UARTprintf(CMD_VALUE_SET, var);
			}
			else {
				UARTprintf(CMD_VALUE_SET_FAIL);
			}
			break;

			case 'd':
			if (ret) {
				var = DecrementVariable(argS);
				UARTprintf(CMD_VALUE_SET, var);
			}
			else {
				UARTprintf(CMD_VALUE_SET_FAIL);
			}
			break;

			case 'l':
			if (ret) {
				var = LeftShiftVariable(argS);
				UARTprintf(CMD_VALUE_SET, var);
			}
			else {
				UARTprintf(CMD_VALUE_SET_FAIL);
			}
			break;

			case 'r':
			if (ret) {
				var = RightShiftVariable(argS);
				UARTprintf(CMD_VALUE_SET, var);
			}
			else {
				UARTprintf(CMD_VALUE_SET_FAIL);
			}
			break;

			case 'z':
			var = SetGlobalVariableValue(0);
			UARTprintf(CMD_VALUE_SET, var);
			break;

			case 'v':
			var = InvertVariable();
			UARTprintf(CMD_VALUE_SET, var);
			break;

			case 'w':
			ToggleAccessRights();
			break;

			case 'a':
			var = AccumulateStack();
			UARTprintf(CMD_POOL_STACK_ER, var);
			break;

			case 'n':
			var = RandomizeVariable();
			UARTprintf(CMD_VALUE_SET, var);
			break;

			case 'Z':
			ResetStackVariables();
			UARTprintf(CMD_STACK_RESET);
			break;

			case 'u':
			var = ConvertToBargraph(BARGRAPH_SINGLE);
			SetGlobalVariableValue(var);
			UARTprintf(CMD_VARIABLE_REMAP, var);
			break;

			case 'f':
			var = ConvertToBargraph(BARGRAPH_FILLED);
			SetGlobalVariableValue(var);
			UARTprintf(CMD_VARIABLE_REMAP, var);
			break;

			case 's':
			SelectVariable();
			break;

			case 'c':
			CopyVariable();
			break;

			case 'o':
			PrintAllVariables();
			break;
		}
	}
	else {
		PrintVariableInfo();
	}

}
//*****************************************************************************
//*****************************************************************************
variable_info_t *GetPointerToVariable (void) {
	return &genericVariable[0];
}
//*****************************************************************************
//*****************************************************************************
void CopyVariable (void) {
	uart_cmd_t *c = GetCmdPointer();
	variable_info_t *var = GetPointerToVariable();
	uint8_t sourceId, destId = 0, value;
	uint8_t source, dest;
	bool copyFailed = false;

	// obtain destination index
	if (isNumericChar(c->currentArgs[ARG_2])) {
		destId = (uint8_t)(c->currentArgs[ARG_2] - KEY_ZERO);
		if (destId < MAX_VARIABLES) {
			// obtain current value at source 
			value = GetGlobalVariableValue();
			// set pointer variable to destination
			// preserve source pointer variable
			sourceId = variableIdx;
			variableIdx = destId;
			// modify variable at new destination
			SetGlobalVariableValue(value);
			// restore variable pointer
			variableIdx = sourceId;
			// check if copy succeded
			source = var[sourceId].varValue[0];
			dest = var[destId].varValue[0];
			if (source == dest) {
				copyFailed = false;
			} else {
				copyFailed = true;
			}
			UARTprintf(CMD_COPY_MAIN, sourceId, destId, copyFailed ? CMD_FAILED : CMD_OK);
		} else {
			UARTprintf(CMD_ERORR_VALUE, destId);
		}
	} else {
		UARTprintf(CMD_ERORR_VALUE, destId);
	}
}

//*****************************************************************************
//*****************************************************************************
void SelectVariable(void) {
	uart_cmd_t *c = GetCmdPointer();
	uint8_t id = 0;

	if (isNumericChar(c->currentArgs[ARG_2])) {
		id = (uint8_t)(c->currentArgs[ARG_2] - KEY_ZERO);
		if (id < MAX_VARIABLES) {
			variableIdx = id;
			UARTprintf(CMD_VALUE_SWITCH, id);
		} else {
			UARTprintf(CMD_ERORR_VALUE, id);
		}
	} else {
		UARTprintf(CMD_ERORR_VALUE, id);
	}

}
//*****************************************************************************
//*****************************************************************************
uint8_t IncrementVariable(uint8_t i) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable;

	tempVariable = GetGlobalVariableValue() + i;
	SetGlobalVariableValue(tempVariable);

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
uint8_t *GetBargraphPointer(uint8_t type) {
	if (type == BARGRAPH_SINGLE) {
		return &bargraphSingle[0];
	} else {
		return &bargraphFilled[0];
	}
}

//*****************************************************************************
//*****************************************************************************
uint8_t ConvertToBargraph(uint8_t type) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t *b = GetBargraphPointer(type);
	uint8_t tempVariable, bargraphIndex;

	// Get Current variable
	tempVariable = GetGlobalVariableValue();

	// get index
	bargraphIndex = tempVariable / BARGRAPH_SCALE;

	// return mapping
	tempVariable = b[bargraphIndex];

	return tempVariable;
}
//*****************************************************************************
// Sums up the the stack values, clears the rest
//*****************************************************************************
uint8_t AccumulateStack (void) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable = 0;
	uint8_t i;

	for (i = 0; i < POOL_STACK_SIZE; i++) {
		tempVariable += var[variableIdx].varValue[i];
		var[variableIdx].varValue[i] = 0;
	}
	var[variableIdx].varValue[0] = tempVariable;

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
void ResetStackVariables(void) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t i;
	for (i = 0; i < POOL_STACK_SIZE; i++) {
		var[variableIdx].varValue[i] = 0;
	}
}

//*****************************************************************************
//*****************************************************************************
uint8_t RandomizeVariable (void) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable = 0;

	tempVariable = rand() % 0xFF + 1;
	SetGlobalVariableValue(tempVariable);

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
uint8_t DecrementVariable(uint8_t i) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable;

	tempVariable = GetGlobalVariableValue() - i;
	SetGlobalVariableValue(tempVariable);

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
uint8_t LeftShiftVariable(uint8_t i) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable;

	tempVariable = (GetGlobalVariableValue() << i);
	SetGlobalVariableValue(tempVariable);

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
uint8_t RightShiftVariable(uint8_t i) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable;

	tempVariable = (GetGlobalVariableValue() >> i);
	SetGlobalVariableValue(tempVariable);

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
uint8_t InvertVariable(void) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t tempVariable;

	tempVariable = ~(GetGlobalVariableValue());
	SetGlobalVariableValue(tempVariable);

	return var[variableIdx].varValue[0];	
}

//*****************************************************************************
//*****************************************************************************
uint8_t SetGlobalVariableValue(uint8_t value) {
	variable_info_t *var = GetPointerToVariable();
	timing_info_t *timing = GetTimerPointer();
	uint8_t i;

	// check if we're allowed to modify
	if (var[variableIdx].writeEnable) {

		// left shift array
		for (i = 1; i <= POOL_STACK_SIZE - 1; i++) {
			var[variableIdx].varValue[POOL_STACK_SIZE - i] = var[variableIdx].varValue[POOL_STACK_SIZE - i - 1];
		}

		// append new value
		var[variableIdx].varValue[0] = value;
	}

	// update last access info
	var[variableIdx].accessTimeMs += 1;

	return var[variableIdx].varValue[0];
}

//*****************************************************************************
// returns value of global variable, this will probably be used by external
// functions.
//*****************************************************************************
uint8_t GetGlobalVariableValue(void) {
	variable_info_t *var = GetPointerToVariable();
	return var[variableIdx].varValue[0];
}

//*****************************************************************************
//*****************************************************************************
void PrintVariableInfo(void) {
	variable_info_t *var = GetPointerToVariable();
	uint8_t i;

	UARTprintf(CMD_VAR_LINE_TOP, variableIdx);
	UARTprintf(CMD_VAR_LINE_1);
	UARTprintf(CMD_VAR_LINE_2, var[variableIdx].varValue[0]);
	UARTprintf(CMD_VAR_LINE_3, var[variableIdx].accessTimeMs);
	UARTprintf(CMD_VAR_LINE_4, var[variableIdx].writeEnable ? CMD_AC_RW : CMD_AC_RO);
	UARTprintf(CMD_VAR_LINE_5);
	for (i = 1; i < POOL_STACK_SIZE; i++) {
		UARTprintf("0x%x  ", var[variableIdx].varValue[i]);
	}
	UARTprintf(CMD_VAR_LINE_6);
}

//*****************************************************************************
//*****************************************************************************
void PrintAllVariables (void) {
	variable_info_t *v = GetPointerToVariable();
	uint8_t i, k;

	for (i = 0; i < MAX_VARIABLES; i++) {
		UARTprintf(CMD_VAR_LINE_5);
		for (k = 0; k < POOL_STACK_SIZE; k++) {
			UARTprintf("0x%x  ", v[i].varValue[k]);
		}
	}
}

//*****************************************************************************
//*****************************************************************************
bool isNumericChar(char a) {

	bool ret = false;

	if (a >= KEY_ZERO && a <= KEY_NINE) {
		ret = true;
	}

	return ret;
}

//*****************************************************************************
//*****************************************************************************
void ToggleAccessRights (void) {
	variable_info_t *var = GetPointerToVariable();
	timing_info_t *timing = GetTimerPointer();

	if (var[variableIdx].writeEnable) {
		var[variableIdx].writeEnable = false;
	}
	else {
		var[variableIdx].writeEnable = true;
	}

	// Tell the user about current access mode
	UARTprintf(CMD_ACCESS_RIGHT, var[variableIdx].writeEnable ? CMD_AC_RW : CMD_AC_RO);

	// update last access info
	var[variableIdx].accessTimeMs = timing->miliSeconds + (timing->seconds * 1000);

}

//*****************************************************************************
//*****************************************************************************
void CmdPoolHelp(void) {
	UARTprintf(CMD_POOL_HEADER);
	UARTprintf(CMD_POOL_LINE_1);
	UARTprintf(CMD_POOL_LINE_2);
	UARTprintf(CMD_POOL_LINE_3);
	UARTprintf(CMD_POOL_LINE_4);
	UARTprintf(CMD_POOL_LINE_5);
	UARTprintf(CMD_POOL_LINE_6);
	UARTprintf(CMD_POOL_LINE_7);
	UARTprintf(CMD_POOL_LINE_8);
	UARTprintf(CMD_POOL_LINE_9);
	UARTprintf(CMD_POOL_LINE_10);
	UARTprintf(CMD_POOL_LINE_11);
	UARTprintf(CMD_POOL_LINE_12);
	UARTprintf(CMD_POOL_LINE_13);
	UARTprintf(CMD_POOL_LINE_14);
	UARTprintf(CMD_POOL_LINE_15);
	UARTprintf(CMD_POOL_LINE_16);
	UARTprintf(CMD_POOL_LINE_17);
	UARTprintf(CMD_POOL_FOOTER);
}
