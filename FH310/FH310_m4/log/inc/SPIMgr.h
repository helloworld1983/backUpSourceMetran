#ifndef INC_SPIMGR_H_
#define INC_SPIMGR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : SPIMrg.h                                    		*/
/*                                                                  */
/* General      : manager for SPI read Write	    				*/
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/20/10     Linh Nguyen(MV)  	    new file        */
/*                                                                  */
/********************************************************************/
#include "board.h"
#include "spifilib_dev.h"
#include "spifilib_api.h"


SPIFI_ERR_T spiMgr_spifiErase(SPIFI_HANDLE_T *pHandle, uint32_t firstBlock, uint32_t numBlocks);

SPIFI_ERR_T spiMgr_spifiRead(SPIFI_HANDLE_T *pHandle, uint32_t addr, uint32_t *readBuff, uint32_t bytes);

SPIFI_ERR_T spiMgr_spifiProgram(SPIFI_HANDLE_T *pHandle, uint32_t addr, const uint32_t *writeBuff, uint32_t bytes);

#endif /* INC_SPIMGR_H_ */
