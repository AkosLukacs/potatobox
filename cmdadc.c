#include <stdint.h>
#include <stdbool.h>
#include "driverlib\pin_map.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\sysctl.h"
#include "driverlib\adc.h"
#include "utils\uartstdio.h"
#include "cmdlib.h"

adc_info_t systemAdc;


void CmdAdc(void) {
	static bool init = true;
	uart_cmd_t *cmdPtr = GetCmdPointer();
	adc_info_t *adc = GetAdcPointer();
	int8_t ch = -1;

	// Initialize device on first run
	if (init){
		InitAdc(adc);
		UARTprintf(ADC_INIT_COMPLETE);
		init = false;
	}

	// Handle parameters
	if (cmdPtr->argsNum) {
		switch(*cmdPtr->currentArgs){
			case 'h':
			CmdAdcHelp();
			break;

			case 'a':
				ch = GetAdcValue(adc, cmdPtr);
				PrintSingleChannel(adc, ch);
			break;

			case 'd':
				GetAdcValues();
				PrintAllChannels();
			break;

			case 'c':
				ToggleTriggerEnable(adc, cmdPtr);
			break;

			case 't':
				SetTriggerValue(adc, cmdPtr);
			break;

			case 's':
				WaitForTrigger(adc, cmdPtr);
			break;

			case 'p':
				WriteToPool();
			break;

			case 'i':
				PrintAdcInformation(adc, cmdPtr);
			break;
		}
	}
	else {
		UARTprintf(CMD_ZERO_ARGS);
	}

}
adc_info_t *GetAdcPointer(void) {
	return &systemAdc;
}

void WriteToPool(void) {
	adc_info_t *a = GetAdcPointer();
	variable_info_t *p = GetPointerToVariable();
	uint32_t adcValue;
	if (a->lastCh != -1) {
		// convert to 8-bit
		adcValue = a->adcLastValue[a->lastCh];
		adcValue = adcValue >> 4;
		SetGlobalVariableValue((uint8_t)adcValue);
		if (p->writeEnable) {
			UARTprintf(ADC_WRITTEN_TO_POOL);
		}
	}
}

void InitAdc(adc_info_t *a) {
	uint8_t i = 0;
	for (i = 0; i < MAX_ADC_CHANNELS; i++){
		a->adcTrigger[i] = 0.0;
		a->triggerEnable[i] = false;
		a->adcLastValue[i] = 0;
		a->lastCh = 0;
	}

	// Initialize launchpad adc on following ports
	// PE4 - AIN9
	// PE5 - AIN8
	// PE0 - AIN3
	// PE1 - AIN2
	// PE2 - AIN1
	// PE3 - AIN0 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
    UARTprintf(ADC_GPIO_INIT_OK);

    // Configure sampling
    ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);	
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH8 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntEnable(ADC0_BASE, 0);
    UARTprintf(ADC_CONFIG_OK);

}

void GetAdcValues(void) {
	uint32_t res[6] = {0};
	uint8_t i = 0;
	adc_info_t *a = GetAdcPointer();

	ADCProcessorTrigger(ADC0_BASE, 0);
	while(!ADCIntStatus(ADC0_BASE, 0, false));
	ADCSequenceDataGet(ADC0_BASE, 0, &res[0]);
	if (ADCSequenceOverflow(ADC0_BASE, 0)){
		ADCSequenceOverflowClear(ADC0_BASE, 0);
		UARTprintf(ADC_OVEFLOW_ERROR);
	}
	ADCIntClear(ADC0_BASE, 0);
	// copy data over
	for (i = 0; i < MAX_ADC_CHANNELS; i++) {
		a->adcLastValue[i] = res[i];
	}
}

void WaitForTrigger(adc_info_t *a, uart_cmd_t *c) {
	timing_info_t *t = GetTimerPointer ();
	uint8_t timeout = 0;
	uint8_t elapsedTime = 0;
	uint8_t previousTime = 0;
	bool triggerHit = false;
	bool timeoutHit = false;
	uint8_t ch = a->lastCh;
	uint16_t value = 0;
	uint16_t trigValue = a->adcTrigger[a->lastCh];


	//check argument
	if(isNumericChar(c->currentArgs[ARG_2])) {
		timeout = c->currentArgs[ARG_2] - KEY_ZERO;
		UARTprintf(ADC_TIMEOUT_SET, timeout);
		UARTprintf(ADC_LAST_CH, ch);
	} else {
		UARTprintf(ADC_INVALID_CHAR, timeout);
		return;
	}

	// check trigger settings
	if (!a->triggerEnable[ch]){
		UARTprintf(ADC_TRIGGER_FAIL);
		return;
	}

	// Ensure equal values
	previousTime = t->seconds;
	// Start timer
	while (!triggerHit && !timeoutHit){
		// Sample ADC data
		GetAdcValues();
		value = a->adcLastValue[ch];
		if (value >= trigValue){
			triggerHit = true;
			UARTprintf(ADC_TRIGGER_HIT, value, elapsedTime);
		}
		// check if time has changed
		// don't write to uart in this loop
		// because variables can become 
		// out of sync due to the fact that
		// t->seconds are incremented via
		// interrupt.
		if (previousTime != t->seconds){
			elapsedTime++;
			// equalize
			previousTime = t->seconds;
		}
		// check if elapsed time is larger then timeout
		if (elapsedTime >= timeout){
			timeoutHit = true;
			UARTprintf(ADC_TIMEOUT_OCCURED);
		}
	}
}

void SetTriggerValue(adc_info_t *a, uart_cmd_t *c) {
	uint8_t valHi = 0, valLo = 0;

	if (c->argsNum == 3) {
		if (isNumericChar(c->currentArgs[ARG_2]) &&
			isNumericChar(c->currentArgs[ARG_3])){
			// convert to numeric
			valHi = c->currentArgs[ARG_2] - KEY_ZERO; 
			valLo = c->currentArgs[ARG_3] - KEY_ZERO; 
			// Convert to proper value
			if (a->triggerEnable[a->lastCh]){
				a->adcTrigger[a->lastCh] = (valHi * 10 +
					valLo) * ADC_CONV_FACTOR;
				UARTprintf(ADC_TRIGGER_SET, 
						a->adcTrigger[a->lastCh]);
			} else {
				UARTprintf(ADC_TRIGGER_FAIL);
			}
		} else {
		UARTprintf(ADC_INVALID_CHAR, valHi);	
		}
	} else {
		UARTprintf(ADC_FEW_ARG_ERROR);	
	}		
}

void ToggleTriggerEnable(adc_info_t *a, uart_cmd_t *c) {
	int8_t ch = -1;

	if (c->argsNum >= 2) {
		if (isNumericChar(c->currentArgs[ARG_2])){
			// convert to numeric
			ch = c->currentArgs[ARG_2] - KEY_ZERO; 
			if (ch < MAX_ADC_CHANNELS) {
				if (a->triggerEnable[ch]){
					a->triggerEnable[ch] = false;
					a->lastCh = ch;
					UARTprintf(ADC_TRIGGER_DIS);
				} else {
					a->triggerEnable[ch] = true;
					a->lastCh = ch;
					UARTprintf(ADC_TRIGGER_READY);
				}
			} else {
				UARTprintf(ADC_INVALID_CHANNEL, ch);
			}
		} else {
		UARTprintf(ADC_INVALID_CHAR, ch);	
		}
	} else {
		UARTprintf(ADC_FEW_ARG_ERROR);	
	}	
}

int8_t GetAdcValue (adc_info_t *a, uart_cmd_t *c) {
	int8_t ch = -1;
	uint8_t i = 0;
	uint32_t res[6] = {0};

	if (c->argsNum >= 2) {
		if (isNumericChar(c->currentArgs[ARG_2])){
			// convert to numeric
			ch = c->currentArgs[ARG_2] - KEY_ZERO; 
			if (ch < MAX_ADC_CHANNELS) {
				a->lastCh = ch;
				UARTprintf(ADC_TRIG_CONVERSION, ch);
				ADCProcessorTrigger(ADC0_BASE, 0);
				while(!ADCIntStatus(ADC0_BASE, 0, false));
				ADCSequenceDataGet(ADC0_BASE, 0, &res[0]);
				// copy data over
				for (i = 0; i < MAX_ADC_CHANNELS; i++) {
					a->adcLastValue[i] = res[i];
				}
			} else {
				UARTprintf(ADC_INVALID_CHANNEL, ch);
				ch = -1;
			}
		} else {
		UARTprintf(ADC_INVALID_CHAR, ch);
		ch = -1;	
		}
	} else {
		UARTprintf(ADC_FEW_ARG_ERROR);	
		ch = -1;
	}

	return ch;
}

void PrintSingleChannel(adc_info_t *a, int8_t ch) {
	if (ch >= 0){
		UARTprintf(ADC_SINGLE, ch, a->adcLastValue[ch]);
	}
}

void PrintAllChannels(void) {
	adc_info_t *a = GetAdcPointer();
	uint8_t i = 0;

	for (i = 0; i < MAX_ADC_CHANNELS; i++) {
		PrintSingleChannel(a, i);
	}
}

void PrintAdcInformation(adc_info_t *a, uart_cmd_t *c) {
	uint8_t i = 0;
	uint8_t ch = 0;
	UARTprintf(ADC_INFO_HEADER);

	// put check for numeric char
	if (c->argsNum == 1){
		UARTprintf(ADC_LAST_CH, a->lastCh);
		for (i = 0; i < MAX_ADC_CHANNELS; i++) {
			UARTprintf(ADC_STATUS_LINE
					,i
					,a->triggerEnable[i] ? ADC_TRIGGER_EN : ADC_TRIGGER_DIS
					,a->adcTrigger[i]
					,a->adcLastValue[i]);
		}
	} else {

		// check second argument
		if (isNumericChar(c->currentArgs[ARG_2])){
			ch = c->currentArgs[ARG_2] - KEY_ZERO; 
			if (ch < MAX_ADC_CHANNELS) {
				UARTprintf(ADC_STATUS_LINE
						,ch
						,a->triggerEnable[ch] ? ADC_TRIGGER_EN : ADC_TRIGGER_DIS
						,a->adcTrigger[ch]
						,a->adcLastValue[ch]);	
			} else {
				UARTprintf(ADC_INVALID_CHANNEL, ch);
			}	
		} else {
			UARTprintf(ADC_INVALID_CHAR, ch);	
		}
	}
}

void CmdAdcHelp(void) {
	UARTprintf(CMD_ADC_HEADER);
	UARTprintf(CMD_ADC_LINE_1);
	UARTprintf(CMD_ADC_LINE_2);
	UARTprintf(CMD_ADC_LINE_3);
	UARTprintf(CMD_ADC_LINE_4);
	UARTprintf(CMD_ADC_LINE_5);
	UARTprintf(CMD_ADC_LINE_6);
	UARTprintf(CMD_ADC_LINE_7);
	UARTprintf(CMD_ADC_LINE_8);
	UARTprintf(CMD_ADC_LINE_9);
	UARTprintf(CMD_ADC_FOOTER);
}
