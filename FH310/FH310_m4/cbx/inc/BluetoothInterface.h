#ifndef INC_BLUETOOTHINTERFACE_H_
#define INC_BLUETOOTHINTERFACE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : BluetoothInterface.h                    			*/
/*                                                                  */
/* General  : Contains functions for interface with Bluetooth module*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*        	   2016/09/06       Linh Nguyen(MV)	    new file        */
/*  #0001	   2017/09/20       Linh Nguyen(MV)	    modify	        */
/*  Add send real data function									    */
/********************************************************************/
#include "LogInterface.h"
#include "BluetoothTask.h"

void BTInterface_CopyLogToEXTRam(uint16_t dest, FH310_LOG_T *source, uint16_t NoLog, LogType logType);
void BTInterface_sendRealdata(MONITOR_DATA_t *monitorData);

#endif /* INC_BLUETOOTHINTERFACE_H_ */
