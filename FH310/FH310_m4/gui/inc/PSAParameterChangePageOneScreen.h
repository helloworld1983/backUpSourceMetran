#ifndef INC_PSAPARAMETERCHANGEPAGEONESCREEN_H_
#define INC_PSAPARAMETERCHANGEPAGEONESCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PSAParameterChangePageOneScreen.h                 */
/*                                                                  */
/* General       : The PSAParameterChangePageOne screen allows user */
/*                 to change psa parameter                          */
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

#define BUFFER_SIZE_VALUE_PSA_PAGE_ONE       5

extern WM_HWIN g_PSAParameterChangePageOneScreen;

//Init PSA parameter change screen
void PSAParameterChangePageOneScreen_Init(void);
//Show PSA parameter change screen
void PSAParameterChangePageOneScreen_Show(void);
//Hide PSA parameter change screen
void PSAParameterChangePageOneScreen_Hide(void);
//Show all item
void PSAParameterChangePageOneScreen_ShowAllChild(void);
//hide all item
void PSAParameterChangePageOneScreen_HideAllChild(void);
//Get condition temperature
int PSAParameterChangePageOneScreen_GetConditionTemp(void);
//Get condition O2
int PSAParameterChangePageOneScreen_GetConditionO2(void);

#endif /* INC_PSAPARAMETERCHANGEPAGEONESCREEN_H_ */
/* end of file */
