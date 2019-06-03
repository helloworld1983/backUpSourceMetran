/*
===============================================================================
 Name        : Portable_Bootloader.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
  //2017/10/05	Quyen Ngo	remove Board_LED_Sets
===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#include "UsbMgr.h"
#include "common.h"
#include "boot.h"
#include "SPIFMgr.h"
#include "Led.h"
#include "updateFlash.h"



int main(void) {

#if defined (__USE_LPCOPEN)


#if !defined(NO_BOARD_LIB)
#if defined (__MULTICORE_MASTER) || defined (__MULTICORE_NONE)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
//	Board_Init();
#endif
#endif
#endif
	// TODO: insert code here
	SetupHardware();

	DEBUGOUT("Go to bootloader \n");
	DEBUGSTR("GO to");
	boot_startBootloader();
	return 0 ;
}
