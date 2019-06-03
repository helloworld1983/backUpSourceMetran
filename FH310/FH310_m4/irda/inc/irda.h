#ifndef INC_IRDA_H_
#define INC_IRDA_H_

/********************************************************************/
/*                                                                  */
/* File Name    : irda.h                                    		*/
/*                                                                  */
/* General       : Decode the IRDA remote signal using time0        */
/* 					capture input                                   */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/11      Quyen Ngo		  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include <stdint.h>

void irda_InitTimer(void);

void irda_Handle(void);

void irda_ClearWaitTask(void);

#endif /* INC_IRDA_H_ */
