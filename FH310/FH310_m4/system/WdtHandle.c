/********************************************************************/
/*                                                                  */
/* File Name    : WdtHandle.c                          				*/
/*                                                                  */
/* General      : Contains functions for int and feed wdt in early	*/
/*				  state when RTOS not running						*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/09/21       Quyen Ngo(MV)	    new file        */
/* #0001	   2017/09/21		Quyen Ngo(MV)		modify			*/
/*		Change WDT timeout to 5s, remove function not use, fix init	*/
/*		sequence													*/
/********************************************************************/

#include "board.h"
#include "WdtHandle.h"
#include "DualCoreCommon.h"

/****************************************************************************************/
/*                                                                                      */
/* General 	-WDT_IRQHandler												    			*/
/*                                                                                      */
/* Details  -Handle functio n for wdt IRQ		    				        			*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void WDT_IRQHandler(void)
{
	uint32_t wdtStatus = Chip_WWDT_GetStatus(LPC_WWDT);

	/* The chip will reset before this happens, but if the WDT doesn't
		   have WWDT_WDMOD_WDRESET enabled, this will hit once */
	if (wdtStatus & WWDT_WDMOD_WDTOF) {
		/* A watchdog feed didn't occur prior to window timeout */
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
	}

	/* Handle warning interrupt */
	if (wdtStatus & WWDT_WDMOD_WDINT) {
		/* A watchdog feed didn't occur prior to warning timeout */
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDINT);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-InitWDT													    			*/
/*                                                                                      */
/* Details  -Initialize WatchDog timer			    				        			*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void wdt_InitWDT(void)
{
	Chip_WWDT_Init(LPC_WWDT);
	Chip_WWDT_SetTimeOut(LPC_WWDT, 5*WDT_1S_CNT);

	/* Configure WWDT to reset on timeout */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

	/* Clear watchdog warning and timeout interrupts */
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);
	Chip_WWDT_Start(LPC_WWDT);

	NVIC_ClearPendingIRQ(WWDT_IRQn);
	NVIC_EnableIRQ(WWDT_IRQn);
	NVIC_SetPriority(WWDT_IRQn,WDT_INT_PRIO);

	return;
}

/*end of file*/

