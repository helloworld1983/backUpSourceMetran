/********************************************************************/
/*                                                                  */
/* File Name    : Key.h	  		                                    */
/*                                                                  */
/* General       : This module contain function for checking current*/
/*                 state of the button						        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#ifndef INC_KEY_H_
#define INC_KEY_H_
#include "board.h"
bool Key_isPowerButtonPressed();
void Key_setPowerOn();
bool Key_isPowerOn();




#endif /* INC_KEY_H_ */
