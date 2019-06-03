#ifndef INC_MAINTENANCESCREEN_H_
#define INC_MAINTENANCESCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : MaintenanceScreen.h		                        */
/*                                                                  */
/* General      : Maintenance screen include:						*/
/* 					monitor screen                                  */
/* 					log data screen                                 */
/* 					calendar setting screen                         */
/* 					software upgrade screen                         */
/* 					function test screen                            */
/* 					serial number setting screen 	                */
/* 					unit cumulative operation hour screen           */
/* 					other setting screen                            */
/* 					psa parameter change screen                     */
/* 					development setting screen						*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "WM.h"

extern WM_HWIN g_maintenanceScreen;

//Init maintenance screen
void maintenanceScreen_Init(void);
//Show maintenance screen
void maintenanceScreen_Show(void);
//Hide maintenance screen
void maintenanceScreen_Hide(void);

#endif /* INC_MAINTENANCESCREEN_H_ */
/* end of file */
