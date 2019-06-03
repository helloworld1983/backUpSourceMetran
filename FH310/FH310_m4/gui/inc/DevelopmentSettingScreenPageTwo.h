#ifndef INC_DEVELOPMENTSETTINGSCREENPAGETWO_H_
#define INC_DEVELOPMENTSETTINGSCREENPAGETWO_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageTwo.h                 */
/*                                                                  */
/* General       : The user is able to test watch dog, flow rate,RTC*/
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

#define DEFAULT_O2_TEST_STR  				((int8_t*)"82")
#define DEFAULT_FLOWRATE_TEST_STR  			((int8_t*)"3.00")

extern WM_HWIN g_developmentSettingScreenPageTwo;

//Init development setting screen page two
void developmentSettingScreenPageTwo_Init(void);
//Show development setting screen page two
void developmentSettingScreenPageTwo_Show(void);
//Hide development setting screen page two
void developmentSettingScreenPageTwo_Hide(void);

#endif /* INC_DEVELOPMENTSETTINGSCREENPAGETWO_H_ */
/* end of file */
