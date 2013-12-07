#ifndef __CMDQUEUE_H__
#define __CMDQUEUE_H__

#include <stdbool.h>

#define CMD_QUEUE_HEADER		"\nAvailable commands:\n"
#define CMD_QUEUE_LINE_1		"queue -h       -> Displays this help.\n"
#define CMD_QUEUE_LINE_2		"queue -e       -> Starts execution from adress 0x0.\n"
#define CMD_QUEUE_LINE_3		"queue -d       -> Disables save to stack feature.\n"
#define CMD_QUEUE_LINE_4		"queue -s       -> Enables save to stack feature.\n"
#define CMD_QUEUE_LINE_5		"queue -r       -> Resets stack pointer.\n"
#define CMD_QUEUE_LINE_6		"queue -e -1    -> Starts execution from adress 0x1.\n"
#define CMD_QUEUE_LINE_7		"queue -e -0 -2 -> Executes code from 0x0 - 0x02.   \n"
#define CMD_QUEUE_LINE_8		"queue -f       -> Continious execution (Hit Backspace to interrupt).\n"
#define CMD_QUEUE_LINE_9		"queue -z       -> Deletes stack contents.\n"
#define CMD_QUEUE_LINE_10 		"queue -b       -> Cancels an ongoing stack execution.\n"
#define CMD_QUEUE_FOOTER		"eof"

#define START_ADDR_ERROR		"\nExecution start adress to high! Requested: 0d%d."
#define STOP_ADDR_ERROR			"\nExecution stop adress to high! Requested: 0d%d."
#define EXECUTION_START			"\nExecuting code from: 0d%d."
#define EXECUTION_START_STOP	"\nExecuting code from: 0d%d - 0d%d."
#define EXECUTION_BREAK			"\nStopping execution at line 0x%x"
#define EXECUTION_BREAK_INF		"\nStatement recorded."

#define STACK_SAVE_ON			"\nSave to stack option enabled."
#define STACK_SAVE_OFF			"\nSave to stack option disabled."
#define STACK_RESET				"\nStack pointer reset."
#define STACK_DELETE			"\nStack table erased. %d elements removed."

void CmdQueue (void);
void CmdQueueHelp (void);

void QueueExecFromStack(uint8_t startAddr, uint8_t stopAddr, bool loop);
void QueueCancelExecFromStack (void);
void QueueDeleteStack (void);


#endif
