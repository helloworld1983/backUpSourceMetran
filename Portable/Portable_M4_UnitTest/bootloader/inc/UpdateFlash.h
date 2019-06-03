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


struct image_sig {
	uint32_t signature;		/* Signature M0/M4 image */
	char build_date[32];	/* Build Date string */
	char build_time[32];	/* Build Time string */
	char version[32];
};

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
bool update_isPowerButtonPressed();
int update_getBootloaderVersion(uint32_t image_addr,uint8_t* versionH,uint8_t* versionL);

#endif /* INC_UPDATEFLASH_H_ */
