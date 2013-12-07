#ifndef __CMDSCRIPT_H__
#define __CMDSCRIPT_H__

#include <stdbool.h>


#define CMD_SCRIPT_HEADER		"\nAvailable commands:\n"
#define CMD_SCRIPT_LINE_1 		"script -h       -> Displays this help.\n"
#define CMD_SCRIPT_LINE_2 		"script          -> Starts new editor .\n"
#define CMD_SCRIPT_LINE_3 		"script -s -0 -0 -> Select script.\n"
#define CMD_SCRIPT_LINE_4 		"script -l       -> Loads selected script.\n"
#define CMD_SCRIPT_LINE_5 		"script -t       -> Lists scripts in memory.\n"
#define CMD_SCRIPT_LINE_6 		"script -d       -> Deletes selected script from memory.\n"
#define CMD_SCRIPT_LINE_7 		"script -c       -> Checks selected script for errors.\n"
#define CMD_SCRIPT_LINE_8 		".\n"
#define CMD_SCRIPT_LINE_9 		".\n"
#define CMD_SCRIPT_LINE_10 		".\n"
#define CMD_SCRIPT_FOOTER 		"eof"

#define CMD_SCRIPT_INIT			"\nScript module initialized.\n"
#define CMD_SCRIPT_ENABLED		"\nScript module enabled.\n"
#define CMD_SCRIPT_DISABLED		"\nScript module disabled.\n"


#define MAX_SCRIPTS 	10
#define MAX_NAME		10
#define MAX_LINE_SIZE 	45
#define MAX_LINES 		80

#define CMD_SCRIPT_CTRL_CH		0x1B			// also means ESC key
#define CMD_SCRIPT_CTRL_CN		0x5B			
#define CMD_SCRIPT_UP			0x41
#define CMD_SCRIPT_DOWN			0x42
#define CMD_SCRIPT_LEFT			0x44
#define CMD_SCRIPT_RIGHT		0x43
#define CMD_SCRIPT_CTRL_SIZE	0x03

#define CMD_PRINTABLE_START		0x20
#define CMD_PRINTABLE_END		0x7F

#define CMD_CLEAR_SCREEN		"\033[2J"
#define CMD_MOVE_CURSOR_UP		"\033[1A"
#define CMD_MOVE_CURSOR_DOWN	"\033[1B"
#define CMD_MOVE_CURSOR_RIGHT	"\033[1C"
#define CMD_MOVE_CURSOR_LEFT	"\033[1D"

//*****************************************************************************
// Global script settings
//*****************************************************************************
typedef struct {
	bool scriptModeOn;
	uint8_t currentScript;

	bool skipChar;
	uint8_t ctrlCharCount;

	char c;

} script_settings_t;

//*****************************************************************************
// Single script instance
//*****************************************************************************
typedef struct {
	char scriptName[MAX_NAME];
	char script[MAX_LINES][MAX_LINE_SIZE];

	bool compiled;
	bool failed;

} script_info_t;

//*****************************************************************************
// Cursor settings
//*****************************************************************************
typedef struct {

	uint8_t X;
	uint8_t Y;

} cursor_info_t;

script_settings_t *GetScriptSettingsPointer(void);
script_info_t *GetScriptPointer(void);
cursor_info_t *GetCursorPointer(void);

// Main Script command function
void CMDScript(void);
// Help function
void CMDScriptHelp(void);
// Callback handler for uart interrupt routine
// which is called during scripting
void ScriptCallBackHandler(char c);
// Initialization script
void ScriptInit(void);
// Cursor movement script
void ScriptCursorMove(char direction);
// Move cursor relative to current location
void ScriptCursorMoveXY(uint8_t x, uint8_t y);

#endif
