/********************************************************************/
/*                                                                  */
/* File Name    : Encoder.h		                                    */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* 				   encoder operation								*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/29      Quyen Ngo(MV)  	new file        	*/
/*                                                                  */
/* #0001	   2017/10/12	   Quyen Ngo(MV)    modify				*/
/*		-Add debugout to detect IPC queue full						*/
/* #0002	   2017/10/22	   Quyen Ngo(MV)    modify				*/
/*		-Change the wait to handle interrupt not use semaphore, now	*/
/*		encoder task is scheduled 50ms								*/
/* #0003 	   2017/10/23	   Quyen Ngo(MV)	modify				*/
/*		-copy the gs_stepVal value to a local variable and calculate*/
/*		base on this local variable(Bug 1996)						*/
/* #0004 	   2017/11/02	   Quyen Ngo(MV)	modify				*/
/*		-Handle the case IPC queue full when send encode msg by		*/
/*		the way: Only update gs_oldStepVal when send msg successful */
/********************************************************************/


#include <string.h>
#include <gpio.h>
#include "board.h"
#include "chip.h"

#include "encoder.h"
#include "IpcMsg.h"
#include "IpcTask.h"
#include "TaskCommon.h"
#include "EncoderInterface.h"

#define MAX_STEP	255

static uint8_t gs_stepVal = 0;

static uint8_t gs_oldStepVal = 0;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoder_Init										                */
/*                                                                                      */
/* Details:         Config GPIO port for encoder					    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data : None				                           						*/
/*                                                                                      */
/****************************************************************************************/
void encoder_Init(void) {

	/* Configure GPIO pin as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, ENCODER_PA_PORT_NUM, ENCODER_PA_BIT_NUM);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, ENCODER_PB_PORT_NUM, ENCODER_PB_BIT_NUM);

	/* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SCU_GPIOIntPinSel(ENCODER_GPIO_INT_INDEX, ENCODER_PA_PORT_NUM, ENCODER_PA_BIT_NUM);

	/* Configure channel interrupt as edge sensitive and falling + rising edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(ENCODER_GPIO_INT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(ENCODER_GPIO_INT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(ENCODER_GPIO_INT_INDEX)); //falling edge
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(ENCODER_GPIO_INT_INDEX)); //rising edge

	//Set priority
	NVIC_SetPriority(ENCODER_PININT_NVIC_NAME,ENCODER_INT_PRIO);

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(ENCODER_PININT_NVIC_NAME);
	NVIC_EnableIRQ(ENCODER_PININT_NVIC_NAME);
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	PININT_IRQ_HANDLER									                */
/*                                                                                      */
/* Details:         Handle encoder PB GPIO interrupt				    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data : 	uint8_t gs_stepVal			                           			*/
/* Local variable :  	uint8_t phaseBVal												*/
/*                                                                                      */
/****************************************************************************************/
void ENCODER_IRQ_HANDLER(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(ENCODER_GPIO_INT_INDEX));
	uint8_t phaseAVal = 0;
	uint8_t phaseBVal = 0;
	static uint8_t s_previousVal;
	phaseAVal = Chip_GPIO_GetPinState(LPC_GPIO_PORT, ENCODER_PA_PORT_NUM, ENCODER_PA_BIT_NUM);
	phaseBVal = Chip_GPIO_GetPinState(LPC_GPIO_PORT, ENCODER_PB_PORT_NUM, ENCODER_PB_BIT_NUM);
	uint8_t directionVal = 0;
	directionVal = (phaseBVal << 1)|phaseAVal;
	if (2 == directionVal) // right
	{
		if (1 == s_previousVal)
		{
			if(gs_stepVal < MAX_STEP)
			{
				gs_stepVal++;
			}
			else
			{
				gs_stepVal = 0;
			}
		}
	}
	else if (0 == directionVal) // left
	{
		if (3 == s_previousVal)
		{
			if(gs_stepVal > 0)
			{
				gs_stepVal--;
			}
			else
			{
				gs_stepVal = MAX_STEP;
			}
		}
	}
	s_previousVal = directionVal;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoder_Handle										                */
/*                                                                                      */
/* Details:         handle encoder step change and send to GUI		    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data : 	(I)	 uint8_t gs_stepVal			                           		*/
/*						(I)(O) uint8_t gs_oldStepVal									*/
/* Local variable :  	IPC_MSG_DATA_t data 											*/
/*                                                                                      */
/****************************************************************************************/
void encoder_Handle(void)
{
	int8_t changeStep = 0;
	uint8_t curStepVal = gs_stepVal;
	if(curStepVal != gs_oldStepVal)
	{
		if(curStepVal > gs_oldStepVal)
		{
			if((curStepVal - gs_oldStepVal) < ENCODER_BASE_VALUE)
			{
				changeStep = curStepVal - gs_oldStepVal;
			}
			else
			{
				changeStep = curStepVal - gs_oldStepVal - (MAX_STEP+1);
			}
		}
		else
		{
			if((gs_oldStepVal - curStepVal) < ENCODER_BASE_VALUE)
			{
				changeStep = curStepVal - gs_oldStepVal;
			}
			else
			{
				changeStep = curStepVal - gs_oldStepVal + (MAX_STEP+1);
			}
		}
		DEBUGOUT("Current: %d , Old: %d, Change: %d \n", curStepVal, gs_oldStepVal, changeStep);
		if(encoderInterface_GetIsEnable() == true)
		{
			IPC_MSG_DATA_t data;
			data.data_id = eEncoderChangeStep;
			data.data_value = (uint32_t)(changeStep + ENCODER_BASE_VALUE);
			memset(&data.stAlarmStatPacket, 0, sizeof(data.stAlarmStatPacket));
			memset(&data.stPsaParamPacket, 0, sizeof(data.stPsaParamPacket));
			memset(&data.stPsaMonitorPacket, 0, sizeof(data.stPsaMonitorPacket));
			memset(&data.version, 0, sizeof(data.version));

			if (ipc_PushMsg(IPCEX_ID_GUI, data) == QUEUE_INSERT) {
				gs_oldStepVal = curStepVal;
				DEBUG_ENCODER("M0 Push data: %d\n",data.data_id);
			}
			else
			{
				DEBUGOUT("IPC full \n");
			}
		}
		else
		{
			gs_oldStepVal = curStepVal;
		}
	}
	return;
}
