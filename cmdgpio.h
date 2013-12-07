#ifndef __CMDGPIO_H__
#define __CMDGPIO_H__
#include <stdint.h>
#include <stdbool.h>

#define CMD_GPIO_HEADER		"\nAvailable commands:\n"
#define CMD_GPIO_LINE_1		"gpio -g -h    -> Returns port G values in hex. (writes to pool)\n"
#define CMD_GPIO_LINE_2		"gpio -g -g    -> Returns port G values in binary. (writes to pool)\n"
#define CMD_GPIO_LINE_3		"gpio -a -2 -1 -> Sets pin 2 on port A to logic 1 (Pin Mode).\n"
#define CMD_GPIO_LINE_4     "gpio -a -2 -1 -> Sets port A to 0x21 (Port Mode).\n"
#define CMD_GPIO_LINE_5		"gpio -a -r    -> Resets port A.\n"
#define CMD_GPIO_LINE_6		"gpio -a -s    -> Sets port A.\n"
#define CMD_GPIO_LINE_7		"gpio          -> Displays this help.\n"
#define CMD_GPIO_LINE_8		"gpio -b       -> Enables port B.\n"
#define CMD_GPIO_LINE_9		"gpio -a -p    -> Toggles 8 bit writes to port.\n"
#define CMD_GPIO_LINE_10	"gpio -d -v    -> Writes global variable value to port.\n"
#define CMD_GPIO_LINE_11	"gpio -g -l    -> Toggles internal pull-ups on current port.\n"
#define CMD_GPIO_FOOTER		"eof"

#define GPIO_PIN_ALL 0xFF
#define GPIO_VAL_ZERO 0x00
#define GPIO_VAL_ONE  0xFF
#define GPIO_BITS_NUM	7
#define GPIO_BIT_SPACE 3

#define PORTS_TOTAL	16
#define PERIPH_NAME 0
#define PORT_NAME 1
#define PORT_ACCESS_MODE_PIN 0
#define PORT_ACCESS_MODE_PORT 1

#define PORT_NOT_FOUND_NUM	0xFF
#define PORT_NOT_FOUND_ER	"\nPort not found."
#define PORT_FOUND 			"\nPort Found at entry: %d."

#define PORT_READY			"\nPort Enabled and Ready."
#define PORT_NOT_READY		"\nPort not ready."
#define PORT_VALUE			"\nPort Value: 0x%x"
#define PORT_VALUE_BIN		"\nPort Value: 0b"
#define PORT_PIN_VALUE		"\nPin %d set to %d"
#define PORT_SET 			"\nPort set."
#define PORT_RESET			"\nPort reset."
#define PORT_SET_TO			"\nPort set to: 0x%x"
#define PORT_PULLUP_SET		"\nPullup enabled."
#define PORT_PULLUP_RESET	"\nPullup disabled."
#define PORT_MODE 			"\nPort %c mode set to : %s"
#define PORT_MODE_PIN 		"pin access."
#define PORT_MODE_PORT 		"port access."

uint8_t FindPort (char port);
bool PortEnable (uint8_t portId);
bool PortDisable (uint8_t portId);
uint8_t PortWrite(uint8_t portId, uint8_t pin, uint8_t value);
uint8_t PortRead(uint8_t portId);
void PrintBinary (uint8_t value);
bool BitSet(uint8_t value, uint8_t bitNum);
void TogglePortAccessMode(uint8_t portId);
uint8_t ConvertArgToHex(char arg);
void TogglePortPullupEnable (uint8_t portId);


void CmdGpio(void);
void CmdGpioHelp(void);


#endif
