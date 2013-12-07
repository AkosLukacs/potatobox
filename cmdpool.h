#ifndef __CMDPOOL_H__
#define __CMDPOOL_H__

#include <stdbool.h>
#include <stdint.h>
 
#define CMD_POOL_HEADER		"\nAvailable commands:\n"
#define CMD_POOL_LINE_1		"pool -h       -> Displays this help.\n"
#define CMD_POOL_LINE_2		"pool          -> Detailed info about variable.\n"
#define CMD_POOL_LINE_3		"pool -i -1    -> Increments variable by one.\n                 Previous value is pushed down the stack.\n"
#define CMD_POOL_LINE_4     "pool -d -2    -> Decrement variable by two.\n                  Previous value is pushed down the stack.\n"
#define CMD_POOL_LINE_5		"pool -l -1    -> Left shift variable by one.\n"
#define CMD_POOL_LINE_6		"pool -r -3    -> Right shift variable by three.\n"
#define CMD_POOL_LINE_7		"pool -z       -> Set variable to zero.\n"
#define CMD_POOL_LINE_8		"pool -v       -> Inverts variable.\n"
#define CMD_POOL_LINE_9		"pool -w       -> Set access to variable.\n"
#define CMD_POOL_LINE_10	"pool -a       -> Accumulate stack values.\n"
#define CMD_POOL_LINE_11	"pool -n       -> Randomizes pool variable.\n"
#define CMD_POOL_LINE_12	"pool -Z       -> Sets all variables to zero.\n                 This will wipe all stack memory.\n"
#define CMD_POOL_LINE_13	"pool -f       -> Converts variable value to filled bargraph value.\n                 Usefull when creating progress bar.\n"
#define CMD_POOL_LINE_14    "pool -u       -> Converts variable value to unfilled bargraph value.\n"
#define CMD_POOL_LINE_15	"pool -s -0    -> Selects variable to perform operations on.\n"
#define CMD_POOL_LINE_16	"pool -c -3    -> Copies variable from current selection to index 3.\n"
#define CMD_POOL_LINE_17	"pool -o       -> Show current values for all variables.\n"
#define CMD_POOL_FOOTER		"eof"


#define CMD_VALUE_SET		"\nVariable value set to 0x%x."
#define CMD_POOL_STACK_ER	"\nStack Accumulated: 0x%x."
#define CMD_VALUE_SET_FAIL	"\nCan't set variable."
#define CMD_ACCESS_RIGHT	"\nVariable access set to: %s"
#define CMD_VARIABLE_INIT	"\nVariable initialized."
#define CMD_STACK_RESET		"\nStack reset to zero."
#define CMD_VARIABLE_REMAP	"\nVariable remapped to: 0x%x"
#define CMD_AC_RW			" read/write."
#define CMD_AC_RO			" read only."
#define CMD_ERORR_VALUE		"\nWrong value: %d."
#define CMD_COPY_MAIN		"\nCopy from %d to %d %s"
#define CMD_FAILED			"failed."
#define CMD_OK 				"ok."
#define CMD_VALUE_SWITCH	"\nSwitched to variable at %d."

#define CMD_VAR_LINE_TOP	"\nVariable index: %d."
#define CMD_VAR_LINE_1		"\nVariable access info:"
#define CMD_VAR_LINE_2		"\n- value : 0x%x"
#define CMD_VAR_LINE_3		"\n- accessed %d times."
#define CMD_VAR_LINE_4		"\n- access mode: %s"
#define CMD_VAR_LINE_5		"\n- stack: "
#define CMD_VAR_LINE_6		"\nVariable info end."

#define MAX_VARIABLES 10
#define POOL_STACK_SIZE	10
#define BARGRAPH_SIZE 8
#define BARGRAPH_SCALE 35

#define BARGRAPH_FILLED 0 
#define BARGRAPH_SINGLE 1


typedef struct {

	uint8_t varValue[POOL_STACK_SIZE];				// 0 - current value, 1...n - previous values
	uint16_t accessTimeMs;							// how many times this variable was accessed
	bool writeEnable;								// read write access

	char lastcommand[9];							// not in use

} variable_info_t;

variable_info_t *GetPointerToVariable (void);

void CmdPool (void);
void CmdPoolHelp (void);

uint8_t IncrementVariable(uint8_t i);
uint8_t DecrementVariable(uint8_t i);
uint8_t LeftShiftVariable(uint8_t i);
uint8_t RightShiftVariable(uint8_t i);
uint8_t AccumulateStack (void);
void ResetStackVariables(void);
uint8_t RandomizeVariable (void);
uint8_t InvertVariable(void);
uint8_t SetGlobalVariableValue(uint8_t value);
uint8_t GetGlobalVariableValue(void);
uint8_t ConvertToBargraph(uint8_t type);
uint8_t *GetBargraphPointer(uint8_t type);
void SelectVariable(void);
void CopyVariable (void);
void PrintAllVariables (void);


bool isNumericChar(char a);
void PrintVariableInfo(void);
void ToggleAccessRights (void);

#endif
