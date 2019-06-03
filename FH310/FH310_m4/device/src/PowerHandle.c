/********************************************************************/
/*                                                                  */
/* File Name    : PowerHandle.c	                                    */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* 				   power detection interrupt, this interrupt only   */
/* 				   init when upgrade bootloader and M0 do not init  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*           2017/08/04      	Quyen Ngo(MV)  		new file        */
/*#0001		 2017/08/24			Quyen Ngo(MV)	Modify handle power	*/
/*			 	detection interrupt, reset watchdog instead of 		*/
/*			 	shutdown battery power for fix bug 1751				*/
/*                                                                  */
/*#0002		 2017/0929			Quyen Ngo(MV)       modify			*/
/*		-Change the power detection interrupt to level mode			*/
/********************************************************************/

#include <board.h>
#include "PowerHandle.h"
#include "DualCoreCommon.h"

#define POWER24_DETECT_PORT_NUM			1
#define POWER24_DETECT_BIT_NUM			8
#define POWER_BACKUP_EN_PORT_NUM 		0
#define POWER_BACKUP_EN_BIT_NUM 		11

#define POWER_PININT_INDEX   1	/* PININT index used for GPIO mapping */
#define POWER_PININT_IRQ_HANDLER  GPIO1_IRQHandler	/* GPIO interrupt IRQ function name */
#define POWER_PININT_NVIC_NAME    PIN_INT1_IRQn	/* GPIO interrupt NVIC interrupt name */

/****************************************************************************************/
/*                                                                                      */
/* Function name 	powerHandle_Init												    */
/*                                                                                      */
/* Details  -init the interrupt for power detection									    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*Macro use :																			*/
/*				 LPC_GPIO_PORT	 ((LPC_GPIO_T*) 			LPC_GPIO_PORT_BASE)     	*/
/*				 POWER24_DETECT_PORT_NUM	 									    	*/
/*				 POWER24_DETECT_BIT_NUM	 									    		*/
/*				 POWER_PININT_INDEX		 									    		*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : 								                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void powerHandle_Init(void)
{
	/* Configure GPIO pin as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,POWER24_DETECT_PORT_NUM, POWER24_DETECT_BIT_NUM);		/* Power 24 detect */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,POWER_BACKUP_EN_PORT_NUM, POWER_BACKUP_EN_BIT_NUM);		/* Battery enable */

	/* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SCU_GPIOIntPinSel(POWER_PININT_INDEX, POWER24_DETECT_PORT_NUM, POWER24_DETECT_BIT_NUM);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(POWER_PININT_INDEX));
	//work around for set GPIO level interrupt
	Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(POWER_PININT_INDEX));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(POWER_PININT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(POWER_PININT_INDEX));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(POWER_PININT_NVIC_NAME);
	NVIC_SetPriority(POWER_PININT_NVIC_NAME,POWER_INT_PRIO);
	NVIC_EnableIRQ(POWER_PININT_NVIC_NAME);




	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	POWER_PININT_IRQ_HANDLER							                */
/*                                                                                      */
/* Details:         Handle power detection interrupt				    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data :								                           			*/
/*                                                                                      */
/****************************************************************************************/
void POWER_PININT_IRQ_HANDLER(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(POWER_PININT_INDEX));
	NVIC_DisableIRQ(POWER_PININT_NVIC_NAME);
	/* Initialize WWDT and event router */
	Chip_WWDT_Init(LPC_WWDT);
	Chip_WWDT_SetTimeOut(LPC_WWDT, 0xFF);//minimum timeout canbe set
	/* Configure WWDT to reset on timeout */
	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);
	/* Enable watchdog interrupt */
	NVIC_EnableIRQ(WWDT_IRQn);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	powerHandle_DisablePowerDetection					                */
/*                                                                                      */
/* Details:         Disable power detection interrupt				    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data :								                           			*/
/*                                                                                      */
/****************************************************************************************/
void powerHandle_DisablePowerDetection(void)
{
	NVIC_DisableIRQ(POWER_PININT_NVIC_NAME);
}

