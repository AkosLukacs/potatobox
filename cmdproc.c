#include <stdint.h>
#include <stdlib.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdproc.h"

pqueue_info_t processQueue;
proc_info_t procTable[MAX_PROCESSES] = {0};

//*****************************************************************************
// initialization routine
//*****************************************************************************
void ProcSchedulerInit (void) {

	uint8_t i;
	pqueue_info_t *pqueue = GetProcessQueuePointer ();
	
	// initialize procTable
	for (i = 0; i <= MAX_PROCESSES -1; i++) {
		procTable[i].procId = 0;
		procTable[i].procPriority = 3;			// set lowest priority
		procTable[i].callsSec = 0;
		procTable[i].called = 0;
		procTable[i].procSuspend = true;		// initially suspend
		procTable[i].procRegistered = false;	// process is not registered
	}

	// initialize process queue
	pqueue->nextFreeSlot = 0;
	pqueue->execElem = 0;
	pqueue->procBusy = false;
	pqueue->policy = POLICY_SEQ;
}

//*****************************************************************************
// clears timing data and updates process calls per second
// this routine is called from systick interrupt context
//*****************************************************************************
void ProcClearTimer (void) {
	uint8_t i;

	for (i = 0; i <= MAX_PROCESSES -1; i++) {
		procTable[i].callsSec = procTable[i].called;
		procTable[i].called = 0;
	}
}

//*****************************************************************************
// Executes a suspended process once
//*****************************************************************************
void ProcSingleExecute (uint8_t procId) {	

	pqueue_info_t *pqueue = GetProcessQueuePointer ();
	UARTprintf(PROC_SINGLE_EXEC, procId);

	// Check if process id is out of bounds
	if (procId > MAX_PROCESSES -1) {
		UARTprintf(PROC_ID_FAIL, procId);
	} else {
		// check if element can be executed, skip otherwise
		if (procTable[procId].procRegistered) {
			
			// notify that process is being executed
			pqueue->procBusy = true;
			// execute
			procTable[procId].ProcFunctionPtr();
			// update counter for this particular process
			procTable[procId].called++;
			// notify that process has stopped
			pqueue->procBusy = false;
		}
		else {
			UARTprintf(PROC_SINGLE_EXFAIL);
		}			
	}

}

//*****************************************************************************
// schedules and executes processes, currently sequential/random execution only
//*****************************************************************************
void ProcScheduler (void) {
	pqueue_info_t *pqueue = GetProcessQueuePointer();
	uint8_t i; 

	// selects which item will be executed according to policy
	i = ProcSchedulerModify (pqueue->policy, pqueue->execElem);

	// check if element is out of bounds
	// happens when the number of registered processes
	// is less than size of the array.
	if (i >= pqueue->nextFreeSlot) {
		i = 0;
	}

	// check if element can be executed, skip otherwise
	if (procTable[i].procRegistered && !procTable[i].procSuspend) {
		// notify that process is being executed
		pqueue->procBusy = true;
		// execute
		procTable[i].ProcFunctionPtr();
		// update statistics counter
		// for this particular process
		procTable[i].called++;
		// notify that process has stopped
		pqueue->procBusy = false;
	}

	// increment to next position in process table
	i++;

	// check for bounds
	if (i > MAX_PROCESSES - 1) {
		i = 0;
	}

	// update information
	pqueue->execElem = i;
}

//*****************************************************************************
// Modify execution mode  according to policy
//*****************************************************************************
uint8_t ProcSchedulerModify (uint8_t policy, uint8_t element) {

	uint8_t execIndexStop;
	uint8_t execElement = element; 

	if (policy == POLICY_RANDOM) {
		// assign upper boundary for randomizing.
		// if process queue is filled, just use 
		// size of queue, otherwise use last registered
		// element
		if (!execIndexStop) {
			execIndexStop = MAX_PROCESSES - 1;
		}
		else {
			execIndexStop -= 1;
		}

		// calculate random index and return
		execElement =  rand() % execIndexStop + 1;
	}

	if (policy == POLICY_PRIORITY) {
		// not yet implemented
	}

	return execElement;
}

//*****************************************************************************
// Get Pid of currently running process, can be used by external processes.
//*****************************************************************************
uint8_t ProcGetPid (void) {
	pqueue_info_t *pqueue = GetProcessQueuePointer();
	return procTable[pqueue->execElem].procId;
}

//*****************************************************************************
// prints specific information about a process
//*****************************************************************************
void ProcPrintInfo (uint8_t procId) {

	if (procId > MAX_PROCESSES -1) {
		UARTprintf(PROC_ID_FAIL, procId);
	}
	else {	
		UARTprintf(PROC_SPEC_LINE_1, procTable[procId].procId);
		UARTprintf(PROC_SPEC_LINE_2, procTable[procId].procPriority);
		UARTprintf(PROC_SPEC_LINE_3, procTable[procId].callsSec);
		UARTprintf(PROC_SPEC_LINE_4, procTable[procId].procName);
		UARTprintf(PROC_SPEC_LINE_5, procTable[procId].ProcFunctionPtr);
		UARTprintf(PROC_SPEC_LINE_6, procTable[procId].procSuspend
				? PROC_SUSPEND : PROC_RUN);
		UARTprintf(PROC_SPEC_LINE_7, procTable[procId].procRegistered
				? "true" : "false");
	}
}

//*****************************************************************************
// prints generic information about all registered processes
//*****************************************************************************
void ProcPrintGenericInfo (void) {
	uint8_t i = 0;

	for (i = 0; i <= MAX_PROCESSES -1; i++) {
	if (procTable[i].procRegistered) {
			UARTprintf(PROC_GENERIC_INFO, procTable[i].procId,
					procTable[i].procName,
					procTable[i].procSuspend ? PROC_SUSPEND : PROC_RUN,
					procTable[i].callsSec);
		}
	}
	UARTprintf("\n");
}

//*****************************************************************************
// suspends or allows execution of existing process
//*****************************************************************************
void ProcSuspend (uint8_t procId, bool suspend) {
	bool reg;

	// check requested number, dont do anything if out of bounds
	if (procId > MAX_PROCESSES -1) {
		UARTprintf(PROC_ID_FAIL, procId);
	}
	else {
		// get process registration
		reg = procTable[procId].procRegistered;
		// check if process is registered
		if (reg) {
			procTable[procId].procSuspend = suspend;
			// tell user about process state
			UARTprintf(PROC_SUSPENDED_OK, procId, suspend ?
					PROC_SUSPEND : PROC_RUN);
		}
	}
}

//*****************************************************************************
// Returns pointer to processQueue
//*****************************************************************************
pqueue_info_t *GetProcessQueuePointer (void) {
	return &processQueue;
}

//*****************************************************************************
// Registers a new process
//*****************************************************************************
bool ProcRegister(char *name, void (*f)(void), uint8_t priority) {
	pqueue_info_t *pqueue = GetProcessQueuePointer();
	bool ret = false;
	uint8_t i;
	uint8_t slot = pqueue->nextFreeSlot;

	// check if there is a free slot available
	if (slot > MAX_PROCESSES - 1) {
		UARTprintf(PROC_REG_FAIL);
		return ret;
	}
	else {
		// populate process information
		ret = true;
		procTable[slot].procId = slot;
		procTable[slot].procPriority = priority;
		procTable[slot].callsSec = 0;
		procTable[slot].ProcFunctionPtr = f;
		// initially suspend process
		procTable[slot].procSuspend = true;	
		procTable[slot].procRegistered = true;

		// copy name
		for (i = 0; i <= PROC_NAME_LEN - 1; i++) {
			procTable[slot].procName[i] = *name;
			name++;
		}

		// tell user
		UARTprintf(PROC_REG_INFO, procTable[slot].procId);
	}
	// update next free slot
	pqueue->nextFreeSlot++;
	return ret;
}

//*****************************************************************************
// executes commands from command line interface
//*****************************************************************************
void CmdProc (void) {
	uart_cmd_t *cmdPtr = GetCmdPointer();
	uint8_t argsNum = cmdPtr->argsNum;
	char arg = cmdPtr->currentArgs[ARG_1];
	char id = cmdPtr->currentArgs[ARG_2] - KEY_ZERO;

	if (argsNum) {		
		switch (arg) {
			case 'h':
					CmdProcHelp ();
					break;
			case 't':
					ProcSuspend (id, true);
					break;
			case 's':
					ProcSuspend (id, false);
					break;
			case 'i':
					ProcPrintInfo (id);
					break;
			case 'r':
					break;
			case 'q':
					break;
			case 'o':
					break;
			case 'e':
					ProcSingleExecute(id);
					break;
		}
	}
	else {
		ProcPrintGenericInfo ();
	}
}

//*****************************************************************************
// prints help 
//*****************************************************************************
void CmdProcHelp (void) {
	UARTprintf(CMD_PROC_HEADER);
	UARTprintf(CMD_PROC_LINE_1);
	UARTprintf(CMD_PROC_LINE_2);
	UARTprintf(CMD_PROC_LINE_3);
	UARTprintf(CMD_PROC_LINE_4);
	UARTprintf(CMD_PROC_LINE_5);
	UARTprintf(CMD_PROC_LINE_6);
	UARTprintf(CMD_PROC_LINE_7);
	UARTprintf(CMD_PROC_LINE_8);
	UARTprintf(CMD_PROC_LINE_9);
	UARTprintf(CMD_PROC_FOOTER);
}
