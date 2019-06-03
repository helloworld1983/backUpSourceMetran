/********************************************************************/
/*                                                                  */
/* File Name    : gpio.h                                    		*/
/*                                                                  */
/* General      : This module contains function to initialize GPIO,	*/
/* 				set default valut to GPIO and check current source	*/
/*              of input voltage				                    */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/06       Thao Ha		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "gpio.h"
#include "board.h"
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	gpio_Init						             					    */
/*                                                                                      */
/* Details:         Init gpio port									    				*/
/*                                                                                      */
/* ReturnValue :    void                                     		                    */
/*                                                                                      */
/****************************************************************************************/
void gpio_Init(void)
{
	Chip_SCU_PinMuxSet(5, 3, SCU_MODE_FUNC0); //POW_ON
	Chip_SCU_PinMuxSet(1, 13, SCU_MODE_FUNC1); //UART1
	Chip_SCU_PinMuxSet(1, 14, SCU_MODE_FUNC1); //UART1

	Chip_SCU_PinMuxSet(1, 6, SCU_MODE_FUNC0); //BAT_ON

	Chip_SCU_PinMuxSet(1, 12, (SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0)); //AC_DETECT
	Chip_SCU_PinMuxSet(5, 4, (SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0)); //CAR_DETECT

	Chip_SCU_PinMuxSet(4, 10, (SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC4)); //BAT_DETECT

	Chip_SCU_PinMuxSet(1, 7, (SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0)); //VBAT_SEN

	Chip_SCU_PinMuxSet(0x5, 0, (SCU_MODE_FUNC0)); // FAN_EN
	Chip_SCU_PinMuxSet(0x1, 0, (SCU_MODE_FUNC0)); // FAN_12V
	Chip_SCU_PinMuxSet(0x5, 1, (SCU_MODE_FUNC0)); // FAN_9V

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
/****************************************************************************************/
void gpio_Set(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,true);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,AC_DETECT_PORT_NUM, AC_DETECT_BIT_NUM);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,CAR_DETECT_PORT_NUM, CAR_DETECT_BIT_NUM);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BAT_DETECT_PORT_NUM, BAT_DETECT_BIT_NUM);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, VBAT_SEN_PORT_NUM, VBAT_SEN_BIT_NUM);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	gpio_InputVoltageCheck				              				    */
/*                                                                                      */
/* Details:     Check which source of voltage is using to supply power for the system   */
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : E_ChargingState			                                            */
/*                                                                                      */
/****************************************************************************************/
E_ChargingState gpio_InputVoltageCheck(void)
{
	if((Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,AC_DETECT_PORT_NUM,AC_DETECT_BIT_NUM)==true)
			&& (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,CAR_DETECT_PORT_NUM,CAR_DETECT_BIT_NUM)==true))
	{
		return eACCharge;
	}
	else
		if((Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,AC_DETECT_PORT_NUM,AC_DETECT_BIT_NUM)==true)
				&& (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,CAR_DETECT_PORT_NUM,CAR_DETECT_BIT_NUM)==false))
		{
			return eCarCharge;
		}
		else
		{
			return eNoCharge;
		}
}
