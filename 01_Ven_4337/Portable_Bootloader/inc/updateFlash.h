/********************************************************************/
/*                                                                  */
/* File Name    : updateFlash.c                                		*/
/*                                                                  */
/* General      : Provide function to update firmware files         */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/25       Thao Ha		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#ifndef INC_UPDATEFLASH_H_
#define INC_UPDATEFLASH_H_
#include "board.h"
typedef enum {
	eNoneUpdate,
	eStartUpdate,
	eChecking,
	eUpdating,
	eFinishUpdate
}E_Update_State;

bool update_startUpdate();
void update_runCtrlSW();
E_Update_State update_getCurrentState();
uint8_t update_getUpdatedVersionH(void);
uint8_t update_getUpdatedVersionL(void);

#endif /* INC_UPDATEFLASH_H_ */
