#ifndef INC_ANYPALINTERFACE_H_
#define INC_ANYPALINTERFACE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : AnypalInterface.h                       			*/
/*                                                                  */
/* General     : Contains functions for interface with Anypal module*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* 		       2017/07/20      Quyen Ngo(MV)  	    new file        */
/* #0001       2017/09/12       Linh Nguyen	  	    modify          */
/*  Add eAnypalPushOut event								     	*/
/* #0002       2017/09/19       Linh Nguyen	  	    modify          */
/*  Rename a anypal event id and add one					     	*/
/* #0003       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/*  Remove un use code										     	*/
/********************************************************************/
#include <lpc_types.h>

#include "UsbCommonDef.h"
#include "TaskCommon.h"

typedef enum
{
	eNoneEvent,
	eAnypalOverCurrentEvent,
	eAnypalOverCurrentClear,
	eAnypalPushOut,
	eResetAllAnypalState,
    eCalendarUpdate,
	eAnypalEnumComplete
}AnypalEvent;


//Define Anypal message data
typedef union
{
	USB_ClassInfo_FTDI_Host_t hDiskInterface;
}ANYPAL_MSG_DATA;

//Define packet send to GUI
typedef struct{
	AnypalEvent id;//Id packet
	ANYPAL_MSG_DATA data;//Data send
}ANYPAL_MSG_STRUCT_t;

bool anypalInterface_SendEvent(AnypalEvent event);


bool anypalInterface_SendMsg(ANYPAL_MSG_STRUCT_t msg);


#endif /* INC_ANYPALINTERFACE_H_ */
