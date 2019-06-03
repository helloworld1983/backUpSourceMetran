/********************************************************************/
/*                                                                  */
/* File Name    : SPIMrg.c                                    		*/
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
/*             2016/20/10     Thao Ha(MV)	  	    new file        */
/*                                                                  */
/********************************************************************/
#include "SPIFMgr.h"
#include "string.h"
static uint32_t lmem[21];
SPIFI_HANDLE_T *pSpifi;
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
SPIFI_ERR_T spifMgr_spifiErase(SPIFI_HANDLE_T *pHandle, uint32_t firstBlock, uint32_t numBlocks)
{
	__disable_irq();
	SPIFI_ERR_T er = spifiErase(pHandle, firstBlock, numBlocks);
	spifiDevSetMemMode(pSpifi, true);
	__enable_irq();
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
SPIFI_ERR_T spifMgr_spifiRead(SPIFI_HANDLE_T *pHandle, uint32_t addr, uint32_t *readBuff, uint32_t bytes)
{
	__disable_irq();
	SPIFI_ERR_T er = spifiRead(pHandle, addr, readBuff, bytes);
	spifiDevSetMemMode(pSpifi, true);
	__enable_irq();
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
SPIFI_ERR_T spifMgr_spifiProgram(SPIFI_HANDLE_T *pHandle, uint32_t addr, const uint32_t *writeBuff, uint32_t bytes)
{
	__disable_irq();
	SPIFI_ERR_T er = spifiProgram(pHandle, addr, writeBuff, bytes);
	spifiDevSetMemMode(pSpifi, true);
	__enable_irq();
	return er;
}
#define SPIFLASH_BASE_ADDRESS (0x14000000)

STATIC const PINMUX_GRP_T spifipinmuxing[] = {
		{0x3, 3,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI CLK */
		{0x3, 4,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D3 */
		{0x3, 5,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D2 */
		{0x3, 6,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D1 */
		{0x3, 7,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D0 */
		{0x3, 8,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)}	/* SPIFI CS/SSEL */
};
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	initializeSpifi				                                        */
/*                                                                                      */
/* Details		: 	Initialize SPIF Flash												*/
/*                                                                                      */
/* Arguments 	: 	None						                               			*/
/* 																						*/
/*  ReturnValue :	SPIFI_ERR_T															*/
/*                                                                                      */
/****************************************************************************************/
SPIFI_HANDLE_T *initializeSpifi(void)
{
	uint32_t memSize;
	SPIFI_HANDLE_T *pReturnVal;

	/* Initialize LPCSPIFILIB library, reset the interface */
	spifiInit(LPC_SPIFI_BASE, true);

	/* register support for the family(s) we may want to work with
      (only 1 is required) */
	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FLP);
	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FL1);
	spifiRegisterFamily(SPIFI_REG_FAMILY_MacronixMX25L);
//	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FL016K);
//	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FL256SAGNFI000);
//	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FL164K);

	/* Get required memory for detected device, this may vary per device family */
	memSize = spifiGetHandleMemSize(LPC_SPIFI_BASE);
	if (memSize == 0) {
		/* No device detected, error */
		DEBUGOUT("SPIF: spifiGetHandleMemSize (General Err)");
	}

	/* Initialize and detect a device and get device context */
	/* NOTE: Since we don't have malloc enabled we are just supplying
      a chunk of memory that we know is large enough. It would be
      better to use malloc if it is available. */
	pReturnVal = spifiInitDevice(&lmem, sizeof(lmem), LPC_SPIFI_BASE, SPIFLASH_BASE_ADDRESS);
	if (pReturnVal == NULL) {
		DEBUGOUT("SPIF: spifiInitDevice (General Err)");
	}
	return pReturnVal;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	spifMgr_unlock				                                        */
/*                                                                                      */
/* Details		: 	Unlock SPIF															*/
/*                                                                                      */
/* Arguments 	: 	None						                               			*/
/* 																						*/
/*  ReturnValue :	bool																*/
/*                                                                                      */
/****************************************************************************************/
bool spifMgr_unlock()
{
	bool error = false;
	uint32_t spifiBaseClockRate;
	uint32_t maxSpifiClock;


	SPIFI_ERR_T errCode;

	/* Setup SPIFI FLASH pin muxing (QUAD) */
	Chip_SCU_SetPinMuxing(spifipinmuxing, sizeof(spifipinmuxing) / sizeof(PINMUX_GRP_T));
	/* SPIFI base clock will be based on the main PLL rate and a divider */
	spifiBaseClockRate = Chip_Clock_GetClockInputHz(CLKIN_MAINPLL);

	/* Setup SPIFI clock to run around 1Mhz. Use divider E for this, as it allows
        higher divider values up to 256 maximum) */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, ((spifiBaseClockRate / 1000000) + 1));
	Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IDIVE, true, false);

	/* Initialize the spifi library. This registers the device family and detects the part */
	pSpifi = initializeSpifi();

	/* Get some info needed for the application */
	maxSpifiClock = spifiDevGetInfo(pSpifi, SPIFI_INFO_MAXCLOCK);

	/* Setup SPIFI clock to at the maximum interface rate the detected device
        can use. This should be done after device init. */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, ((spifiBaseClockRate / maxSpifiClock) + 1));
	/* start by unlocking the device */
	errCode = spifiDevUnlockDevice(pSpifi);
	if (errCode != SPIFI_ERR_NONE) {
		DEBUGOUT("Unlock device Error:%d %s\r\n",errCode, spifiReturnErrString(errCode));
		error = true;
		return error;
	}
	spifiDevSetMemMode(pSpifi, true);
	return true;
}
