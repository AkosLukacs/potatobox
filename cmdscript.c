#include <stdint.h>
#include "utils\uartstdio.h"
#include "cmdlib.h"
#include "cmdline.h"
#include "cmdscript.h"

script_settings_t scriptSettings;
cursor_info_t cursor;
script_info_t scripts[MAX_SCRIPTS];

void CMDScript (void) {
	static bool init = true;

	uart_cmd_t *cmd = GetCmdPointer();
	uart_info_t *uart = GetUartPointer();
	stack_info_t *stack = GetStackPointer();

	if (init) {
		ScriptInit();
		UARTprintf(CMD_SCRIPT_INIT);
		init = false;
	}

	if (cmd->argsNum) {
		switch(GetArgument(0)){
			case 'h': CMDScriptHelp();
			break;
		}
	} else {
		scriptSettings.scriptModeOn = true;
		UARTprintf(CMD_SCRIPT_ENABLED);
	}

}

void CMDScriptHelp(void) {
	UARTprintf(CMD_SCRIPT_HEADER);
	UARTprintf(CMD_SCRIPT_LINE_1);
	UARTprintf(CMD_SCRIPT_LINE_2);
	UARTprintf(CMD_SCRIPT_LINE_3);
	UARTprintf(CMD_SCRIPT_LINE_4);
	UARTprintf(CMD_SCRIPT_LINE_5);
	UARTprintf(CMD_SCRIPT_LINE_6);
	UARTprintf(CMD_SCRIPT_LINE_7);
	UARTprintf(CMD_SCRIPT_LINE_8);
	UARTprintf(CMD_SCRIPT_LINE_9);
	UARTprintf(CMD_SCRIPT_LINE_10);
	UARTprintf(CMD_SCRIPT_FOOTER);
}


script_settings_t *GetScriptSettingsPointer(void) {
	return &scriptSettings;
}

script_info_t *GetScriptPointer(void) {
	return scripts;
}

cursor_info_t *GetCursorPointer(void) {
	return &cursor;
}

void ScriptInit(void) {
	script_settings_t *s = GetScriptSettingsPointer();
	cursor_info_t *c = GetCursorPointer();

	s->scriptModeOn = false;
	s->currentScript = 0;
	s->skipChar = false;
	s->ctrlCharCount = 0;

	c->X = 0;
	c->Y = 0;
}

void ScriptCallBackHandler(char c) {
	script_settings_t *s = GetScriptSettingsPointer();
	cursor_info_t *cur = GetCursorPointer();
	script_info_t *file = GetScriptPointer();
	volatile static bool init = true;
	volatile uint8_t pos = 0;
	volatile static uint8_t spaceChars = 0;

	if (init) {
		UARTprintf(CMD_CLEAR_SCREEN);
		init = false;
		s->ctrlCharCount = 0;
		s->skipChar = false;

		// reset cursor position, vertical
		//for (pos = 0; pos >= 23; pos++) {
		//	ScriptCursorMove(CMD_SCRIPT_UP);
		//}

	} else {

		// getchar
		s->c = c;

		// catch multiple spaces
		if (c == KEY_SPACE) {
			spaceChars++;
			if (spaceChars >= 2) {
				spaceChars = 0;
				c = KEY_ENTER;
			}
		} else {
			spaceChars = 0;
		}

		// catch movement control chars
		switch(c) {
			case CMD_SCRIPT_CTRL_CH :
				s->ctrlCharCount = 1;
			break;
			case CMD_SCRIPT_CTRL_CN :
				s->ctrlCharCount = 2;
			break;
			case CMD_SCRIPT_UP :
				ScriptCursorMove(CMD_SCRIPT_UP);
				s->ctrlCharCount++;
			break;
			case CMD_SCRIPT_DOWN :
				ScriptCursorMove(CMD_SCRIPT_DOWN);
				s->ctrlCharCount++;
			break;
			case CMD_SCRIPT_LEFT :
				ScriptCursorMove(CMD_SCRIPT_LEFT);
				s->ctrlCharCount++;
			break;
			case CMD_SCRIPT_RIGHT :
				ScriptCursorMove(CMD_SCRIPT_RIGHT);
				s->ctrlCharCount++;
			break;
			default : 
				s->ctrlCharCount = 0;
			break;
		}

		// handle control char skipping
		if (s->ctrlCharCount == 0) {
			s->skipChar = false;
		} else {
			s->skipChar = true;
		}

		// print character and write to memory if printable char
		// char will be skipped if last character recieved is a part
		// of control char stream
		//
		// Must also be able to handle direct pasting from files
		// End of line should be handled properly
		if (s->skipChar == false) {
			if (c == KEY_ENTER || c == '\n') {
				ScriptCursorMove(CMD_SCRIPT_DOWN);
				for (pos = cur->X; pos != 0; pos--) {
					ScriptCursorMove(CMD_SCRIPT_LEFT);
				}
			} else {
				if (cur->X + 1 <= MAX_LINE_SIZE) {
					UARTprintf("%c",c);
					cur->X = cur->X + 1;
					//file[s->currentScript].script[cur->X][cur->Y] = c;
				}
			}
		}
	}
}


void ScriptCursorMove(char direction) {
	cursor_info_t *cur = GetCursorPointer();
	script_settings_t *s = GetScriptSettingsPointer();

	switch(direction) {
		case CMD_SCRIPT_UP :
			if (cur->Y + 1 <= MAX_LINES) {
				cur->Y = cur->Y + 1;
				UARTprintf(CMD_MOVE_CURSOR_UP);
			}
		break;
		case CMD_SCRIPT_DOWN :
			if (cur->Y - 1 >= 0) {
				cur->Y = cur->Y - 1;
				UARTprintf(CMD_MOVE_CURSOR_DOWN);
			}
		break;
		case CMD_SCRIPT_LEFT :
			if (cur->X - 1 >= 0) {
				cur->X = cur->X - 1;
				UARTprintf(CMD_MOVE_CURSOR_LEFT);
			}
		break;
		case CMD_SCRIPT_RIGHT :
			if (cur->X + 1 <= MAX_LINE_SIZE) {
				cur->X = cur->X + 1;
				UARTprintf(CMD_MOVE_CURSOR_RIGHT);
			}
		break;
	}
}
