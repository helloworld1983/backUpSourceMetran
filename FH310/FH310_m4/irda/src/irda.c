/********************************************************************/
/*                                                                  */
/* File Name    : irda.h                                    		*/
/*                                                                  */
/* General       : Decode the IRDA remote signal using time2        */
/* 					capture input                                   */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/11      Quyen Ngo		  	new file        */
/* #0001       2017/09/27	   Quyen Ngo			modify			*/
/* 		remove definition not use									*/
/*                                                                  */
/********************************************************************/


#include <irda.h>
#include <TaskCommon.h>
#include "board.h"
#include "timers.h"
#include "GuiTask.h"
#include "GuiInterface.h"

#ifdef DEBUG
#define Q_DEBUG
#endif

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

#define IRDA_SYSTEM_CLOCK 204000

#define IRDA_MAX_BIT_INDEX 32

#define BIT1_TIME_MAX     2.46*IRDA_SYSTEM_CLOCK
#define BIT1_TIME_MIN     1.7*IRDA_SYSTEM_CLOCK
#define BIT0_TIME_MAX     1.5*IRDA_SYSTEM_CLOCK
#define BIT0_TIME_MIN     0.7*IRDA_SYSTEM_CLOCK
#define LEADER_TIME_MIN   8.0*IRDA_SYSTEM_CLOCK
#define LEADER_TIME_MAX   15.0*IRDA_SYSTEM_CLOCK

#define START_BIT_TIME_MIN 100*IRDA_SYSTEM_CLOCK
#define BIT_MIN	0.2*IRDA_SYSTEM_CLOCK

#define DETECT_LEADER (s_bitTime >= LEADER_TIME_MIN) && (s_bitTime <= LEADER_TIME_MAX)

//IRDA CODE
#define TURN_ON 		0xE702F70A
#define TURN_OFF 		0xE702F712
#define TURN_UP 		0xE702F71A
#define TURN_DOWN 		0xE702F722
#define CONFIRM 		0xE702F72A
#define PRESET1 		0xE702F732
#define PRESET2 		0xE702F73A
#define PRESET3 		0xE702F742

/* decode mode */
#define MODE_WAIT_LEADER 0
#define MODE_DECODING    1

/* task wait flag */
#define irda_SetWaitTask() s_isWaitTask=true
#define irda_ClearWaitTask() s_isWaitTask=false
#define irda_IsWaitTask() s_isWaitTask

/* handle s_irdaData */
#define irda_ReadIrdaData s_irdaData
#define irda_WriteIrdaData(data) s_irdaData=data

static uint32_t s_irdaData = 0x00000000;

static uint32_t s_isWaitTask=false;

/****************************************************************************************/
/*                                                                                      */
/* General Intit and config timer 0 capture 3 for catching IRDA signal				    */
/*                                                                                      */
/* Details  -Set up timer 0 interrupt                 									*/
/*			-Set up timer 0 capture input P1.17											*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
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
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void irda_InitTimer(void)
{
	NVIC_DisableIRQ(TIMER0_IRQn);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);

	Chip_TIMER_Reset(LPC_TIMER0); //timer0 disable and reset
	Chip_Clock_Enable(CLK_MX_TIMER0);

	Chip_TIMER_Select_Capture(LPC_GIMA,0,3);

	/*Set the falling edge capture interrupt of timer0*/
	Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER0, 3);
	/*Enable the  capture interrupt of timer0*/
	Chip_TIMER_CaptureEnableInt(LPC_TIMER0, 3);
	Chip_TIMER_Enable(LPC_TIMER0);
	NVIC_SetPriority(TIMER0_IRQn, IRDA_IRQ_PRIORITY);
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Handle interrupt of timer 0				                                    */
/*                                                                                      */
/* Details  -Read capture value               											*/
/*			-Calculate bit time															*/
/* 			-Shift bit to IRDA command data												*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) 	s_irdaData 													*/
/* 					   (I/O) s_isWaitTask			 						                */
/* local valiables   : long taskWoken			                                        */
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/

void TIMER0_IRQHandler( void )
{
	long taskWoken = 0;
	static uint32_t s_mode=MODE_WAIT_LEADER;
	static uint32_t s_irdata=0x00;
	static uint8_t s_bitIndex = IRDA_MAX_BIT_INDEX; //0 -> 31
	static uint32_t s_previousTC = 0;
	static uint32_t s_bitTime = 0;
	uint32_t currentTC = Chip_TIMER_ReadCapture(LPC_TIMER0,3);

	/**	calculate time from falling edge to falling edge. */
	if ((currentTC - s_previousTC) > 0)
	{
		s_bitTime = currentTC - s_previousTC;
	}
	else
	{
		s_bitTime = 0xFFFFFFFF + currentTC - s_previousTC;
	}

	switch(s_mode){
	case MODE_DECODING:	/* under decoding */
		if ((s_bitTime >= BIT0_TIME_MIN) && (s_bitTime <= BIT0_TIME_MAX))
		{
			s_bitIndex --;
			s_irdata|= 0 << s_bitIndex; /* Low bot */
		}
		else if ((s_bitTime >= BIT1_TIME_MIN) && (s_bitTime <= BIT1_TIME_MAX))
		{
			s_bitIndex --;
			s_irdata|= 1 << s_bitIndex;	/*High bit */
		}else if (s_bitTime > START_BIT_TIME_MIN)
		{
			s_mode=MODE_WAIT_LEADER;/* time is expired */
		}else if(s_bitTime < BIT_MIN)
		{
			s_mode=MODE_WAIT_LEADER; /* less than minmum bit --> noise */
		}else if (DETECT_LEADER)	/* Leader code */
		{
			if(s_bitIndex!=16)
			{		/* There are leader code between first command and second command */
				/* start decoding */
				s_bitIndex = IRDA_MAX_BIT_INDEX;
				s_mode=MODE_DECODING;
				s_irdata=0x00;
				irda_WriteIrdaData(0x00000000);
			}
		}
		if(s_bitIndex==0)
		{
			if(!irda_IsWaitTask())
			{
				/* All data is received */
				irda_SetWaitTask();
				irda_WriteIrdaData(s_irdata);
				s_mode=MODE_WAIT_LEADER;
				xSemaphoreGiveFromISR(g_irdaBinarySem,&taskWoken);
				if(taskWoken)
				{
					portYIELD_FROM_ISR(taskWoken);
				}
			}else{
				s_mode=MODE_WAIT_LEADER;/* if new command is received before irda_task is executed, new command is not used.*/
			}
		}
		break;
	case MODE_WAIT_LEADER:  /* waiting leader code */
		if (DETECT_LEADER)	/* All data is received */
		{
			/* start decoding */
			s_bitIndex = IRDA_MAX_BIT_INDEX;
			s_mode=MODE_DECODING;
			s_irdata=0x00;
			irda_WriteIrdaData(0x00000000);
		}
		else
		{
			s_mode=MODE_WAIT_LEADER;
		}
		break;
	default:
		break;
	}
	s_previousTC = currentTC;
	Chip_TIMER_ClearCapture(LPC_TIMER0, 3);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name irda_Handle						                                    */
/*                                                                                      */
/* Details  Decode Irda command and send to GUI task									*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : uint32_t gs_irdaData												*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void irda_Handle(void)
{
	GUI_EVENT_STRUCT event;
	switch(irda_ReadIrdaData)
	{
	case TURN_ON:
#ifdef Q_DEBUG
		DEBUGOUT("TURN_ON \n");
#endif
		event.id = eIrdaOnEventId;
		event.data.charData = E_TURN_ON_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case TURN_OFF:
#ifdef Q_DEBUG
		DEBUGOUT("TURN_OFF \n");
#endif
		event.id = eIrdaOffEventId;
		event.data.charData = E_TURN_OFF_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case TURN_UP:
#ifdef Q_DEBUG
		DEBUGOUT("TURN_UP \n");
#endif
		event.id = eIrdaUpEventId;
		event.data.charData = E_TURN_UP_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case TURN_DOWN:
#ifdef Q_DEBUG
		DEBUGOUT("TURN_DOWN \n");
#endif
		event.id = eIrdaDownEventId;
		event.data.charData = E_TURN_DOWN_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case CONFIRM:
#ifdef Q_DEBUG
		DEBUGOUT("CONFIRM \n");
#endif
		event.id = eIrdaConfirmEventId;
		event.data.charData = E_CONFIRM_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case PRESET1:
#ifdef Q_DEBUG
		DEBUGOUT("PRESET1 \n");
#endif
		event.id = eIrdaPreset1EventId;
		event.data.charData = E_PRESET1_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case PRESET2:
#ifdef Q_DEBUG
		DEBUGOUT("PRESET2 \n");
#endif
		event.id = eIrdaPreset2EventId;
		event.data.charData = E_PRESET2_ID;
		guiInterface_SendGuiEvent(event);
		break;
	case PRESET3:
#ifdef Q_DEBUG
		DEBUGOUT("PRESET3 \n");
#endif
		event.id = eIrdaPreset3EventId;
		event.data.charData = E_PRESET3_ID;
		guiInterface_SendGuiEvent(event);
		break;
	default:
		break;
	}
	irda_ClearWaitTask(); /* a wait task flag is cleared */
	return;
}
