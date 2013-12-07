#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdhelp.h"
#include "cmdtime.h"

void CmdTime (void) {
	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (cmdPtr->argsNum) {
		switch (*cmdPtr->currentArgs) {
			case 'h':
			CmdTimeHelp();
			break;

			case 'r':
			CmdTimeReset();
			UARTprintf(CMD_TIME_RESET);
			break;

			case 'd':
			UARTprintf(CMD_DELAY_INIT);
			CmdTimeDelay(TIME_DELAY_SEC);
			break;

			case 'm':
			UARTprintf(CMD_DELAY_INIT);
			CmdTimeDelay(TIME_DELAY_MS);
			break;

			case 'p':
			CMDTimePerformance();
			break;

			case 'a':
			CMDTimePrintProfileSamples();
			break;

			default:
			UARTprintf(CMD_TIME_GENERIC_ERROR);
			break;
		}
	}
	else {
		CmdTimeStats();
	}
}

void CmdTimeReset (void) {
	timing_info_t *timing = GetTimerPointer();

	timing->miliSeconds = 0;
	timing->seconds = 0;
	timing->minutes = 0;
	timing->hours = 0;
	timing->days = 0;

	timing->cmdStart = 0;
	timing->cmdStop = 0;

	timing->elapsedTime = 0;
}


void CmdTimeDelay (uint8_t delayType) {
	uart_cmd_t *cmd = GetCmdPointer ();
	timing_info_t *timing = GetTimerPointer ();

	// capture increment unit, delay unit can be either
	// x second(s) or x/10th of a second
	uint8_t timeUnit = cmd->currentArgs[ARG_2] - KEY_ZERO;
	
	// capture current time in milliseconds
	uint16_t currentMs = timing->miliSeconds + 
			( timing->seconds * TIME_MSEC_RES * 10) ;
	
	// allocate future stop time
	uint16_t futureMs = 0;
	uint16_t elapsedMs = 0;

	// variables for interrupt check
	bool stopTimer = false;
	uint16_t currentTimerMs = 0;
	uint16_t previousTimerMs = 0; 

	// check for bounds
	if (timeUnit > TIME_DELAY_MIN  && timeUnit <= TIME_DELAY_MAX) {
		// calculate future stop time
		if (delayType == TIME_DELAY_SEC) {
			futureMs = currentMs + timeUnit * TIME_MSEC_RES * 10;
		}
		else {
			futureMs = currentMs + timeUnit * TIME_MSEC_RES;
		}

		// run timer
		previousTimerMs = 0;
		while (!stopTimer) {
			// check if timing interrupt has occured			
			currentTimerMs = timing->miliSeconds;
			if (currentTimerMs != previousTimerMs) {
				// update shift register
				previousTimerMs = currentTimerMs;
				// increment elapsed time and check if
				// if timer needs to stop
				currentMs += TIME_MSEC_RES;
				elapsedMs += TIME_MSEC_RES;
				if (currentMs >= futureMs) {
					stopTimer = true;
				}
			}
		}
		// Report back to user about elapsed time
		UARTprintf(CMD_TIME_DELAY_MSEC, elapsedMs);
	}
	else {
		UARTprintf(CMD_DELAY_ERROR);		
	}
}

void CmdTimeStats (void) {
	timing_info_t *timing = GetTimerPointer();
	float fElapsedTime = (float)timing->elapsedTime/SYSTICK_MAX_VALUE;

	
	UARTprintf(CMD_TIME_REPORT_1);
	UARTprintf(CMD_TIME_REPORT_2, timing->days, timing->hours, timing->minutes,
			 timing->seconds, timing->miliSeconds);

	// Convert to proper timing
	UARTprintf(CMD_TIME_REPORT_S, (uint32_t)(fElapsedTime),
			(uint32_t)(fElapsedTime*1000));

	// display us or ms based on elapsed time
	UARTprintf(CMD_TIME_REPORT_TICKS, timing->elapsedTime);

	UARTprintf(CMD_TIME_REPORT_4);
}

void CMDTimePerformance (void) {
	timing_info_t *timing = GetTimerPointer();
	uint8_t i = 0;
	uint8_t samplePoints = timing->eTindex;
	unsigned long maxTick = 0;
	unsigned long minTick = SYSTICK_MAX;
	unsigned long avgTick = 0;

	if (samplePoints == 0) { samplePoints = 1;}
	for (i = 1; i < samplePoints; i++) {
		// calculate max time
		if (timing->elapsedTimeProfile[i] > maxTick) {
			maxTick = timing->elapsedTimeProfile[i];
		}
		// calculate min time
		if (timing->elapsedTimeProfile[i] < minTick) {
			minTick = timing->elapsedTimeProfile[i];
		}
		// calculate average
		avgTick += timing->elapsedTimeProfile[i];
	}
	avgTick /= samplePoints;

	// show data
	UARTprintf(CMD_TIME_PROFILE_1);
	UARTprintf(CMD_TIME_PROFILE_2, maxTick);
	UARTprintf(CMD_TIME_PROFILE_3, minTick);
	UARTprintf(CMD_TIME_PROFILE_4, avgTick);
	UARTprintf(CMD_TIME_PROFILE_5, samplePoints);
}

void CMDTimePrintProfileSamples(void) {
	timing_info_t *timing = GetTimerPointer();
	uint8_t i = 0;

	UARTprintf(CMD_TIME_PROFILE_S);

	for (i = 0; i < TIMING_PROFILE_DEPTH; i++) {
		if (i%TIME_SAMPLES_PER_LINE == 0) {
			UARTprintf("\n");
		}
		UARTprintf("%d ", timing->elapsedTimeProfile[i]);
	}
}

void CmdTimeHelp (void) {
	UARTprintf(CMD_TIME_HELP_HEADER);
	UARTprintf(CMD_TIME_HELP_1);
	UARTprintf(CMD_TIME_HELP_2);
	UARTprintf(CMD_TIME_HELP_3);
	UARTprintf(CMD_TIME_HELP_4);
	UARTprintf(CMD_TIME_HELP_5);
	UARTprintf(CMD_TIME_HELP_6);
	UARTprintf(CMD_TIME_HELP_7);
	UARTprintf(CMD_TIME_HELP_8);
	UARTprintf(CMD_TIME_HELP_FOOTER);
}
