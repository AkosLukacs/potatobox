#ifndef __CMDTIME_H__
#define __CMDTIME_H__
#include <stdint.h>

#define CMD_TIME_HELP_HEADER		"\nAvailable commands:\n"
#define CMD_TIME_HELP_1				"time -h       -> Displays this help.\n"
#define CMD_TIME_HELP_2				"time          -> shows timing statistics\n"
#define CMD_TIME_HELP_3				"                 including, time spent.\n"
#define CMD_TIME_HELP_4				"time -r       -> Reset perfomance counters\n"
#define CMD_TIME_HELP_5				"time -d -2    -> Causes a 2 second delay.\n"
#define CMD_TIME_HELP_6				"time -m -1    -> Causes a 100 ms delay.\n"
#define CMD_TIME_HELP_7				"time -p       -> Show detailed performance profile.\n"
#define CMD_TIME_HELP_8				"time -a       -> Prints profile data.\n"
#define CMD_TIME_HELP_FOOTER		"eof\n"


#define CMD_TIME_REPORT_1			"\nTiming report:\n"
#define CMD_TIME_REPORT_2			"Current time: %dd %02d hours %02d minutes %02d:%04d\n"
#define CMD_TIME_REPORT_S 			"Command usage time: %d.%03d sec\n"
#define CMD_TIME_REPORT_US			"Command usage time: %d us."
#define CMD_TIME_REPORT_MS			"Command usage time: %d ms."
#define CMD_TIME_REPORT_TICKS		"Command usage time: %d ticks."
#define CMD_TIME_REPORT_4			"\nTiming report end\n"


#define CMD_TIME_RESET				"\nPerfomance counters are reset.\n"
#define CMD_TIME_DELAY_SEC			"\nDelay Finished: %d seconds."
#define CMD_TIME_DELAY_MSEC			"\nDelay Finished: %d miliseconds."
#define CMD_DELAY_INIT				"\nInitializing delay."
#define CMD_DELAY_ERROR				"\nRequested delay rejected: %d sec."
#define CMD_TIME_GENERIC_ERROR		"\nInvalid syntax. Use -h for help."

#define CMD_TIME_PROFILE_1			"\nDetailed performace profile:"
#define CMD_TIME_PROFILE_2			"\nTicks Maximum: %d"
#define CMD_TIME_PROFILE_3			"\n      Minimum: %d"
#define CMD_TIME_PROFILE_4			"\n      Average: %d"
#define CMD_TIME_PROFILE_5			"\nSample points: %d"

#define CMD_TIME_PROFILE_S			"\nTiming Profile Sampling Points:\n"


#define TIME_SAMPLES_PER_LINE 5
#define TIME_DELAY_MAX 9
#define TIME_DELAY_MIN 0

#define TIME_DELAY_MS	10
#define TIME_DELAY_SEC	1
#define TIME_MSEC_RES	100

#define CMD_US_TRIGGER	1000
#define SYSTICK_MAX_VALUE 16777216

void CmdTimeHelp (void);
void CMDTimePerformance (void);
void CMDTimePrintProfileSamples(void);
void CmdTimeStats (void);
void CmdTimeReset (void);
void CmdTimeDelay (uint8_t delayType);
void CmdTime (void);

#endif
