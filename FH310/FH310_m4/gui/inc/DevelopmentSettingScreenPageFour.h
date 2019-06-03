#ifndef INC_DEVELOPMENTSETTINGSCREENPAGEFOUR_H_
#define INC_DEVELOPMENTSETTINGSCREENPAGEFOUR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageFour.h              	*/
/*                                                                  */
/* General       : The user is able to dump file into USB memory	*/
/*  																*/
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

#define GUI_DELAY_DEVELOP_PAGE_FOUR					50
#define BASIS										2
#define BIT_0										0
#define BIT_1										1
#define BIT_2										2
#define BIT_3										3
#define BIT_4										4
#define BIT_5										5
#define BIT_6										6
#define BIT_7										7
#define VALUE_0										0
#define VALUE_1										1

extern WM_HWIN g_developmentSettingScreenPageFour;

//Init development setting screen page four
void developmentSettingScreenPageFour_Init(void);
//Show development setting screen page four
void developmentSettingScreenPageFour_Show(void);
//Hide development setting screen page four
void developmentSettingScreenPageFour_Hide(void);
//Upload file to USB
void developmentSettingScreenPageFour_UploadToUSB(void);
//Close pop up
void developmentSettingScreenPageFour_ClosePopUp(bool saveResult);

#endif /* INC_DEVELOPMENTSETTINGSCREENPAGEFOUR_H_ */
/* end of file */
