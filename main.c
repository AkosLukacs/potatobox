#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "library/cmdlib.h"




int main (void) {
	
    // init
    InitCommandLineSystem();

    // loop forever.
	while (1) {
		CMDLineScheduler();
		ProcScheduler();
	}
	return 0;
}



