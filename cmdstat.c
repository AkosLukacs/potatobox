#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "utils\uartstdio.h"
#include "cmdline.h"
#include "cmdstat.h"

void CmdStat(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();

	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdStatHelp();
			break;

			case 'a':
			CmdStatAdvancedInfo();
			break;
		}
	}
	else {
		// Collect and display info
		UARTprintf("\n");
		UARTprintf(CMD_STAT_CLOCK,SysCtlClockGet()/1000000);
		UARTprintf(CMD_STAT_SRAM, SysCtlSRAMSizeGet()/1024);
		UARTprintf(CMD_STAT_FLASH, SysCtlFlashSizeGet()/1024);		
	}

}

void CmdStatAdvancedInfo (void) {
	uint32_t intState = SysCtlIntStatus(false);
	uint32_t pwmState = SysCtlPWMClockGet();
	uint32_t adcState = SysCtlADCSpeedGet();
	UARTprintf("\n");
	UARTprintf(CMD_STAT_SYSINT, intState);
	if (intState & SYSCTL_INT_MOSC_PUP) {
		UARTprintf("- Master Oscillator OK.\n");
	}

	if (intState & SYSCTL_INT_USBPLL_LOCK) {
		UARTprintf("- USB PLL Lock OK.\n");
	}

	if (intState & SYSCTL_INT_PLL_LOCK) {
		UARTprintf("- Master PLL Lock OK.\n");
	}

	if (intState & SYSCTL_INT_CUR_LIMIT) {
		UARTprintf("- Current Limit Interrupt.\n");
	}

	if (intState & SYSCTL_INT_PLL_FAIL) {
		UARTprintf("- PLL Lock Fail .\n");
	}

	UARTprintf(CMD_STAT_PWMCLK, pwmState);
	if (!pwmState) {	// default setting
		pwmState = 1;
	}
	if (pwmState & SYSCTL_PWMDIV_2) {
		pwmState = 2;
	}
	if (pwmState & SYSCTL_PWMDIV_4) {
		pwmState = 4;
	}
	if (pwmState & SYSCTL_PWMDIV_8) {
		pwmState = 8;
	}
	if (pwmState & SYSCTL_PWMDIV_16) {
		pwmState = 16;
	}
	if (pwmState & SYSCTL_PWMDIV_32) {
		pwmState = 32;
	}
	UARTprintf("- PWM Clock: %d Hz.\n", SysCtlClockGet()/pwmState);
	UARTprintf(CMD_STAT_ADCCLK, adcState);
	if (adcState & SYSCTL_ADCSPEED_1MSPS) {
		UARTprintf("- ADC Rate: 1 MHz.\n");
	}
	if (adcState & SYSCTL_ADCSPEED_500KSPS) {
		UARTprintf("- ADC Rate: 500 kHz.\n");
	}
	if (adcState & SYSCTL_ADCSPEED_250KSPS) {
		UARTprintf("- ADC Rate: 250 kHz.\n");
	}
	if (!adcState) {
		UARTprintf("- ADC Rate: 125 kHz.\n");
	}

}

void CmdStatHelp(void) {
	UARTprintf(CMD_STAT_HEADER);
	UARTprintf(CMD_STAT_LINE_1);
	UARTprintf(CMD_STAT_LINE_2);
	UARTprintf(CMD_STAT_LINE_3);
	UARTprintf(CMD_STAT_FOOTER);
}
