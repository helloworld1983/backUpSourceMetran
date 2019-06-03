/********************************************************************/
/*                                                                  */
/* File Name    : SPIMrg.c                                    		*/
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
/* #0001	   2017/10/29	  Quyen Ngo(MV)			modify			*/
/*		-Replace disable_irq when access SPI by vtaskSuspend for 	*/
/*		avoid problem that the audio I2S interrupt is delayed		*/
/*		(Bug 1997)													*/
/********************************************************************/

#include  "SPIMgr.h"
#include "board.h"
#include "TaskCommon.h"

extern SPIFI_HANDLE_T *pSpifi;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	spiMgr_spifiErase			                                        */
/*                                                                                      */
/* Details		: 	Erase multiple blocks												*/
/*                                                                                      */
/* Arguments 	: 	(I)  SPIFI_HANDLE_T *pHandle                               			*/
/*					(I)	 uint32_t firstBlock											*/
/*					(I)	 uint32_t numBlocks												*/
/* 																						*/
/*  ReturnValue :	SPIFI_ERR_T															*/
/*                                                                                      */
/* Using Global Data : (I) None							                              	*/
/*                                                                                      */
/****************************************************************************************/
SPIFI_ERR_T spiMgr_spifiErase(SPIFI_HANDLE_T *pHandle, uint32_t firstBlock, uint32_t numBlocks)
{
	vTaskSuspendAll();
	SPIFI_ERR_T er = spifiErase(pHandle, firstBlock, numBlocks);
	spifiDevSetMemMode(pSpifi, true);
	xTaskResumeAll();
	return er;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	spiMgr_spifiRead			                                        */
/*                                                                                      */
/* Details		: 	Read the device into the passed buffer								*/
/*                                                                                      */
/* Arguments 	: 	(I)  SPIFI_HANDLE_T *pHandle                               			*/
/*					(I)	 uint32_t addr													*/
/*					(I)	 uint32_t bytes													*/
/*					(O)	 uint32_t *readBuff												*/
/* 																						*/
/*  ReturnValue :	SPIFI_ERR_T															*/
/*                                                                                      */
/* Using Global Data : (I) None							                              	*/
/*                                                                                      */
/****************************************************************************************/
SPIFI_ERR_T spiMgr_spifiRead(SPIFI_HANDLE_T *pHandle, uint32_t addr, uint32_t *readBuff, uint32_t bytes)
{
	vTaskSuspendAll();
	SPIFI_ERR_T er = spifiRead(pHandle, addr, readBuff, bytes);
	spifiDevSetMemMode(pSpifi, true);
	xTaskResumeAll();
	return er;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	spiMgr_spifiProgram			                                        */
/*                                                                                      */
/* Details		: 	Read the device into the passed buffer								*/
/*                                                                                      */
/* Arguments 	: 	(I)  SPIFI_HANDLE_T *pHandle                               			*/
/*					(I)	 uint32_t addr													*/
/*					(I)	 uint32_t bytes													*/
/*					(I)	 uint32_t *writeBuff											*/
/* 																						*/
/*  ReturnValue :	SPIFI_ERR_T															*/
/*                                                                                      */
/* Using Global Data : (I) None							                              	*/
/*                                                                                      */
/****************************************************************************************/
SPIFI_ERR_T spiMgr_spifiProgram(SPIFI_HANDLE_T *pHandle, uint32_t addr, const uint32_t *writeBuff, uint32_t bytes)
{
	vTaskSuspendAll();
	SPIFI_ERR_T er = spifiProgram(pHandle, addr, writeBuff, bytes);
	spifiDevSetMemMode(pSpifi, true);
	xTaskResumeAll();
	return er;
}
