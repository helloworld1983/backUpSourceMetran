#ifndef INC_CONFIRMUPGRADESCREEN_H_
#define INC_CONFIRMUPGRADESCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ConfirmUpgradeScreen.h                           	*/
/*                                                                  */
/* General      :The ConfirmUpgradeScreen screen allows user        */
/*                to confirm upgrade Main-CPU					    */
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

#define BORDER_CONFIRM_UPGRADE_COLOR 		0xECC9B1
#define SIZE_BUFFER_FIRMWARE 		 		10
#define START_SECTOR 				 		14
#define END_SECTOR 					 		14
#define SIZE_FIRST_VERSION_BUFFER 			 7
#define SIZE_LAST_VERSION_BUFFER 			 4
#define DELAY_200MS 						(200/portTICK_PERIOD_MS)

#define START_POSITION_FIRST_VERSION_CONTROL	18
#define END_POSITION_FIRST_VERSION_CONTROL		24
#define START_POSITION_LAST_VERSION_CONTROL		25
#define END_POSITION_LAST_VERSION_CONTROL		28

#define START_POSITION_FIRST_VERSION_BOOT		20
#define END_POSITION_FIRST_VERSION_BOOT			26
#define START_POSITION_LAST_VERSION_BOOT		27
#define END_POSITION_LAST_VERSION_BOOT			30

extern WM_HWIN g_confirmUpgradeScreen;

//Init confirm upgrade screen
void confirmUpgradeScreen_Init(void);
//Show confirm upgrade screen
void confirmUpgradeScreen_Show(void);
//Hide confirm upgrade screen
void confirmUpgradeScreen_Hide(void);

#endif /* INC_CONFIRMUPGRADESCREEN_H_ */
/* end of file */
