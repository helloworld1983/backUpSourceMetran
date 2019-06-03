/********************************************************************/
/*                                                                  */
/* File Name    : SPIMrg.h                                    		*/
/*                                                                  */
/* General      : manager for SPI read Write	    				*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/20/10     Linh Nguyen(MV)  	    new file        */
/*                                                                  */
/********************************************************************/

#ifndef DEVICE_INC_SPIFMGR_H_
#define DEVICE_INC_SPIFMGR_H_

#include "board.h"
#include "spifilib_dev.h"
#include "spifilib_api.h"


SPIFI_ERR_T spifMgr_spifiErase(SPIFI_HANDLE_T *pHandle, uint32_t firstBlock, uint32_t numBlocks);

SPIFI_ERR_T spifMgr_spifiRead(SPIFI_HANDLE_T *pHandle, uint32_t addr, uint32_t *readBuff, uint32_t bytes);

SPIFI_ERR_T spifMgr_spifiProgram(SPIFI_HANDLE_T *pHandle, uint32_t addr, const uint32_t *writeBuff, uint32_t bytes);

bool spifMgr_unlock();
#endif /* DEVICE_INC_SPIFMGR_H_ */
