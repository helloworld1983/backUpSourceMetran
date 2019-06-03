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
/*  #0001	   2016/09/06       Linh Nguyen(MV)	    modify	        */
/*  Add send real data function									    */
/********************************************************************/
#include "CBX02Handler.h"
#include "BluetoothInterface.h"
#include "TaskCommon.h"

extern xQueueHandle g_RealDataQueue;

/****************************************************************************************/
/*                                                                                      */
/* Function name: BTInterface_CopyLogToEXTRam            		                  		*/
/*                                                                                      */
/* Details:  the interface for another module can copy data to external ram				*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                        	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) uint16_t dest				                              	*/
/*                     (I) FH310_LOG_T *source                            			   	*/
/*                     (I) uint16_t NoLog			                                   	*/
/*                     (I) LogType logType			                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void BTInterface_CopyLogToEXTRam(uint16_t dest, FH310_LOG_T *source, uint16_t NoLog, LogType logType)
{
	vTaskSuspendAll();
	cbx_CopyLogToEXTRam(dest, source, NoLog, logType);
	xTaskResumeAll();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: BTInterface_sendRealdata				 		                  		*/
/*                                                                                      */
/* Details:  the interface for another module can copy data to external ram				*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) MONITOR_DATA_t monitorData				  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                        	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_RealDataQueue				                              	*/
/*                                                                                      */
/****************************************************************************************/
void BTInterface_sendRealdata(MONITOR_DATA_t *monitorData)
{
	if(!xQueueSend(g_RealDataQueue, monitorData,QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within 500ms\n");
	}
}
