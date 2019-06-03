
/********************************************************************/
/*                                                                  */
/* File Name    : Gpio.c		                                    */
/*                                                                  */
/* General       : This module contain function for Initialize and  */
/* 					configure all gpio port use in M4 core          */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          Dec 26, 2016      Thao Ha(MV)  			new file        */
/*                                                                  */
/********************************************************************/

#include <Gpio.h>
#include "scu_18xx_43xx.h"



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	gpio_Init						             					    */
/*                                                                                      */
/* Details:         Init gpio port									    				*/
/*                                                                                      */
/* ReturnValue :    void                                     		                    */
/*                                                                                      */
/* Using Global Data : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void gpio_Init(void) {
	Chip_GPIO_Init(LPC_GPIO_PORT);

	Chip_SCU_PinMuxSet(0x4, 3, (SCU_MODE_FUNC0)); // Val1
	Chip_SCU_PinMuxSet(0x4, 0, (SCU_MODE_FUNC0)); // Val2
	Chip_SCU_PinMuxSet(0x4, 4, (SCU_MODE_FUNC0)); //Val3

	Chip_SCU_PinMuxSet(0x6, 1, (SCU_MODE_FUNC0)); // Flow controller reset
	Chip_SCU_PinMuxSet(0x1, 8, (SCU_MODE_FUNC0)); // Compressor reset
	Chip_SCU_PinMuxSet(0x1, 10, (SCU_MODE_FUNC0)); // Compressor Power On

	Chip_SCU_PinMuxSet(0x5, 0, (SCU_MODE_FUNC0)); // FAN_EN
	Chip_SCU_PinMuxSet(0x1, 0, (SCU_MODE_FUNC0)); // FAN_12V
	Chip_SCU_PinMuxSet(0x5, 1, (SCU_MODE_FUNC0)); // FAN_9V

	Chip_SCU_PinMuxSet(7, 4, SCU_MODE_FUNC0); //LED2
	Chip_SCU_PinMuxSet(7, 5, SCU_MODE_FUNC0); //LED1
	Chip_SCU_PinMuxSet(7, 6, SCU_MODE_FUNC0); //LED0
	Chip_SCU_PinMuxSet(7, 7, SCU_MODE_FUNC0); //LED3

	Chip_SCU_PinMuxSet(5, 3, SCU_MODE_FUNC0); //POW_ON

	Chip_SCU_PinMuxSet(1, 13, SCU_MODE_FUNC1); //UART1
	Chip_SCU_PinMuxSet(1, 14, SCU_MODE_FUNC1); //UART1

	Chip_SCU_PinMuxSet(6, 11, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //BAT_ST1

	Chip_SCU_PinMuxSet(4, 10, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC4)); //BAT_DETECT

	Chip_SCU_PinMuxSet(1, 12, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //AC_DETECT

	Chip_SCU_PinMuxSet(5, 4, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //CAR_DETECT

	Chip_SCU_PinMuxSet(1, 6, SCU_MODE_FUNC0); //BAT_ON

	Chip_SCU_PinMuxSet(4, 9, SCU_MODE_FUNC4); //CHARGE_EN
	Chip_SCU_PinMuxSet(9, 6, SCU_MODE_FUNC0); //CHARGE_EN_SUB

	Chip_SCU_PinMuxSet(9, 5, SCU_MODE_FUNC4); //TP108 //SCL
	Chip_SCU_PinMuxSet(1, 20, SCU_MODE_FUNC0); //TP109 //SDA

	Chip_SCU_PinMuxSet(5, 2, SCU_MODE_FUNC0); //USB_DETECT_5V
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	gpio_Set							              				    */
/*                                                                                      */
/* Details:       Set default value for gpio port										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void gpio_Set(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,true);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,AC_DETECT_PORT_NUM, AC_DETECT_BIT_NUM);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,CAR_DETECT_PORT_NUM, CAR_DETECT_BIT_NUM);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CHARGE_EN_SUB_PORT_NUM, CHARGE_EN_SUB_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CHARGE_EN_SUB_PORT_NUM, CHARGE_EN_SUB_BIT_NUM,true);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BAT_DETECT_PORT_NUM, BAT_DETECT_BIT_NUM);
}



