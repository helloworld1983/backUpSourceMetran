#ifndef INC_CUMULATIVEHOURDIALOG_H_
#define INC_CUMULATIVEHOURDIALOG_H_

/********************************************************************/
/*                                                                  */
/* File Name    : CumulativeHourDialog.h                            */
/*                                                                  */
/* General       : unit cumulative operation hour change		 	*/
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

#define BORDER_CUMULATIVE_HOUR_COLOR 0xECC9B1
#define SIZE_BUFFER_CUMULATIVE 		 16
#define SIZE_BUFFER_HOUR   			 6
#define SIZE_BUFFER_NUMBER 			 3

extern WM_HWIN g_cumulativeHourDialog;

//Init cumulative operation hour change dialog
void cumulativeHourDialog_Init(void);
//Show cumulative operation hour change dialog
void cumulativeHourDialog_Show(uint8_t flow);
//Show hour cummulative flow rate
void cumulativeHourDialog_ShowHour(uint8_t flow);
//Hide cumulative operation hour change dialog
void cumulativeHourDialog_Hide(void);

#endif /* INC_CUMULATIVEHOURDIALOG_H_ */
/* end of file */
