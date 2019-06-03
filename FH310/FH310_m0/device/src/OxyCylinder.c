/********************************************************************/
/*                                                                  */
/* File Name     : OxyCylinder.c	                               	*/
/*                                                                  */
/* General       : handle mode oxygen cylinder module	            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/16      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*		-remove assert												*/
/* #0002	   2017/09/08	   Quyen Ngo	modify					*/
/*		-add code to detect the abnormal ext flow  pressure case	*/
/* #0003	   2017/11/10	   Quyen Ngo	modify					*/
/*		-Change the moethod to detect Oxyclinder attach when init.	*/
/*		Does not wait for 1 second (Bug 2023)						*/
/********************************************************************/

/* include system files */
/* include user files */
#include <OxyCylinder.h>
#include "TaskCommon.h"

/* constants, macro definition */

#ifdef DEBUG
//#define DEBUG_O2CYLIND_EN
#endif

#ifdef DEBUG_O2CYLIND_EN
#define DEBUG_O2CYLIND(...) printf(__VA_ARGS__)
#else
#define DEBUG_O2CYLIND(...)
#endif

#define O2_CYLIN_ATTACH_PRESS		90
#define O2_CYLIN_REMOVE_PRESS 		80
#define O2_CYLIN_ABNORMAL_PRESS	    165

#define CYLIND_1S_TIME 1000/DEVICE_TASK_DELAY_TIME //1000ms/50ms

/* definitions of structures */

/*global variable*/
static E_O2CylinStatus s_currentState = O2_CYLIN_UNKNOW_STATUS;

static uint16_t s_o2AttachTimeCnt = 0;
static uint16_t s_o2RemoveTimeCnt = 0;
static uint16_t s_o2AbnormalTimeCnt = 0;


/* external declaration */

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: oxyCylinder_Handle
//
//    Detail: Check extFlow pressure and detect whether Oxygen cylinder is attach or not
//
//    Arguments: (I) float extFlowPress
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void oxyCylinder_Handle(float extFlowPress)
{
	switch (s_currentState) {
	case O2_CYLIN_UNKNOW_STATUS:
		if((extFlowPress >= O2_CYLIN_ATTACH_PRESS) && (extFlowPress < O2_CYLIN_ABNORMAL_PRESS))
		{
			s_currentState = O2_CYLIN_ATTACH;
		}
		else
		{
			s_o2AttachTimeCnt = 0;
			s_currentState = O2_CYLIN_NOTATTACH;
		}
		break;
	case O2_CYLIN_ATTACH:
		if(extFlowPress < O2_CYLIN_REMOVE_PRESS)
		{
			s_o2AbnormalTimeCnt = 0;
			s_o2RemoveTimeCnt++;
			if(s_o2RemoveTimeCnt > CYLIND_1S_TIME)
			{
				s_o2RemoveTimeCnt = 0;
				s_currentState = O2_CYLIN_REMOVE;
			}
		}
		else if(extFlowPress > O2_CYLIN_ABNORMAL_PRESS)
		{
			s_o2RemoveTimeCnt = 0;
			s_o2AbnormalTimeCnt++;
			if(s_o2AbnormalTimeCnt > CYLIND_1S_TIME)
			{
				s_o2AbnormalTimeCnt = 0;
				s_currentState = O2_CYLIN_NOTATTACH;
			}
		}
		else
		{
			s_o2AbnormalTimeCnt = 0;
		}
		break;
	case O2_CYLIN_NOTATTACH:
	case O2_CYLIN_REMOVE:
		if((extFlowPress >= O2_CYLIN_ATTACH_PRESS) && (extFlowPress < O2_CYLIN_ABNORMAL_PRESS))
		{
			s_o2AttachTimeCnt++;
			if(s_o2AttachTimeCnt > CYLIND_1S_TIME)
			{
				s_o2AttachTimeCnt = 0;
				s_currentState = O2_CYLIN_ATTACH;
			}
		}
		else
		{
			s_o2AttachTimeCnt = 0;
		}
		break;
	default:
		DEBUGOUT("Invalid O2 cylind state\n");
		break;
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: oxyCylinder_GetStatus
//
//    Detail: Get status oxygen cylinder detect
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: E_O2CylinStatus
//
//
//    Using Global Data : (I) s_currentState
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
E_O2CylinStatus oxyCylinder_GetStatus(void)
{
	DEBUG_O2CYLIND("Cylind detect: %d \n",s_currentState);
	return s_currentState;
}
