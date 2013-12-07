#include <stdbool.h>
#include <stdint.h>
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "utils\uartstdio.h"
#include "cmdlib.h"

//*****************************************************************************
// Create instances of control structures
//*****************************************************************************
uart_info_t uartData;
uart_cmd_t uartCmd;
timing_info_t timingData;
stack_info_t stack;

//*****************************************************************************
// Contains previous commands or code to be executed in sequence
//*****************************************************************************
uart_cmd_t uartCmdStack[COMMAND_STACK_DEPTH]; 

//*****************************************************************************
// This table holds encoded string that are supported by this command line 
// interface. MAX_COMMANDS refers to the size of this table.
//*****************************************************************************

uint16_t cmdTable[MAX_COMMANDS] = {0};
void (*cmdFuncTable[MAX_COMMANDS])(void) = {0};

//*****************************************************************************
// Simple Command for initializing hardware and command line scheduler
//*****************************************************************************
void InitCommandLineSystem (void) {

	bool retCode, retFunc;
	uint8_t componentNum = 0;

	//
	// Initialize device hardware
	//
	InitCortexHardware();
	UARTprintf("\n\nHardware Initialized.\n");

	//
	// Initialize process scheduler
	//
	ProcSchedulerInit ();
	UARTprintf("- Process scheduler enabled. \n");

	//
	// Initialize command line processor, must be
	// called before registering components
	//
	CMDLineProcessInit();
	UARTprintf("- Command line interpeter enabled.\n");
	
	//
	// Register Components
	//
	UARTprintf("- Registering components.\n");
	retCode = RegisterCommandCode(0, EncodeCommand("exit", 4));
	retFunc = RegisterCommandFunc(0, CmdExit);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(1, EncodeCommand("gpio", 4));
	retFunc = RegisterCommandFunc(1, CmdGpio);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(2, EncodeCommand("stat", 4));
	retFunc = RegisterCommandFunc(2, CmdStat);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(3, EncodeCommand("help", 4));
	retFunc = RegisterCommandFunc(3, CmdHelp);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(4, EncodeCommand("time", 4));
	retFunc = RegisterCommandFunc(4, CmdTime);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(5, EncodeCommand("print", 5));
	retFunc = RegisterCommandFunc(5, CmdPrint);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(6, EncodeCommand("queue", 5));
	retFunc = RegisterCommandFunc(6, CmdQueue);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(7, EncodeCommand("clear", 5));
	retFunc = RegisterCommandFunc(7, CmdClear);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(8, EncodeCommand("proc", 4));
	retFunc = RegisterCommandFunc(8, CmdProc);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(9, EncodeCommand("pool", 4));
	retFunc = RegisterCommandFunc(9, CmdPool);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(10, EncodeCommand("adc", 3));
	retFunc = RegisterCommandFunc(10, CmdAdc);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(11, EncodeCommand("jump", 4));
	retFunc = RegisterCommandFunc(11, CmdJump);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	retCode = RegisterCommandCode(13, EncodeCommand("script", 6));
	retFunc = RegisterCommandFunc(13, CMDScript);
	componentNum++;
	if (!retCode || !retFunc) {
		UARTprintf("Failed to register component.\n");
		componentNum--;
	}
	UARTprintf("- %d Components registered.\n", componentNum);

	//
	// Print prompt
	//
	UARTprintf(INPUT_PROMPT);
}
								
//*****************************************************************************
// This is the main routine of the command line processor, this routine should
// ideally run in the while loop in the main(), but it also can be run in 
// a task/thread on a RTOS
//
// This function checks if enter key has been detected by the interrupt
// service routine and if it's true, the processing of the buffer and
// execution of the command will take place
//
//*****************************************************************************
void CMDLineScheduler (void) {

	uart_info_t *packet = GetUartPointer();
	uart_cmd_t *cmd = GetCmdPointer();
	timing_info_t *timing = GetTimerPointer();
	stack_info_t *stack = GetStackPointer();

	if(packet->enterFlag) {
		CalculateUsage(timing);
		timing->cmdStart = SysTickValueGet();
		cmd->currentCmd = UARTEncodeCommand(packet); 
		UARTGetArguments(packet, cmd);
		if (!FindCommand(cmd)) {
			if (packet->rxCount >= MIN_CMD_LENGTH) {
				UARTprintf(CMD_NOT_FOUND);
			}
		}
		else {
			ExecuteCommand(cmd);
			SaveToStack(cmd);
			if (stack->execFromStack) {
				ExecuteFromStack();
			}			
		}	
		CMDResetCtrlStructure(cmd);	
		UARTResetCtrlStructure(packet);
		timing->cmdStop = SysTickValueGet();
		// display different input prompts based on mode
		// during scripting, show current stack position
		if (stack->saveCommands) {
			UARTprintf(STACK_PROMPT, stack->stackIdx);
		} else {
			UARTprintf(INPUT_PROMPT);
		}
	}	
}

//*****************************************************************************
// This function Calulates time spent on processing in microseconds
//
//*****************************************************************************
void CalculateUsage (timing_info_t *timing) {
	
	//
	// Calculate the number of us elapsed
	// but first check if cmdStart is > cmdStop
	// if not, systick has experienced an overflow
	// correct for this.
	//

	if (timing->cmdStart > timing->cmdStop) {
		timing->elapsedTime = timing->cmdStart - timing->cmdStop; 	
	}
	else {
		timing->elapsedTime = (( timing->cmdStart + 
				( SYSTICK_MAX - timing->cmdStop )));
	}

	// push current value to profile array
	// and update counter
	timing->elapsedTimeProfile[timing->eTindex] = timing->elapsedTime;
	timing->eTindex = timing->eTindex + 1; 
}

//*****************************************************************************
// This function initializes the control structures to their default values
//
//*****************************************************************************
void CMDLineProcessInit (void) {
	
	uart_info_t *packet = GetUartPointer();
	uart_cmd_t *cmd = GetCmdPointer();
	timing_info_t *timing = GetTimerPointer();
	stack_info_t *stack = GetStackPointer();

	UARTResetCtrlStructure(packet);
	CMDResetCtrlStructure(cmd);
	TimingResetCtrlStructure(timing);
	StackResetCtrlStructure(stack);

	packet->procBusy = false;
	packet->bytesTotal = 0;
	packet->displayStats = false;
	cmd->cmdTblPtr = cmdTable; 

	CMDLineSplashScreen ();
}


//*****************************************************************************
// This function will reset the timing_info_t structure
//
//*****************************************************************************
void TimingResetCtrlStructure (timing_info_t *timing) {

	uint8_t i = 0;

	timing->miliSeconds = 0;
	timing->seconds = 0;
	timing->minutes = 0;
	timing->hours = 0;
	timing->days = 0;
	timing->cmdStart = 0;
	timing->cmdStop = 0;
	timing->elapsedTime = 0;
	timing->eTindex = 0;

	for (i = 0; i < TIMING_PROFILE_DEPTH; i++) {
		timing->elapsedTimeProfile[i] = 0;
	}
}

//*****************************************************************************
// This function will reset the uart_info_t structure
//
//*****************************************************************************
void UARTResetCtrlStructure (uart_info_t *packet) {

	uint8_t i = 0;

	packet->procBusy = false;
	packet->enterFlag = false;
	packet->cmdFound = false;		
	packet->rxCount = 0;
	packet->cmdPos = 0;

	// clear buffer
	for (i = 0; i < BUFFER_SIZE; i++){
		packet->uartBuf[i] = 0;
	}
}

//*****************************************************************************
// This function will reset the uart_cmd_t structure
//
//*****************************************************************************
void CMDResetCtrlStructure (uart_cmd_t *cmd) {

	uint8_t i = 0;

	cmd->currentCmd = 0;	
	cmd->cmdIdx = 0;	
	cmd->commandFound = false;
	cmd->argsNum = 0;
	cmd->ExecComplete = true;
	
	// reset arguments
	for (i = 0; i < MAX_ARGS; i++) {
		cmd->currentArgs[i] = 0;
	}
}

//*****************************************************************************
// This function will reset the stack_info_t structure
//
//*****************************************************************************
void StackResetCtrlStructure (stack_info_t *stackPtr) {
	stackPtr->stackIdx = 0;
	stackPtr->startIdx = 0;
	stackPtr->stopIdx = 0;
	stackPtr->currIdx = 0;
	stackPtr->execFromStack = false;
	stackPtr->saveCommands = false;
	stackPtr->contExecution = false;
}

//*****************************************************************************
// This function will encode a string of arbitary size to int.
//
//*****************************************************************************
uint16_t EncodeCommand (char *command, uint8_t size) {
	const uint16_t inc = 2;
	uint16_t result = 0;
	uint8_t i = 0;

	for (i = 0; i <= size - 1 ; i++) {
		result += (uint16_t)*command * (i + inc);
		command++;		
	}
	return result;
}

//*****************************************************************************
// This function will do the same as the function above, but it will check
// if valid command delimiter is found, in case the incoming data is garbage.
//
//*****************************************************************************
uint16_t UARTEncodeCommand	(uart_info_t *packet){	
	if (packet->cmdFound) {
		return EncodeCommand(packet->uartBuf, packet->cmdPos); 
	}
	else {
		return 0;
	}
}

//*****************************************************************************
// This function will search the table pointed by pointer in the uart_cmt_t 
// structure and if it finds anything it will set the appropriate flag and 
// populate the cmdIdx within the structure.
//*****************************************************************************
bool FindCommand (uart_cmd_t *cmd) {

	uint8_t i = 0;
	uint16_t *cmdPtr = cmd->cmdTblPtr;
	bool ret = false;

	// check if currentCmd filed is populated
	if (cmd->currentCmd){
		// search supplied table for match
		while (!ret) {
			if (cmd->currentCmd == *cmdPtr) {
				ret = true;
				cmd->cmdIdx = i;
				cmd->commandFound = true;
			}
			else {
				cmdPtr++;
				i++;
				if (i > MAX_COMMANDS - 1) {
					return ret;
				}
			}
		}
	}
	return ret;
}

//*****************************************************************************
// This function will populate the argument table and argument count in the
// uart_cmd_t structure, if it finds anything.
//*****************************************************************************
uint8_t UARTGetArguments (uart_info_t *packet, uart_cmd_t *cmd) {
	uint8_t i = packet->cmdPos;	
	cmd->argsNum = 0;

	if (packet->cmdFound) {
		while(i != packet->rxCount) {
			if (packet->uartBuf[i] == KEY_DASH) {
				cmd->currentArgs[cmd->argsNum] = packet->uartBuf[i + 1];
				cmd->argsNum++;
				if (cmd->argsNum >= MAX_ARGS) {
					return cmd->argsNum;
				}
			}
			i++;
		}
	}
	return cmd->argsNum;
}

//*****************************************************************************
// Returns adress of control structure
//*****************************************************************************
uart_info_t *GetUartPointer(void) {
	return &uartData;
}

//*****************************************************************************
// Returns adress of control structure
//*****************************************************************************
uart_cmd_t *GetCmdPointer(void) {
	return &uartCmd;
}


//*****************************************************************************
// Returns adress of control structure
//*****************************************************************************
timing_info_t *GetTimerPointer(void) {
	return &timingData;
}

//*****************************************************************************
// Returns adress of control structure
//*****************************************************************************
stack_info_t *GetStackPointer(void) {
	return &stack;
}

//*****************************************************************************
// Returns pointer to uart stack structure
//*****************************************************************************
uart_cmd_t *GetUartStackPointer(void) {
	return uartCmdStack;
}

//*****************************************************************************
// Returns argument char from control structure
//*****************************************************************************
char GetArgument(uint8_t argNum) {
	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (argNum <= MAX_ARGS - 1) {
		return cmdPtr->currentArgs[argNum];
	}
	else {
		return 0;
	}
}

//*****************************************************************************
// Timekeeping interrupt service routine, called by NVIC
//*****************************************************************************
void SysTickIntHandler (void) {
	timing_info_t *timing;
	bool clearProcTiming = false;
	timing = GetTimerPointer();


	// handle time
	if (timing->miliSeconds >= 1000) {
		timing->miliSeconds = 0;
		timing->seconds++;
		clearProcTiming = true;
	}
	else {
		timing->miliSeconds += SYSTICK_TIME;
	}

	if (timing->seconds >= 59) {
		timing->seconds = 0;
		timing->minutes++;
	}

	if (timing->minutes >= 59) {
		timing->minutes = 0;
		timing->hours++;
	}

	if (timing->hours >= 23) {
		timing->hours = 0;
		timing->days++;
	}

	// clear process Scheduler Timing data
	if (clearProcTiming) {
		ProcClearTimer();
		clearProcTiming = false;
	}

	//
	// update heartbeat parameters
	// 
	if ((timing->seconds)%HEARTBEAT_INTERVAL) {
		switch (timing->miliSeconds) {

			case 200: GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0xFF);
			break;
			case 300: GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0x00); 
			break;
			case 500: GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0x00); 
			break;
			case 600: GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0xFF);
			break;	
			case 700: GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0x00);
			break;	
			default: 
			break;
		}
	}
}

//*****************************************************************************
//  Saves current command to next available location in stack
//*****************************************************************************
void SaveToStack (uart_cmd_t *cmd) {

	stack_info_t *stack = GetStackPointer();
	int arg = cmd->argsNum;
	bool save = stack->saveCommands;
	bool exec = stack->execFromStack;
	
	//
	// before saving check if stack execution is taking place
	// or save to stack option is disabled
	//
	if (save && !exec) {
		
		//
		// convert argument number to index number
		// because if argument number is
		// equal to 2, that means that they are 
		// stored at locations 0 and 1
		//
		arg--;

		// handle stack position overflow
		if (stack->stackIdx == COMMAND_STACK_DEPTH) {
			stack->stackIdx = 0;
		}
		// copy relevant information
		uartCmdStack[stack->stackIdx].currentCmd = cmd->currentCmd;
		uartCmdStack[stack->stackIdx].cmdIdx = cmd->cmdIdx;
		uartCmdStack[stack->stackIdx].cmdTblPtr = cmd->cmdTblPtr;
		uartCmdStack[stack->stackIdx].commandFound = cmd->commandFound;
		uartCmdStack[stack->stackIdx].argsNum = cmd->argsNum;

		// do not copy arguments if none are found
		if (cmd->argsNum) {
			while (arg >= 0) {
				uartCmdStack[stack->stackIdx].currentArgs[arg] = cmd->currentArgs[arg];
				arg--;
			}
		}
		stack->stackIdx++;
	}
}

//*****************************************************************************
// Copies info from stack to command line
//*****************************************************************************
void GetFromStack (uint8_t position) {
	uart_cmd_t *cmd = GetCmdPointer();
	uint8_t arg = uartCmdStack[position].argsNum;
	uint8_t i;

	//
	// convert argument number to index number
	// because if argument number is
	// equal to 2, that means that they are 
	// stored at locations 0 and 1
	// prevent underflow if number of arguments
	// is zero
	//
	//if (arg) {
	//	arg--;
	//}
	// copy relevant information
	cmd->currentCmd = uartCmdStack[position].currentCmd;
	cmd->cmdIdx = uartCmdStack[position].cmdIdx;
	cmd->commandFound = uartCmdStack[position].commandFound;
	cmd->argsNum = uartCmdStack[position].argsNum;
	
	// THIS NEEDS TO BE FIXED, because sometimes
	// when a command with 3 args is followed by a command
	// with two args, this shit gets confused.
	// try: for(i = 0; i < arg; i++)
	//
	// This is probably fixed in CMDResetCtrlStructure()
	// all arguments are reset after call to function 
	// has been completed.
	if (arg) {
		for (i = 0; i <= arg; i++) {
			cmd->currentArgs[i] = uartCmdStack[position].currentArgs[i];
		}
	}
}

//*****************************************************************************
// Executes commands within a adress range
//*****************************************************************************
void ExecuteFromStack(void) {
	uart_cmd_t *cmd = GetCmdPointer();
	stack_info_t *stack = GetStackPointer();
	timing_info_t *timing = GetTimerPointer();
	bool loop = stack->contExecution;
	int j = stack->stopIdx;

	// 
	// assign code entry point in stack
	// NB!: entry point is always valid
	// handled by cmdqueue commands
	//
	stack->currIdx = stack->startIdx;
		//
		// Execute code from stack, if user cancels execution
		// finish executing current function, before returning
		// cancel is handled from uart interrupt service routine
		//
		while (stack->currIdx != j && stack->execFromStack) {
			// calculate usage if running a script
			CalculateUsage(timing);
			timing->cmdStart = SysTickValueGet();
			// check if current stack i properly populated, skip overwise
			if (uartCmdStack[stack->currIdx].commandFound) {
				// copy command from stack to cmd
				GetFromStack(stack->currIdx);		
				ExecuteCommand(cmd);	
				stack->currIdx++;
			}
			else {
				// increment stack counter if command is not valid
				stack->currIdx++;
			}
			// loop forever if requested
			if (loop && stack->currIdx >= j) {
					stack->currIdx = stack->startIdx;
			}
			// Get stop value for timing
			timing->cmdStop = SysTickValueGet();
		}
	// reset stack control strucutre after
	// execution has been completed or 
	// canceled
	StackResetCtrlStructure(stack);
}

//*****************************************************************************
// Enables or disables execution from stack
//*****************************************************************************
bool StackExecution(bool enable) {

	stack_info_t *stackPtr = GetStackPointer();
	stackPtr->execFromStack = enable;
	return enable;
}

//*****************************************************************************
// Enables or disables the use of stack
//*****************************************************************************
void SaveCommands(bool enable) {
	stack_info_t *stackPtr = GetStackPointer();
	stackPtr->saveCommands = enable;
}

//*****************************************************************************
// UART interrupt service routine, called by NVIC
//*****************************************************************************
void UartIntHandler (void) {	
	uint8_t i;
	char c;	
	stack_info_t *stack = GetStackPointer();
	uart_cmd_t *cmd = GetCmdPointer();
	script_settings_t *s = GetScriptSettingsPointer();

	// 
	// get current interrupt status and 
	// clear it
	//
	uartData.lastIntStatus = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, uartData.lastIntStatus);
	
	// get current char for processing
	c = (char)UARTCharGet(UART0_BASE);

	// Script processing
	if (s->scriptModeOn) {
		ScriptCallBackHandler(c);	
	} else {
		// check if previous processing is done yet
		if (!uartData.procBusy) {
			if (c == KEY_ENTER) {
				// acknolowedge to user that packet is recieved
				// I don't always use printf, but when i do, i do it
				// from the interrupt handler :)
				if (uartData.displayStats) {
					UARTprintf("\nBytes recieved: %d\n", uartData.rxCount);
					UARTprintf("Bytes sofar: %d\n", uartData.bytesTotal);
					UARTprintf("Command: ");
					for (i = 0; i <= uartData.rxCount; i++) {
						UARTprintf("%x", uartData.uartBuf[i]);
					}
					UARTprintf("\nCmd ends at position %d\n", uartData.cmdPos);
				}
				// Tell string processing routine that packet is ready
				// for processing
				uartData.enterFlag = true;
				uartData.procBusy = true;
			}
			else {
				// save char and send it back to terminal
				// TODO: do not print non-printable characters or 
				// control characters
				uartData.currentChar = c;
				UARTprintf("%c", c);
				
				// space usually delimits cmd on first occurence
				if (c == KEY_SPACE && !uartData.cmdFound) { 
					uartData.cmdPos = uartData.rxCount;
					uartData.cmdFound = true;
				}
				// handle overflow
				if (uartData.rxCount >= BUFFER_SIZE - 1) {
					uartData.rxCount = 0;
					UARTprintf(CMD_OVERFLOW);
				}
				else {
					// handle backspace input to 
					// delete things from buffer
					if (c == KEY_BACKSPACE) {
						if (uartData.rxCount <= 0) {
							uartData.rxCount = 0;
						}
						else {
							uartData.rxCount--;
						}
						if (uartData.uartBuf[uartData.rxCount] == KEY_SPACE) {
							uartData.cmdFound = false;
						}
					}
					else {
						uartData.uartBuf[uartData.rxCount] = c;	
						uartData.rxCount++;
					}
				}
			}
		}
		else {
			UARTprintf(CMD_PROC_BUSY);
			if (c == KEY_BACKSPACE && stack->execFromStack) {
				// cancel current stack execution 
				stack->contExecution = false;
				stack->execFromStack = false;
			} 

		}	
		// toggle led to indicate byte activity
		if (uartData.ledFlag) {
			uartData.ledFlag = false;
			//GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0xFF);
		}
		else {
			uartData.ledFlag = true;
			//GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0x00);
		}

		// increment stat counter
		uartData.bytesTotal++;
	}
}

//*****************************************************************************
// Executes command
//*****************************************************************************
bool ExecuteCommand (uart_cmd_t *cmd){

	// check if current command code
	// is valid
	if (!cmd->currentCmd) {
		return false;
	}
	else {
		cmd->ExecComplete = false;
	}

	// Execute Command
	cmdFuncTable[cmd->cmdIdx]();

	// Update flag
	cmd->ExecComplete = true;

	return cmd->ExecComplete;
}


//*****************************************************************************
// Registers a command code in a table if correct position is specified
//*****************************************************************************
bool RegisterCommandCode (uint8_t pos, uint16_t cmdCode) {
	uart_cmd_t *cmd = GetCmdPointer();
	uint16_t *cmdPtr = cmd->cmdTblPtr;
	bool ret = false;

	// Sanity check for correct pointer
	if (!cmdPtr) {
		UARTprintf(CMD_PTR_ERROR);
		return false;
	}

	// advance pointer to entry
	cmdPtr += pos;

	// check bounds, set entry if valid
	if (pos < MAX_COMMANDS - 1) {
		*cmdPtr = cmdCode;
		ret = true;
	}
	else {
		UARTprintf(CMD_INV_IDX, pos);
	}
	return ret;
}

//*****************************************************************************
// Register a handler within a table if correct position is specified
//*****************************************************************************
bool RegisterCommandFunc (uint8_t pos, void (*handler)(void)) {
	bool ret = false;

	if (pos < MAX_COMMANDS - 1) {
		cmdFuncTable[pos] = handler;
		ret = true;
	}
	else {
		UARTprintf(CMD_INV_IDX, pos);
	}
	return ret;
}

//*****************************************************************************
// Evaluates command just if it was entered via uart terminal.
// This command fills the appropriate fields of the uart structure,
// causing the program to think that the data was recieved via uart Terminal
// Example:
// EvaluateStringCommand("time -d -1\n", 11);
// Will cause a 1 second delay to be executed
// 
// In order to execute several commands in series,
// the CMDLineScheduler () command must be executed
// each time EvaluateStringCommand () is executed.
//
//
// This command can be used to execute stream of chars that is coming 
// from other source than uart, it can be from an udp stream or can or 
// whatever.
//
//*****************************************************************************
bool EvaluateStringCommand (char *command) {
	uart_info_t *packet = GetUartPointer();
	char *c = command;
	uint8_t i = 0;
	bool ret = false;

	// prepare structure parameters
	packet->enterFlag = false;
	packet->procBusy = false;
	packet->cmdFound = false;

	// identify command location, command is denoted by space
	// only on first occurence, also count the number of chars
	// so we know if there are any arguments after command.
	while (!ret) {
		if (*c == KEY_SPACE && packet->cmdFound == false) {
			packet->cmdPos = i;
			packet->cmdFound = true;
		}
		else {
			// handle limit
			if (i >= BUFFER_SIZE && !ret) {
				return false;
			}
		}
		// handle character size
		if (*c == '\n') {
			packet->rxCount = i;
			ret = true;
		}
		c++;
		i++;
	}
	// 
	// copy string to input buffer
	// and force evaluation of the
	// command
	//
	if (packet->cmdFound) {
		c = command;
		for (i = 0; i <= packet->rxCount - 1; i++) {
			packet->uartBuf[i] = *c;
			c++;
		}
		// force packet evaluation
		packet->enterFlag = true;
	}
	
	// remember to clear buffer here to avoid 
	// unknown chars in buffer

	return packet->cmdFound;
}

//*****************************************************************************
// Evaluates multiple string commands
// char *cmd[] should contain strings to be executed
// uint8_t cmdNum should contain the number of commands to execute
//*****************************************************************************
bool EvaluateMultipleCommands (char *cmd[], uint8_t cmdNum) {

	uint8_t index = 0;

	bool ret = false;

	if (cmd == 0) {
		UARTprintf(CMD_PTR_ERROR);
		return ret;
	}

	for (index = 0; index <= cmdNum - 1; index++) {
		ret = EvaluateStringCommand(cmd[index]);
		CMDLineScheduler ();
	}

	UARTprintf(CMD_MULT_FINISH, index);
	UARTprintf(INPUT_PROMPT);
	return ret;
}


//*****************************************************************************
// Initilizes hardware
//*****************************************************************************
void InitCortexHardware(void) {
	//
	// Enable FPU
	//
	FPUEnable();
	FPULazyStackingEnable();

	// Set clocking to 50 MHz, due to REV_A1 being a total ass.
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//
	// Configure UART0 for 115200-8n1
	// 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
  	UARTStdioConfig(0, 115200, 16000000);
	UARTFIFODisable(UART0_BASE);
	UARTIntEnable(UART0_BASE, UART_INT_RX);
	UARTprintf("\n\nSystem initializing.\n");

	//
	// Enable SysTick for periodic Interrupts
	// systick is used by command line process
	//
	SysTickEnable();
	SysTickPeriodSet(SysCtlClockGet()/(SYSTICK_TIME/10));
	SysTickIntEnable();
	UARTprintf("- Systick timer enabled.\n");

	//
	// Configure Heartbeat led
	//
	SysCtlPeripheralEnable(HEARTBEAT_CTRL_PORT);
	GPIOPinTypeGPIOOutput(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN);
	GPIOPinWrite(HEARTBEAT_BASE_PORT, HEARTBEAT_PIN, 0xFF);
	UARTprintf("- GPIO enabled. \n");


    //
    // Enable UART interrupts
    //
    IntMasterEnable();
    IntEnable(INT_UART0);
    UARTprintf("- Interrupts enabled.\n");
}

//*****************************************************************************
// Displays standard splash screen
//*****************************************************************************
void CMDLineSplashScreen (void) {
	UARTprintf("\n\nWelcome to potatobox, a simple command line interpreter\n");
	UARTprintf("Device: ");
	#ifdef PART_TM4C1233H6PM
		UARTprintf("TM4C1233H6PM\n");
	#elif PART_TM4C1230C3PM
		UARTprintf("TM4C1230C3PM\n");
	#elif PART_TM4C1230D5PM
		UARTprintf("TM4C1230D5PM\n");
	#elif PART_TM4C1230E6PM
		UARTprintf("TM4C1230E6PM\n");
	#elif PART_TM4C1230H6PM
		UARTprintf("TM4C1230H6PM\n");
	#elif PART_TM4C1231C3PM
		UARTprintf("TM4C1231C3PM\n");
	#elif PART_TM4C1231D5PM
		UARTprintf("TM4C1231D5PM\n");
	#elif PART_TM4C1231D5PZ  
		UARTprintf("TM4C1231D5PZ\n");
	#elif PART_TM4C1231E6PM
		UARTprintf("TM4C1231E6PM\n");
	#elif PART_TM4C1231E6PZ
		UARTprintf("TM4C1231E6PZ\n");
	#elif PART_TM4C1231H6PM
		UARTprintf("TM4C1231H6PM\n");
	#elif PART_TM4C1231H6PZ  
		UARTprintf("TM4C1231H6PZ\n");
	#elif PART_TM4C1232C3PM
		UARTprintf("TM4C1232C3PM\n");
	#elif PART_TM4C1232D5PM  
		UARTprintf("TM4C1232D5PM\n");
	#elif PART_TM4C1232E6PM
		UARTprintf("TM4C1232E6PM\n");
	#elif PART_TM4C1232H6PM  
		UARTprintf("TM4C1232H6PM\n");
	#elif PART_TM4C1233C3PM
		UARTprintf("TM4C1233C3PM\n");
	#elif PART_TM4C1233D5PM  
		UARTprintf("TM4C1233D5PM\n");
	#elif PART_TM4C1233D5PZ
		UARTprintf("TM4C1233D5PZ\n");
	#elif PART_TM4C1233E6PM  
		UARTprintf("TM4C1233E6PM\n");
	#elif PART_TM4C1233E6PZ
		UARTprintf("TM4C1233E6PZ\n");
	#elif PART_TM4C1233H6PM  
		UARTprintf("TM4C1233H6PM\n");
	#elif PART_TM4C1233H6PZ
		UARTprintf("TM4C1233H6PZ\n");
	#elif PART_TM4C1236D5PM  
		UARTprintf("TM4C1236D5PM\n");
	#elif PART_TM4C1236E6PM
		UARTprintf("TM4C1236E6PM\n");
	#elif PART_TM4C1236H6PM  
		UARTprintf("TM4C1236H6PM\n");
	#elif PART_TM4C1237D5PM
		UARTprintf("TM4C1237D5PM\n");
	#elif PART_TM4C1237D5PZ  
		UARTprintf("TM4C1237D5PZ\n");
	#elif PART_TM4C1237E6PM
		UARTprintf("TM4C1237E6PM\n");
	#elif PART_TM4C1237E6PZ  
		UARTprintf("TM4C1237E6PZ\n");
	#elif PART_TM4C1237H6PM
		UARTprintf("TM4C1237H6PM\n");
	#elif PART_TM4C1237H6PZ  
		UARTprintf("TM4C1237H6PZ\n");
	#elif PART_TM4C123AE6PM
		UARTprintf("TM4C123AE6PM\n");
	#elif PART_TM4C123AH6PM  
		UARTprintf("TM4C123AH6PM\n");
	#elif PART_TM4C123BE6PM
		UARTprintf("TM4C123BE6PM\n");
	#elif PART_TM4C123BE6PZ 
		UARTprintf("TM4C123BE6PZ\n"); 
	#elif PART_TM4C123BH6PM
		UARTprintf("TM4C123BH6PM\n");
	#elif PART_TM4C123BH6PZ  
		UARTprintf("TM4C123BH6PZ\n");
	#elif PART_TM4C123FE6PM
		UARTprintf("TM4C123FE6PM\n");
	#elif PART_TM4C123FH6PM  
		UARTprintf("TM4C123FH6PM\n");
	#elif PART_TM4C123GE6PM
		UARTprintf("TM4C123GE6PM\n");
	#elif PART_TM4C123GE6PZ  
		UARTprintf("TM4C123GE6PZ\n");
	#elif PART_TM4C123GH6PM
		UARTprintf("TM4C123GH6PM\n");
	#elif PART_TM4C123GH6PZ  
		UARTprintf("TM4C123GH6PZ\n");
	#elif PART_TM4C1231H6PGE
		UARTprintf("TM4C1231H6PGE\n");
	#elif PART_TM4C1233H6PGE  
		UARTprintf("TM4C1233H6PGE\n");
	#elif PART_TM4C1237H6PGE
		UARTprintf("TM4C1237H6PGE\n");
	#elif PART_TM4C123BH6PGE  
		UARTprintf("TM4C123BH6PGE\n");
	#elif PART_TM4C123BH6ZRB
		UARTprintf("TM4C123BH6ZRB\n");
	#elif PART_TM4C123GH6PGE  
		UARTprintf("TM4C123GH6PGE\n");
	#elif PART_TM4C123GH6ZRB
		UARTprintf("TM4C123GH6ZRB\n");
	#else
		UARTprintf("Generic CortexM-4\n");
	#endif
	UARTprintf("Type: help -h to get started.\n");
	UARTprintf("antoker. www.antoker.com\n\n");
}
