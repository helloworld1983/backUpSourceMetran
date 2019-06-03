#ifndef INC_MONITORSCREEN_H_
#define INC_MONITORSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : MonitorScreen.h                            		*/
/*                                                                  */
/* General       : Displaying data in monitor PSA			 		*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "WM.h"
#include "DualCoreCommon.h"

extern WM_HWIN g_monitorScreen;

//Init monitor PSA screen
void monitorScreen_Init(void);
//Show monitor PSA screen
void monitorScreen_Show(void);
//Hide monitor PSA screen
void monitorScreen_Hide(void);
//Display monitor PSA
void monitorScreen_UpdatePSA(IPC_MSG_DATA_t data);
//Change flow rate in monitor PSA
void monitorScreen_ChangeFlowrate(uint8_t FRPSAValue);

#endif /* INC_MONITORSCREEN_H_ */
/* end of file */

