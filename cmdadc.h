#ifndef __CMDADC_H__
#define __CMDADC_H__


#define CMD_ADC_HEADER		"\nAvailable commands:\n"
#define CMD_ADC_LINE_1		"adc  -h       -> Displays this help.\n"
#define CMD_ADC_LINE_2		"adc  -a -0    -> Returns voltage on channel 0 (channel selection is stored)\n"
#define CMD_ADC_LINE_3		"adc  -d       -> Displays voltages on all channels.\n"
#define CMD_ADC_LINE_4		"adc  -c -0    -> Toggles trigger on channel 0.\n"
#define CMD_ADC_LINE_5		"adc  -t -2 -3 -> Sets trigger voltage to 2.3 Volts.\n"
#define CMD_ADC_LINE_6		"adc  -s -3    -> Adc goes to sleep waiting for trigger.\n"
#define CMD_ADC_LINE_7		"              -> with timeout of 3 seconds.\n"
#define CMD_ADC_LINE_8		"adc  -p       -> Writes value of last checked channel to pool.\n"
#define CMD_ADC_LINE_9		"adc  -i       -> Display adc configuration parameters.\n"
#define CMD_ADC_FOOTER		"eof"

#define ADC_GPIO_INIT_OK 	"\nADC GPIO Initiated."
#define ADC_CONFIG_OK		"\nADC configured."
#define ADC_INIT_COMPLETE	"\nADC initialized."
#define ADC_INVALID_CHANNEL	"\nInvalid Channel: %d."
#define ADC_INVALID_CHAR	"\nInvalid char: %d."
#define ADC_FEW_ARG_ERROR	"\nToo few arguments specified.\n"
#define ADC_TRIG_CONVERSION	"\nTriggering conversion on channel: %d"
#define ADC_WRITTEN_TO_POOL	"\nValue stored in pool. (Truncated!)"
#define ADC_TRIGGER_SET		"\nTrigger set to 0x%x"
#define ADC_TRIGGER_FAIL	"\nTrigger not enabled."
#define ADC_TRIGGER_READY	"\nTrigger enabled."
#define ADC_TRIGGER_DIS		"\nTrigger disabled."
#define ADC_LAST_CH			"\nCurrent channel: %d."
#define ADC_INVALID_TIMEOUT	"\nInvalid timeout set: %d."
#define ADC_TIMEOUT_SET		"\nTimeout set to %d seconds."
#define ADC_TRIGGER_HIT		"\nTrigger Hit @ 0x%x after %d seconds."
#define ADC_WAITING_TRIGGER	"\nWaiting for trigger: %d/%d."
#define ADC_TIMEOUT_OCCURED	"\nTimeout has occured, please retrigger."
#define ADC_OVEFLOW_ERROR	"\nADC Overflow error."

#define ADC_INFO_HEADER		"\nADC information."
#define ADC_STATUS_LINE		"\nChannel: %d Trigger: %s Trigger Value: 0x%x Current value: 0x%x"
#define ADC_SINGLE			"\nChannel: %d Value: 0x%x"
#define ADC_TRIGGER_EN		"enabled"
#define ADC_TRIGGER_DISB		"disabled"

#define MAX_ADC_CHANNELS	6
#define ADC_CONV_FACTOR		124
#define MAX_TIMEOUT_SECONDS	9

typedef struct {
	
	uint16_t adcTrigger[MAX_ADC_CHANNELS];
	uint16_t adcLastValue[MAX_ADC_CHANNELS];
	bool triggerEnable[MAX_ADC_CHANNELS];

	//
	// used to identify last channel that was accessed
	// or channel to set the trigger on
	//
	int8_t lastCh;	

} adc_info_t;


adc_info_t *GetAdcPointer(void);
void InitAdc(adc_info_t *a);
void PrintAdcInformation(adc_info_t *a, uart_cmd_t *c);
int8_t GetAdcValue (adc_info_t *a, uart_cmd_t *c);
void GetAdcValues(void);
void PrintSingleChannel(adc_info_t *a, int8_t ch);
void PrintAllChannels(void);
void WriteToPool(void);
void ToggleTriggerEnable(adc_info_t *a, uart_cmd_t *c);
void SetTriggerValue(adc_info_t *a, uart_cmd_t *c);
void WaitForTrigger(adc_info_t *a, uart_cmd_t *c);


void CmdAdc(void);
void CmdAdcHelp(void);

#endif
