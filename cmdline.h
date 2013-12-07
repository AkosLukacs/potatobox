#ifndef __CMDLINE_H__
#define __CMDLINE_H__

#include <stdbool.h>
#include <stdint.h>

//*****************************************************************************
// Hearbeat configuration settings
//*****************************************************************************
#define HEARTBEAT_CTRL_PORT SYSCTL_PERIPH_GPIOF	
#define HEARTBEAT_BASE_PORT GPIO_PORTF_BASE
#define HEARTBEAT_PIN GPIO_PIN_3
#define HEARTBEAT_INTERVAL	2

//*****************************************************************************
// The following defines can be used to define certain aspects of the 
// command processing, such as prompt, default error message, buffer size
// and number of arguments.
//*****************************************************************************
#define INPUT_PROMPT 	"\nLM4F>>"					// normal prompt
#define STACK_PROMPT	"\nLM4F$:%02d>"				// used during scripting 
#define CMD_NOT_FOUND	"\nCommand not found."
#define CMD_PROC_BUSY	"\nPlease wait for command to finish."
#define CMD_OVERFLOW	"\nCan't handle this."
#define CMD_DEF_HELP	"\nUse command -h, for help."
#define CMD_ZERO_ARGS	"\nNo arguments found."
#define CMD_CANCEL_PROC	"\nCanceling execution at %d : 0x%x."
#define CMD_PTR_ERROR	"\nCommand table pointer not assigned."
#define CMD_INV_IDX		"\nInvalid index position specified: %d."
#define CMD_MULT_FINISH	"\nExecution finished of %d elements."

#define BUFFER_SIZE 64							// size of uart buffer	
#define MAX_ARGS	3 							// number of arguments
#define MAX_COMMANDS 32 						// number of commands to store
#define MIN_CMD_LENGTH 3 						// minimum command length
#define COMMAND_STACK_DEPTH 255 				// command stack length
#define TIMING_PROFILE_DEPTH 255				// how many timing parameters to keep in memory


//*****************************************************************************
// System defines, DO NOT MODIFY
//*****************************************************************************
#define ARG_1 0
#define ARG_2 1 
#define ARG_3 2

#define SYSTICK_MAX	16777216 					
#define SYSTICK_TIME	 100					

//*****************************************************************************
// Do not modify the following defines!
// They represent control characters from standard ascii table.
// New characters can be added if required
//*****************************************************************************
#define KEY_ESCAPE		0x1B
#define KEY_ENTER		0x0d
#define KEY_SPACE		0x20
#define KEY_BACKSPACE	0x7F
#define KEY_DASH		0x2d

#define KEY_ZERO		0x30
#define KEY_SEVEN		0x37
#define KEY_NINE		0x39
#define KEY_ALPHA		0x61
#define KEY_HEX			0x66
#define KEY_NINE		0x39
#define HEX_OFFSET		0x0A

#define KEY_CTRL_START	0x1B	// control character start
#define KEY_CTRL_BEND	0x34	// End 	 	
#define KEY_CTRL_END	0x7E	// control character stop
//*****************************************************************************
// This structure is populated by the uart interrupt handler.
//
// currentChar   - the last char recieved by the interrupt hadnler
// uarBuf        - holds characters recieved so far
// rxCount       - holds the number of characters recieved so far
// lastIntStatus - last interrupt status that triggered the interrupt handler
// cmdFound      - is true if command is found, when enter is pressed
// ledFlag       - true if led is on
// enterFlag     - true if enter has been pressed, causes interrupt handler
//				   to process current buffer
// procBusy      - true if command is being executed, characters sent to 
//				   uart are not processed
// macroMode	 - true if command execution is skipped
// displayStats  - if true, displays debug data, when enter is pressed
// bytesTotal    - counts the number of bytes recieved during current session
//*****************************************************************************
typedef struct {
	char currentChar;					
	char uartBuf[BUFFER_SIZE];			

	uint8_t rxCount;					
	uint8_t cmdPos;					

	unsigned long lastIntStatus;

	bool cmdFound;
	bool ledFlag;
	bool enterFlag;
	bool procBusy;

	bool displayStats;
	unsigned long bytesTotal;
	
} uart_info_t;

//*****************************************************************************
// This structure is used to hold information related to which command is about
// to be executed, which arguments were passed and if execution is complete.
//
// currentCmd		- holds encoded command string
// cmdIdx			- holds index at which the currentCmd was found in command
//					  table.
// *cmdTblPtr		- holds pointer to table which contains encoded strings that
//					  can be executed
// currentArgs		- contains current arguments that were passed with command
// argsNum			- holds number of arguments passed, 0 if no arguments.
// ExecComplete		- true if execution of current command is complete.
//*****************************************************************************
typedef struct {

	uint16_t currentCmd;			// encoded string
	uint16_t cmdIdx;				// index
	uint16_t *cmdTblPtr;			// pointer to encoded string table
	uint8_t argsNum;				// number of arguments
	
	char currentArgs[MAX_ARGS]; 	// list of arguments

	bool commandFound;
	bool ExecComplete;
	
} uart_cmd_t;

//*****************************************************************************
// This structure holds system timing properties
//*****************************************************************************
typedef struct 
{
	volatile uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint16_t days;
	volatile uint16_t miliSeconds;

	unsigned long cmdStart;
	unsigned long cmdStop;
	unsigned long elapsedTime;

	unsigned long elapsedTimeProfile[TIMING_PROFILE_DEPTH];
	uint8_t eTindex;
	
} timing_info_t;

//*****************************************************************************
// This structure holds stack info
//*****************************************************************************
typedef struct {

	uint8_t stackIdx;
	uint8_t startIdx;
	uint8_t stopIdx;
	uint8_t currIdx;

	bool execFromStack;
	bool saveCommands;
	bool contExecution;

} stack_info_t;


//*****************************************************************************
// Function Prototypes
// 
// Register UartIntHandler and SysTickIntHandler in startup.s
// Add both pointers to their respective interrupt vectors
//*****************************************************************************

void InitCommandLineSystem (void);
void UartIntHandler (void);
void SysTickIntHandler (void);

uart_info_t *GetUartPointer(void);
uart_cmd_t *GetCmdPointer(void);
uart_cmd_t *GetUartStackPointer(void);
timing_info_t *GetTimerPointer(void);
stack_info_t *GetStackPointer(void);

char GetArgument(uint8_t argNum);

void CMDLineSplashScreen (void);
void CMDLineProcessInit (void);
void CMDLineScheduler (void);
void CMDResetCtrlStructure (uart_cmd_t *cmd); 
void UARTResetCtrlStructure (uart_info_t *packet);
void TimingResetCtrlStructure (timing_info_t *timing); 
void StackResetCtrlStructure (stack_info_t *stackPtr);

uint16_t UARTEncodeCommand	(uart_info_t *packet);	
uint8_t UARTGetArguments (uart_info_t *packet, uart_cmd_t *cmd); 
uint16_t EncodeCommand (char *command, uint8_t size); 

bool FindCommand (uart_cmd_t *cmd); 
bool ExecuteCommand (uart_cmd_t *cmd);

void CalculateUsage (timing_info_t *timing);

void SaveToStack (uart_cmd_t *cmd);
void GetFromStack (uint8_t position);
void ExecuteFromStack(void); 
void PrintStack (uint8_t position); 

void SaveCommands(bool enable);
bool StackExecution(bool enable); 

bool RegisterCommandCode (uint8_t pos, uint16_t cmdCode);
bool RegisterCommandFunc (uint8_t pos, void (*handler)(void));

bool EvaluateStringCommand (char *command);
bool EvaluateMultipleCommands (char *cmd[], uint8_t cmdNum);

void InitCortexHardware(void);


#endif 
