
/********************************************************************/
/*                                                                  */
/* File Name    : FH310_m4.c                       			    	*/
/*                                                                  */
/* General       : main definition									*/
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
/*  #0001	   2017/08/24      Linh Nguyen		  	modify		    */
/* 	improve handle update bootloader failure						*/
/*  #0002      2017/08/29      Linh Nguyen		  	modify		    */
/* 	define a function to update bootloader							*/
/*  #0003      2017/09/05      Viet Le		  		modify		    */
/*  remove #include "assert.h"										*/
/*	#0004	   2017/09/27	   Quyen Ngo			modify			*/
/*	init wdt after system init										*/
/*  #0005	   2017/09/24      Linh Nguyen(MV)  	modify		    */
/*  Change function name for coding rule					     	*/
/*  #0006      2017/09/28     Viet Le		  		modify		    */
/*  -Add guiInterface_Init() function in main() function(bug 1923)	*/
/*  -Use true/false for this flag upgrade (bug 1914)				*/
/*  #0007      2017/10/03     Viet Le		  		modify		    */
/*  -Edit EEPROM_ReadChecksum()--> 									*/
/*		guiInterface_GetChecksumOfSettings() to check sum (bug 1925)*/
/*  #0008      2017/10/03     Viet Le		  		modify		    */
/*  -Edit setting_Checkrange()--> 									*/
/*		guiInterface_CheckSettingRange() to check setting range 	*/
/*		(bug 1924)													*/
/*	#0009      2017/10/14	  Quyen Ngo				modify			*/
/*	-Add Feed WDT before and after show wakeup screen because the 	*/
/*	show time is change from 1s->3s									*/
/*  #0010      2017/10/03     Viet Le		  		modify		    */
/*	-Remove this variable spifipinmuxing_main(Coverity)				*/
/*  #0010      2017/10/03     Quyen Ngo		  		modify		    */
/*	-Move Reset external eeprom function after init audio (Bug 2034)*/
/*  #0011      2017/11/22     Quyen Ngo	  			modify		    */
/*	-Change sequence to init audio and reset EEPROM (Bug 2034)		*/
/*  #0012      2017/12/13     Quyen Ngo	  			modify		    */
/*	-Change the return of updateSpi function from 1 to true			*/
/********************************************************************/
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#define LPC_UARTX       			LPC_USART2
#define UPGRADE_WAIT_MS				50

#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0APP) | defined (__MULTICORE_MASTER_SLAVE_M0SUB)
#include "cr_start_m0.h"
#endif

#include <stdio.h>

#include "../ipclib/IpcMsg.h"
#include "GuiTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "AudioMgr.h"
#include "queue.h"
#include <IrdaTask.h>
#include <UsbMgr.h>
#include <EEPROM.h>
#include "nand.h"
#include "setting.h"
#include "PSATableMgr.h"
#include "string.h"
#include "GUI.h"
#include "Lcd.h"
#include "AudioMgr.h"
#include "UsbTask.h"
#include "spifilib_api.h"
#include "CreateContactDialog.h"
#include "ModeMgr.h"
#include <RealTimeClock.h>
#include "LogTask.h"
#include "updateFlash.h"
#include "../anypal/inc/AnypalHandler.h"
#include "MainScreen.h"
#include "updateFlash.h"
#include "BluetoothTask.h"
#include "USBHostTask.h"
#include "GuiHandler.h"
#include "../anypal/inc/FtdiAnypalHost.h"
#include "Font.h"
#include "ExternalEeprom.h"
#include "GuiInterface.h"
#include "I2CInterface.h"
#include "Delay.h"
#include "PowerHandle.h"
#include "lpc_swim_font.h"
#include "lpc_rom8x16.h"
#include "WdtHandle.h"

// TODO: insert other definitions and declarations here
#define TICKRATE_HZ 8000
#define SPIFLASH_BASE_ADDRESS 0x14000000
static uint32_t lmem[21];

__BSS(RAM5) SWIM_WINDOW_T upgradeScreen;

SPIFI_HANDLE_T *pSpifi;

static bool s_updateResult = true;

/****************************************************************************************/
/*                                                                                      */
/* General 	- GetUpdateResult							                                */
/*                                                                                      */
/* Details  -	Get Update Result											      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : 						                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_updateResult : 									        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool GetUpdateResult(void)
{
	return s_updateResult;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- SetUpdateResult							                                */
/*                                                                                      */
/* Details  -	Set Update Result											      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool value												            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE							                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) s_updateResult   									        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
void SetUpdateResult(bool value)
{
	s_updateResult = value;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-system_init													    		*/
/*                                                                                      */
/* Details  -Initialize all device in FH310			    				        		*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void system_init(void) {
	SystemCoreClockUpdate();
	Board_Init();
	wdt_InitWDT();
	// Q_add USB manager
	usb_hw_setup();

	//EEPROM
	EEPROM_Init();
	lcd_Init();
	lcd_TouchInit();
	logTask_InitLogTask();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-main_tasks													    			*/
/*                                                                                      */
/* Details  -Create all task in FH310			    				        			*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void main_tasks(void)
{
	// Start M0APP slave processor
#if defined (__MULTICORE_MASTER_SLAVE_M0APP)
	cr_start_m0(SLAVE_M0APP,&__core_m0app_START__);
#endif
	__disable_irq();
	anypal_CreateTimer();
	guihandler_CreateTimer();
	ipc_CreateTasks();
	irdaTask_Create();
	guiTask_Create();
	usbTask_Create();
	ftdiAnypalHost_CreateAnypalTask();
	usbHostTask_Create();
	logTask_CreateLogTask();
	BTTask_CreateBTTask();
	__enable_irq();
	ipc_WaitForHandShake();

	/* Start the scheduler */
	vTaskStartScheduler();
	/* Control should never come here */
	while (1)
	{

	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-MSleep													    				*/
/*                                                                                      */
/* Details  -Milli-second sleep function			    				        		*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int32_t msecs													    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void MSleep(int32_t msecs)
{
	int32_t curr = (int32_t) Chip_RIT_GetCounter(LPC_RITIMER);
	int32_t final = curr + ((SystemCoreClock / 1000) * msecs);

	/* If the value is zero let us not worry about it */
	if (!msecs || (msecs < 0)) {
		return;
	}

	if ((final < 0) && (curr > 0)) {
		while (Chip_RIT_GetCounter(LPC_RITIMER) < (uint32_t) final) {}
	}
	else {
		while ((int32_t) Chip_RIT_GetCounter(LPC_RITIMER) < final) {}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-initializeSpifi													        */
/*                                                                                      */
/* Details  -Initialize SPIFI						    				        		*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : SPIFI_HANDLE_T *		                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
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
	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FL016K);
	spifiRegisterFamily(SPIFI_REG_FAMILY_SpansionS25FL256SAGNFI000);

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
	return pReturnVal;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-updateSPI													        		*/
/*                                                                                      */
/* Details  -Update SPIFI						    				        			*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : result update SPI flash		                       				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool updateSPI(void)
{
	bool error = false;
	uint32_t spifiBaseClockRate;
	uint32_t maxSpifiClock;


	SPIFI_ERR_T errCode;

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

/****************************************************************************************/
/*                                                                                      */
/* General 	- updateBootloader							                                */
/*                                                                                      */
/* Details  -	Process bootloader updation											    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : None : 						                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_updateResult : 									        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
static void updateBootloader(void)
{
	int32_t width = LCD_WIDTH;
	int32_t height = LCD_HEIGHT;
	int32_t framebuf = (int32_t) (FRAMEBUFFER_ADDR);
	memset((void*) (framebuf), 0x00, width * height * 2);
	swim_window_open(&upgradeScreen, width, height, (COLOR_T*) framebuf, 0, 0, width - 1,
			height - 1, 0, WHITE, BLACK, BLACK);
	swim_set_font(&upgradeScreen,&font_rom8x16);
	swim_clear_screen(&upgradeScreen,BLACK);

	int8_t bootName[50] = {'\0'};
	swim_put_text_xy(&upgradeScreen,"System will reboot automatically.", 0, 0);
	swim_put_newline(&upgradeScreen);
	swim_put_text(&upgradeScreen,"Do not POWER OFF !!!");
	while(true)
	{
		swim_put_newline(&upgradeScreen);
		if(updateFlash_CheckBootloaderFile(bootName) == false)
		{
			swim_put_newline(&upgradeScreen);
			swim_put_text(&upgradeScreen,"Bootloader is updating...");
			if(updateFlash_Update(bootName) == false)
			{
				swim_put_text(&upgradeScreen,"Done");
				guiInterface_SetSettingValue(eModeSettingId,eStopMode);
				guiInterface_SetSettingValue(eUpgradeSettingId, false);
				if(guiInterface_GetSettingValue(eUpdateCtrlSWId) == true)
				{
					__disable_irq();
					Chip_IAP_PreSectorForReadWrite(14, 14,  IAP_FLASH_BANK_B);
					Chip_IAP_EraseSector(14, 14, IAP_FLASH_BANK_B);
					__enable_irq();
					guiInterface_SetSettingValue(eUpdateCtrlSWId, false);
				}

				modeMgr_WatchDogReset();
			}
			else
			{
				swim_put_newline(&upgradeScreen);
				swim_put_text(&upgradeScreen,"Fail: [E2] USB memory access error");
				swim_put_newline(&upgradeScreen);
				swim_put_text(&upgradeScreen,"Please remove the USB and insert again with update files!");
				s_updateResult = false;
			}
		}
		else
		{
			s_updateResult = false;
		}
		while (!s_updateResult)
		{
			delay_MS(10);
			Chip_WWDT_Feed(LPC_WWDT);
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-main													        			*/
/*                                                                                      */
/* Details  -Main function FH310						    				        	*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int		                       				                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int main(void)
{
	//initialize hardware
	system_init();
	updateSPI();
	Chip_IAP_init();
	powerHandle_Init();

	audioMgr_ResetAudio();
	extEeprom_ResetSW();
	guiInterface_Init();
	GUI_Init();

	uint8_t checksumValue = guiInterface_EEPROMChecksum(EEPROM_START_PAGE_ADDR_CHECKSUM,EEPROM_END_PAGE_ADDR_CHECKSUM);

	if((guiInterface_GetChecksumOfSettings() == checksumValue)
			&& (guiInterface_CheckSettingRange() == true))
	{
		guiTask_SetFlagAlarmNVRam(false);

		if(guiInterface_GetSettingValue(eUpgradeSettingId) == true)
		{
			updateBootloader();
		}
		else
		{
			Chip_WWDT_Feed(LPC_WWDT);
			mainScreen_ShowWakeup();
			Chip_WWDT_Feed(LPC_WWDT);
		}
	}
	else
	{
		Chip_WWDT_Feed(LPC_WWDT);
		mainScreen_ShowWakeup();
		Chip_WWDT_Feed(LPC_WWDT);
		guiTask_SetFlagAlarmNVRam(true);
	}

	powerHandle_DisablePowerDetection();

	// Use memory devices for all windows
#if GUI_SUPPORT_MEMDEV
	WM_SetCreateFlags(WM_CF_MEMDEV);
	WM_EnableMemdev(WM_HBKWIN);
#endif

	//Initialize FreeRTOS tasks
	ipc_Init();
	guiTask_Init();
	irdaTask_Init();
	usbTask_Init();
	BTTask_InitBTTask();
	ftdiAnypalHost_InitAnypalTask();

	main_tasks();

	return 0 ;
}
