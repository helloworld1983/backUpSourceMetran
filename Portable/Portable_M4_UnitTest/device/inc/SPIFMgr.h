
#ifndef DEVICE_INC_SPIFMGR_H_
#define DEVICE_INC_SPIFMGR_H_
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
/*             2016/20/10     Thao Ha(MV)  	    new file        */
/*                                                                  */
/********************************************************************/

#include "board.h"
#include "spifilib_dev.h"
#include "spifilib_api.h"

/****************************************************************************************/
/*                                                                                      */
/* Function name:   spiMgr_spifiErase                                                   */
/*                                                                                      */
/* Details      :   Erase multiple blocks                                               */
/*                                                                                      */
/* Arguments    :   (I)  SPIFI_HANDLE_T *pHandle                                        */
/*                  (I)  uint32_t firstBlock                                            */
/*                  (I)  uint32_t numBlocks                                             */
/*                                                                                      */
/*  ReturnValue :   SPIFI_ERR_T                                                         */
/*                                                                                      */
/* Using Global Data : (I) None                                                         */
/*                                                                                      */
/****************************************************************************************/
SPIFI_ERR_T spifMgr_spifiErase(SPIFI_HANDLE_T *pHandle, uint32_t firstBlock, uint32_t numBlocks);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   spiMgr_spifiRead                                                    */
/*                                                                                      */
/* Details      :   Read the device into the passed buffer                              */
/*                                                                                      */
/* Arguments    :   (I)  SPIFI_HANDLE_T *pHandle                                        */
/*                  (I)  uint32_t addr                                                  */
/*                  (I)  uint32_t bytes                                                 */
/*                  (O)  uint32_t *readBuff                                             */
/*                                                                                      */
/*  ReturnValue :   SPIFI_ERR_T                                                         */
/*                                                                                      */
/* Using Global Data : (I) None                                                         */
/*                                                                                      */
/****************************************************************************************/
SPIFI_ERR_T spifMgr_spifiRead(SPIFI_HANDLE_T *pHandle, uint32_t addr, uint32_t *readBuff, uint32_t bytes);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   spiMgr_spifiProgram                                                 */
/*                                                                                      */
/* Details      :   Read the device into the passed buffer                              */
/*                                                                                      */
/* Arguments    :   (I)  SPIFI_HANDLE_T *pHandle                                        */
/*                  (I)  uint32_t addr                                                  */
/*                  (I)  uint32_t bytes                                                 */
/*                  (I)  uint32_t *writeBuff                                            */
/*                                                                                      */
/*  ReturnValue :   SPIFI_ERR_T                                                         */
/*                                                                                      */
/* Using Global Data : (I) None                                                         */
/*                                                                                      */
/****************************************************************************************/
SPIFI_ERR_T spifMgr_spifiProgram(SPIFI_HANDLE_T *pHandle, uint32_t addr, const uint32_t *writeBuff, uint32_t bytes);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   spifMgr_unlock                                                      */
/*                                                                                      */
/* Details      :   Unlock SPIF                                                         */
/*                                                                                      */
/* Arguments    :   None                                                                */
/*                                                                                      */
/*  ReturnValue :   bool                                                                */
/*                                                                                      */
/****************************************************************************************/
bool spifMgr_unlock();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   spifMgr_ComputeChecksumSPIF                                         */
/*                                                                                      */
/* Details      :   Unlock SPIF                                                         */
/*                                                                                      */
/* Arguments    :   uint32_t baseAddr                                                   */
/*                  uint32_t nBytes                                                     */
/*                                                                                      */
/*  ReturnValue :   uint32_t                                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t spifMgr_ComputeChecksumSPIF(uint32_t baseAddr, uint32_t nBytes);

#endif /* DEVICE_INC_SPIFMGR_H_ */
