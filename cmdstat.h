#ifndef __CMDSTAT_H__
#define __CMDSTAT_H__

#define CMD_STAT_HEADER		"\nAvailable commands:\n"
#define CMD_STAT_LINE_1		"stat -h       -> Displays this help.\n"
#define CMD_STAT_LINE_2		"stat          -> Display info about hardware.\n"
#define CMD_STAT_LINE_3		"stat -a       -> Displays advanced info.\n"
#define CMD_STAT_FOOTER		"eof\n"

#define CMD_STAT_CLOCK		"Clock:\t\t%d Mhz\n"
#define CMD_STAT_SRAM		"SRAM:\t\t%d kb\n"
#define CMD_STAT_FLASH		"FLASH:\t\t%d kb\n"

#define CMD_STAT_ENABLED	"enabled"
#define CMD_STAT_DISABLED	"disabled"

#define CMD_STAT_SYSINT		"System interrupt status: 0x%x\n"
#define CMD_STAT_PWMCLK		"System PWM Clock Setting: 0x%x\n"
#define CMD_STAT_ADCCLK		"System ADC Clock Setting: 0x%x\n"


void CmdStat (void);
void CmdStatHelp (void);
void CmdStatAdvancedInfo (void);

#endif
