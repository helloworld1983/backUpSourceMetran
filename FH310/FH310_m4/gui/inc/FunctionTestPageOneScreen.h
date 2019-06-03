#ifndef INC_FUNCTIONTESTPAGEONESCREEN_H_
#define INC_FUNCTIONTESTPAGEONESCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageOneScreen.h                       */
/*                                                                  */
/* General       : The FunctionTestPageOne screen allows user to    */
/*                 monitor the current system information in        */
/* 				   numerical and graphicaldisplay such as:          */
/* 				   Temperature (Unit internal),                     */
/*   			   Temperature (compressor outlet),                 */
/* 				   Oxygen concentration,			                */
/*                 Measurement flow rate,                           */
/*                 Voltage of Illuminance Sensor,			        */
/*                 Product tank pressure,                           */
/*                 Cannula pressure, 						        */
/*                 Compressor positive pressure,                    */
/*                 Compressor negative pressure,	                */
/*                 External flow path pressure,                     */
/*                 Compressor driver board 			                */
/*                 software version,                                */
/*                 Flow controller software version, 			    */
/*                 Boot loader software version, 					*/
/*                 Backup software software version		            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "WM.h"
#include "TaskCommon.h"
#include "DualCoreCommon.h"

#define FUNCTIONTEST_PAGEONE_X		0
#define FUNCTIONTEST_PAGEONE_Y		0
#define FUNCTIONTEST_PAGEONE_SIZE_X	800
#define FUNCTIONTEST_PAGEONE_SIZE_Y	480
#define SIZE_BUFFER_BT_VERSION    	50

extern WM_HWIN g_functionTestPageOneScreen;

//Init function test page one
void functionTestPageOneScreen_Init(void);
//Show function test page one
void functionTestPageOneScreen_Show(void);
//Hide function test page one
void functionTestPageOneScreen_Hide(void);
//Display version bluetooth
void functionTestPageOneScreen_DisplayBT(BT_FW_VERSION_T BTVersion);
//Update monitor
void functionTestPageOneScreen_UpdateMonitor(IPC_MSG_DATA_t data);
//Display version compressor
void functionTestPageOneScreen_DisplayVersionComp(const int8_t* version);
//Display version flow controller
void functionTestPageOneScreen_DisplayVersionFlow(const int8_t* version);

#endif /* INC_FUNCTIONTESTPAGEONESCREEN_H_ */
/* end of file */
