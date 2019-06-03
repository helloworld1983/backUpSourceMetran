/********************************************************************/
/*                                                                  */
/* File Name    : AnypalInterface.h                        			*/
/*                                                                  */
/* General   : Contains functions for interface with Anypal module	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*        	   2016/07/20       Quyen Ngo(MV)  	    new file        */
/* #0001       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/********************************************************************/

#include "AnypalInterface.h"
#include "AnypalHandler.h"


QueueHandle_t gs_anypalQueue;

/****************************************************************************************/
/*                                                                                      */
/* General 	-ftdiAnypal_SendEvent													    */
/*                                                                                      */
/* Details  -Send Event																	*/
/*                                                                                      */
/* Arguments : (I) AnypalEvent event	 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_anypalQueue				                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
bool anypalInterface_SendEvent(AnypalEvent event)
{
	ANYPAL_MSG_STRUCT_t msg;
	msg.id = event;
	if(xQueueSend(gs_anypalQueue, &msg,QUEUE_WAITING) == pdFALSE)
	{
		DEBUGOUT("\n Failure send anypal event");
		return false;
	}
	return true;
}

//Q_add
/****************************************************************************************/
/*                                                                                      */
/* General 	-fidiAnypal_SendMsg													    	*/
/*                                                                                      */
/* Details  -Send Event																	*/
/*                                                                                      */
/* Arguments : (I) ANYPAL_EVENT_STRUCT_t msg	 					  					*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
bool anypalInterface_SendMsg(ANYPAL_MSG_STRUCT_t msg)
{
	if(xQueueSend(gs_anypalQueue, &msg,QUEUE_WAITING) == pdFALSE)
	{
		DEBUGOUT("\n Failure send anypal msg");
		return false;
	}
	return true;
}
//end Q_add
