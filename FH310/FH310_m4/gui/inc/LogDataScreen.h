#ifndef LOGDATASCREEN_H_
#define LOGDATASCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : LogDataScreen.h                       			*/
/*                                                                  */
/* General       : Display log data information	                    */
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

#include "board.h"
#include "WM.h"
#include "LogMgr.h"

#define HEADER_COLOR 			0xFFD7C2
#define GUI_DELAY_LOG_DATA		5
#define TICK_WAIT_LOG_DATA		500
#define SIZE_BUFFER_NUMBER_LOG	5
#define MAX_ROW_OF_PAGE			10

extern WM_HWIN g_logDataScreen;

//Init log data screen
void logDataScreen_Init(void);
//Show log data screen
void logDataScreen_Show(void);
//Hide log data screen
void logDataScreen_Hide(void);
//Close pop up
void logDataScreen_ClosePopUp(bool saveResult);
//Get string name of log
int8_t* logDataScreen_GetStringLogName(LogId ID);

#endif /* LOGDATASCREEN_H_ */
/* end of file */
