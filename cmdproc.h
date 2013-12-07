#ifndef __CMDPROC_H__
#define __CMDPROC_H__

#include <stdbool.h>
#include <stdint.h>

#define CMD_PROC_HEADER		"\nAvailable commands:\n"
#define CMD_PROC_LINE_1		"proc          -> Show list of registered processes.\n"
#define CMD_PROC_LINE_2		"proc -h       -> Displays this help.\n"
#define CMD_PROC_LINE_3		"proc -t -0    -> Suspends process 0.\n"
#define CMD_PROC_LINE_4		"proc -s -1    -> Starts process 1.\n"
#define CMD_PROC_LINE_5		"proc -i -0    -> Show detailed process info.\n"
#define CMD_PROC_LINE_6		"proc -r       -> Set scheduler policy to random.\n"
#define CMD_PROC_LINE_7 	"proc -q       -> Set scheduler policy to sequential.\n"
#define CMD_PROC_LINE_8		"proc -o       -> Set scheduler policy to priority.\n"
#define CMD_PROC_LINE_9		"proc -e -5    -> Execute process 5 once, then suspends it.\n"
#define CMD_PROC_FOOTER		"eof"

#define MAX_PRIORITY	3
#define MAX_PROCESSES	15
#define PROC_NAME_LEN	30

#define POLICY_RANDOM	0
#define POLICY_SEQ		1
#define POLICY_PRIORITY	2


#define PROC_SPEC_LINE_1	"\nPID: %d information.\n"
#define PROC_SPEC_LINE_2	"Priority: %d.\n"
#define PROC_SPEC_LINE_3	"Calls/sec: %d \n"
#define PROC_SPEC_LINE_4	"Name: %s\n"
#define PROC_SPEC_LINE_5	"Function: 0x%00x.\n"
#define PROC_SPEC_LINE_6	"Suspended: %s\n"
#define PROC_SPEC_LINE_7	"Registered: %s\n"

#define PROC_GENERIC_INFO	"\nPID: %d, Name: %s, Suspended: %s, Calls/sec: %d."
#define PROC_REG_INFO		"\nProcess registered with pid: %d."
#define PROC_REG_FAIL		"\nUnable to register process."
#define PROC_ID_FAIL		"\nUnable to find requested process: %d."
#define PROC_SUSPENDED_OK	"\nProcess with PID: %d Suspended: %s."
#define PROC_SINGLE_EXEC	"\nExecuting process with PID: %d."
#define PROC_SINGLE_EXFAIL	"\nCould not execute process!"
#define PROC_SUSPEND		"true"
#define PROC_RUN			"false"

//*****************************************************************************
// procId - unique process identifier
// procPriority - process priority, used to schedule process execution
// callsSec - total number of calls per second
// called  - incremented each time process is called
// procName - brief process description
// ProcFunctionPtr - pointer to function
// procSuspend - set to true if process is suspended, this usually applies
//		to processes that should not be executed or should be executed only
//		once.
// procRegistered - tells if process has been registered
//*****************************************************************************
typedef struct {
	uint8_t	procId;
	uint8_t procPriority;
	int callsSec;
	int called;

	char procName[PROC_NAME_LEN];
	void (*ProcFunctionPtr)(void);

	bool procSuspend;
	bool procRegistered;
} proc_info_t;

//*****************************************************************************
// nextFreeSlot - contains next free index in execution table
// execElem - contains index of currently running process
// policy - scheduler policy
// procBusy - process is being executed
//*****************************************************************************
typedef struct {
	uint8_t nextFreeSlot;
	uint8_t execElem;
	uint8_t policy;

	bool procBusy;

} pqueue_info_t;

//*****************************************************************************
// Process Scheduler Functions
//*****************************************************************************
void ProcSchedulerInit (void);
void ProcScheduler (void);
void ProcPrintInfo (uint8_t procId);
void ProcPrintGenericInfo (void);
void ProcClearTimer (void);

pqueue_info_t *GetProcessQueuePointer (void);

bool ProcRegister(char *name, void (*f)(void), uint8_t priority);

void ProcSuspend (uint8_t procId, bool suspend);
void ProcSingleExecute (uint8_t procId);
uint8_t ProcSchedulerModify (uint8_t policy, uint8_t element);
uint8_t ProcGetPid (void);

void CmdProc (void);
void CmdProcHelp (void);


#endif
