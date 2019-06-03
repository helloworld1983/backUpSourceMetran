#ifndef INC_BLUETOOTHTASK_H_
#define INC_BLUETOOTHTASK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : BluetoothTask.h                        			*/
/*                                                                  */
/* General      : Manager of bluetooth task							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/01/8       Linh Nguyen	  	    new file        */
/* #0001       2016/09/05     Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode:								*/ 
/*	- Rename BTEvent struct											*/
/*	- Add new struct BT_MSG_t										*/
/*	- Add BTTask_SendMgs function									*/
/* #0002       2017/09/20     Linh Nguyen	  	    modify          */
/*  Remane some ID of BTEventId										*/ 
/********************************************************************/
#include "LogInterface.h"

typedef enum
{
	eUpdateCompositionId,
	eDeleteCompositionId,
	eOnBluetoothId,
	eOffBluetoothId,
	eBeginGettingSpO2PulseDataId,
	eEndGettingSpO2PulseDataId,
	eOnBluetoothTranferId,
	eOffBluetoothTranferId
}BTEventId;

typedef struct
{
	BTEventId id; //bt event ID
	NUM_OF_LOG_t mgs; //Bt message
}BT_MSG_t;

void BTTask_CreateBTTask(void);
void BTTask_InitBTTask(void);
bool BTTask_SendEvent(BTEventId event);
bool BTTask_SendMgs(BT_MSG_t mgs);
#endif /* INC_BLUETOOTHTASK_H_ */
