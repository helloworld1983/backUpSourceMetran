#ifndef INC_PSAPARAMETERCHANGEPAGETWOSCREEN_H_
#define INC_PSAPARAMETERCHANGEPAGETWOSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PSAParameterChangePageTwoScreen.h                 */
/*                                                                  */
/* General       : The PSAParameterChangePageTwo screen allows user */
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

#define BUFFER_VALUE_PSA_PAGE_TWO	 			   10
#define PSA_VALUE_SCALE_PAGE_TWO				   10
#define FC_OUTPUT_SCALE_PAGE_TWO				   0.2

extern WM_HWIN g_PSAParameterChangePageTwoScreen;

//Init PSA parameter change screen
void PSAParameterChangePageTwoScreen_Init(void);
//Show PSA parameter change screen
void PSAParameterChangePageTwoScreen_Show(void);
//Hide PSA parameter change screen
void PSAParameterChangePageTwoScreen_Hide(void);

#endif /* INC_PSAPARAMETERCHANGEPAGETWOSCREEN_H_ */
/* end of file */
