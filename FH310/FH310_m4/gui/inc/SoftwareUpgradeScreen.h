#ifndef INC_SOFTWAREUPGRADESCREEN_H_
#define INC_SOFTWAREUPGRADESCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : SoftwareUpgradeScreen.h                           */
/*                                                                  */
/* General      :The SoftwareUpgrade screen allows user             */
/*                to upgrade Main-CPU								*/
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
#include "UsbMgr.h"
#include "DualCoreCommon.h"

#define GUI_DELAY_SOFTWARE_UPGRADE	50

extern WM_HWIN g_softwareUpgradeScreen;

//Init software upgrade screen
void softwareUpgradeScreen_Init(void);
//Show software upgrade screen
void softwareUpgradeScreen_Show(void);
//Hide software upgrade screen
void softwareUpgradeScreen_Hide(void);
//Upgrade firmware
void softwareUpgradeScreen_UpgradeFW(CheckFWFileResult checkResult);
//Hide all item
void softwareUpgradeScreen_HideAllItem(void);
//Show all item
void softwareUpgradeScreen_ShowAllItem(void);
//Check exist boot file
bool softwareUpgradeScreen_GetExistBootFile(void);
//Check exist firmware file
bool softwareUpgradeScreen_GetExistFirmFile(void);

#endif /* INC_SOFTWAREUPGRADESCREEN_H_ */
/* end of file */

