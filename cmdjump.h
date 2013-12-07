#ifndef __CMDJUMP_H__
#define __CMDJUMP_H__

#define CMD_JUMP_HEADER		"\nAvailable commands:\n"
#define CMD_JUMP_LINE_1		"jump -h       -> Displays this help.\n"
#define CMD_JUMP_LINE_2		"jump -v -0 -1 -> Set branching value.\n"
#define CMD_JUMP_LINE_3		"jump -i -a -b -> Set branching index.\n"
#define CMD_JUMP_LINE_4     "jump -l       -> Branches to index if branching\n                 value is larger than pool value.\n"
#define CMD_JUMP_LINE_5		"jump -s       -> Branches to index if branching\n                 value is smaller than pool value.\n"
#define CMD_JUMP_LINE_6		"jump -e       -> Branches to index if branching\n                 values is equal to pool value.\n"
#define CMD_JUMP_LINE_7		"jump -n       -> Branches to index if branching\n                 value is not equal to pool value.\n"
#define CMD_JUMP_LINE_8		"jump -d       -> Get current jump assigments.\n"
#define CMD_JUMP_LINE_9 	"jump -a -2 -3 -> Unconditional jump to 0x23.\n"
#define CMD_JUMP_FOOTER		"eof"

#define CMD_INVALID_INDEX	"\nInvalid jump index, requested: %d, max: %d"
#define CMD_NO_EXEC_SCRIPT	"\nAssigment skipped, not in scripting mode."
#define CMD_JUMP_RECORDED	"\nJump assigment recorded."
#define CMD_JUMP_SKIPPED	"\nBranching skipped, not in execution mode."
#define BRANCHING_TO		"\nBranching conditions met, jumping to: 0x%x."
#define NO_BRANCHING		"\nBranching conditions not met, skipping."
#define BRANCHING_VALUE		"\nBranching value recorded: 0x%x."
#define BRANCH_INDEX		"\nBranching index set: 0x%x."
#define INVALID_ARGUMENTS	"\nInvalid arguments: -%c -%c."
#define INVALID_ARG_NUM		"\nInvalid number of arguments."
#define BRANCHING_INFO		"\nCurrent compare value: 0x%x, jump index: 0x%x."

#define TYPE_EQUAL 0
#define TYPE_NOT_EQUAL 1
#define TYPE_LARGER 2
#define TYPE_SMALLER 3
#define MAX_BRANCHING_VALUE 0xFF

void SetBranchValue(void);
void SetBranchIndex(void);
void CompareAndBranch(uint8_t type);
void PrintBranchingInfo(void);


void CmdJump(void);
void CmdJumpHelp(void);

#endif
