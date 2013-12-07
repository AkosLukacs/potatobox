#include <stdint.h>
#include <stdbool.h>
#include "utils\uartstdio.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "cmdline.h"
#include "cmdgpio.h"
#include "cmdpool.h"


unsigned long portMap[16][2] = {{SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE},
							 {SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE},
							 {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE},
							 {SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE},
							 {SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE},
							 {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE},
							 {SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE},
							 {SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE},
							 {0, 0},						
							 {SYSCTL_PERIPH_GPIOJ, GPIO_PORTJ_BASE},
							 {SYSCTL_PERIPH_GPIOK, GPIO_PORTK_BASE},
							 {SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE},
							 {SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE},
							 {SYSCTL_PERIPH_GPION, GPIO_PORTN_BASE},
							 {0, 0},
							 {SYSCTL_PERIPH_GPIOP, GPIO_PORTP_BASE}};

unsigned long pinMap[] = {GPIO_PIN_0,
						  GPIO_PIN_1,
						  GPIO_PIN_2,
						  GPIO_PIN_3,
						  GPIO_PIN_4,
						  GPIO_PIN_5,
						  GPIO_PIN_6,
						  GPIO_PIN_7,
						  GPIO_PIN_ALL};

uint8_t currentPortIdx;
uint8_t currentPinIdx;
uint8_t currentPinValue;
uint8_t currentPortValue;

//
// Contains information on how to access the port
// either by individual pins or by port, default
// is pin access.
//
uint8_t portAccessMode[16] = {PORT_ACCESS_MODE_PIN};

//
// Contains info on pull-ups, initially disabled
//
bool portPullupEnable[16] = {false};

void CmdGpio(void) {

	uart_cmd_t *cmdPtr = GetCmdPointer();
	char arg = cmdPtr->currentArgs[ARG_1];
	uint8_t retVal = 0;

	// Obtain the port id from first argument
	// and check if port exists 
	if (cmdPtr->argsNum) {
		retVal = FindPort(arg);
		currentPortIdx = retVal;

		if (retVal == PORT_NOT_FOUND_NUM) {
			UARTprintf(PORT_NOT_FOUND_ER);
		}
		else {
			// Obtain port id number and enable port
			UARTprintf(PORT_FOUND, currentPortIdx);
			PortEnable(currentPortIdx);
			arg = cmdPtr->currentArgs[ARG_2];
			// Check if second argument is a number
			// if it is, then we are accessing individual 
			// pins, also check if global port access is
			// enabled
			if (arg >= KEY_ZERO && arg <= KEY_SEVEN &&
					 portAccessMode[currentPortIdx] == PORT_ACCESS_MODE_PIN) { 
				currentPinIdx = arg - KEY_ZERO;
				arg = cmdPtr->currentArgs[ARG_3];
				currentPinValue = arg - KEY_ZERO;
				PortWrite(currentPortIdx, currentPinIdx, currentPinValue);
				UARTprintf(PORT_PIN_VALUE, currentPinIdx, currentPinValue);
			}
			// Now check if second number is within range of hexadecimal chars
			// i.e 0,1,2,3,4,...,9,A,B,...F and if global port access is
			// enabled, write second and third arguments to port
			// specified by first argument
			else if (arg >= KEY_ZERO && arg <= KEY_HEX && 
					portAccessMode[currentPortIdx] == PORT_ACCESS_MODE_PORT) {
				// concatenate both values in argument 2 & 3
				currentPinValue = 0;
				currentPinValue |= (ConvertArgToHex(cmdPtr->currentArgs[ARG_2]) << 4);
				currentPinValue |=  ConvertArgToHex(cmdPtr->currentArgs[ARG_3]);
				// write to port
				GPIOPinTypeGPIOOutput(portMap[currentPortIdx][PORT_NAME], GPIO_PIN_ALL);	
				GPIOPinWrite(portMap[currentPortIdx][PORT_NAME], GPIO_PIN_ALL, currentPinValue);
				// tell user want was written to port
				UARTprintf(PORT_SET_TO, currentPinValue);
			}
			// the two checks above failed, hence the user wants either
			// to read some port value or set/reset port, check argument
			// and execute proper instruction
			else { 
				switch (arg) {
					case 'h':
					currentPortValue = PortRead(currentPortIdx);
					UARTprintf(PORT_VALUE, currentPortValue);
					// write to global variable
					SetGlobalVariableValue(currentPortValue);
					break;
					case 'g':
					currentPortValue = PortRead(currentPortIdx);
					PrintBinary(currentPortValue);
					// write to global variable
					SetGlobalVariableValue(currentPortValue);
					break;
					case 'r':
					PortWrite(currentPortIdx, GPIO_PIN_ALL, GPIO_VAL_ZERO);
					UARTprintf(PORT_RESET);
					break;
					case 's':
					PortWrite(currentPortIdx, GPIO_PIN_ALL, GPIO_VAL_ONE);
					UARTprintf(PORT_SET);
					break;
					case 'p':
					TogglePortAccessMode(currentPortIdx);
					break;
					case 'v':
					// write from global variable
					currentPortValue = GetGlobalVariableValue();
					GPIOPinTypeGPIOOutput(portMap[currentPortIdx][PORT_NAME], GPIO_PIN_ALL);	
					GPIOPinWrite(portMap[currentPortIdx][PORT_NAME], GPIO_PIN_ALL, currentPortValue);
					UARTprintf(PORT_VALUE, currentPortValue);
					break;
					case 'l':
					TogglePortPullupEnable(currentPortIdx);
					break;
				}
				PortDisable(currentPortIdx);
			}
		}
		currentPortIdx = 0;
		currentPinIdx = 0;
		currentPinValue = 0;
		currentPortValue = 0;
	}
	else {
		CmdGpioHelp();
	}
}

void CmdGpioHelp(void) {
	UARTprintf(CMD_GPIO_HEADER);
	UARTprintf(CMD_GPIO_LINE_1);
	UARTprintf(CMD_GPIO_LINE_2);
	UARTprintf(CMD_GPIO_LINE_3);
	UARTprintf(CMD_GPIO_LINE_4);
	UARTprintf(CMD_GPIO_LINE_5);
	UARTprintf(CMD_GPIO_LINE_6);
	UARTprintf(CMD_GPIO_LINE_7);
	UARTprintf(CMD_GPIO_LINE_8);
	UARTprintf(CMD_GPIO_LINE_9);
	UARTprintf(CMD_GPIO_LINE_10);
	UARTprintf(CMD_GPIO_LINE_11);
	UARTprintf(CMD_GPIO_FOOTER);
}

uint8_t FindPort (char port) {

	uint8_t ret = PORT_NOT_FOUND_NUM;
	uint8_t portId = port - KEY_ALPHA;
	

	if (portId <= PORTS_TOTAL - 1 && portMap[portId][PERIPH_NAME] != 0) {
		return portId; 
	} 
	else {
		return ret;
	}
}

bool PortEnable (uint8_t portId) {
	if(!SysCtlPeripheralReady(portMap[portId][PERIPH_NAME])) {
		SysCtlPeripheralEnable(portMap[portId][PERIPH_NAME]);
		return true;
	}
	else {
		UARTprintf(PORT_READY);
		return true;
	}
}

bool PortDisable (uint8_t portId) {
	// SysCtlPeripheralDisable(portMap[portId][PERIPH_NAME]);
	return true;
}

uint8_t PortWrite(uint8_t portId, uint8_t pin, uint8_t value) {

	// make sure value is either zero or one
	if (value > 0) {
		value = GPIO_VAL_ONE;
	}
	else {
		value = GPIO_VAL_ZERO;
	}

	// access and write to correct port
	if (pin == GPIO_PIN_ALL) {
		GPIOPinTypeGPIOOutput(portMap[portId][PORT_NAME], GPIO_PIN_ALL);	
		GPIOPinWrite(portMap[portId][PORT_NAME], GPIO_PIN_ALL, value);
	}
	else {
		GPIOPinTypeGPIOOutput(portMap[portId][PORT_NAME], pinMap[pin]);
		GPIOPinWrite(portMap[portId][PORT_NAME], pinMap[pin], value);
	}
	return true;
}

uint8_t PortRead(uint8_t portId) {
	// set gpio to input
	GPIOPinTypeGPIOInput(portMap[portId][PORT_NAME], GPIO_PIN_ALL);
	// enable pullups if set
	if (portPullupEnable[portId]) {
		GPIOPadConfigSet(portMap[portId][PORT_NAME], GPIO_PIN_ALL,
				GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
		return GPIOPinRead(portMap[portId][PORT_NAME], GPIO_PIN_ALL);
	}
	else {
		return GPIOPinRead(portMap[portId][PORT_NAME], GPIO_PIN_ALL);
	}
}

void TogglePortPullupEnable (uint8_t portId) {
	if (portPullupEnable[portId]) {
		portPullupEnable[portId] = false;
		UARTprintf(PORT_PULLUP_RESET);		
	}
	else {
		portPullupEnable[portId] = true;
		UARTprintf(PORT_PULLUP_SET);	
	}	
}

void TogglePortAccessMode(uint8_t portId) {
	if (portAccessMode[portId] == PORT_ACCESS_MODE_PIN) {
		portAccessMode[portId] = PORT_ACCESS_MODE_PORT;
		UARTprintf(PORT_MODE, (char)(portId + KEY_ALPHA), PORT_MODE_PORT);
	} 
	else {
		portAccessMode[portId] = PORT_ACCESS_MODE_PIN;
		UARTprintf(PORT_MODE, (char)(portId + KEY_ALPHA), PORT_MODE_PIN);
	}

}

void PrintBinary (uint8_t value) {
	uint8_t i = 0;
	
	UARTprintf(PORT_VALUE_BIN);

	while(i <= GPIO_BITS_NUM) {
		UARTprintf("%d", BitSet(value, i));
		if (i == GPIO_BIT_SPACE){
			UARTprintf(" ");
		}
		i++;
	}
	UARTprintf("\n");
}

bool BitSet(uint8_t value, uint8_t bitNum) {
	uint8_t val = value;
	val &= 1 << bitNum;

	if (val > 0) {
		return true;
	}
	return false;
}

uint8_t ConvertArgToHex(char arg) {

	bool numRange = false;
	bool alpRange = false;

	// check if char is within numeric range
	if (arg >= KEY_ZERO && arg <= KEY_NINE) {
		numRange = true;
	}

	// check if char is within hex num range
	if (arg >= KEY_ALPHA && arg <= KEY_HEX)
	{
		alpRange = true;
	}

	// check if one of them is true, otherwise return 0
	if (numRange) {
		return (arg - KEY_ZERO);
	}
	else if (alpRange) {
		return (arg - KEY_ALPHA + HEX_OFFSET);
	}
	else {
		return 0;
	}
}
