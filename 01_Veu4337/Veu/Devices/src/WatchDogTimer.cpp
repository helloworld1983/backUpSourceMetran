/******************************************************************************/
//
//$COMMON.H$
//    File Name: WatchdogTimer.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: WatchdogTimer
//
//    WatchdogTimer allow user init, set Watchdog of system
//
/******************************************************************************/
#include "WatchDogTimer.h"
#include "board.h"
#include "DebugTrace.h"

//Timeout=1/Twdclk*4*TCvalue 1/((1/12000000)*4)
#define WDT_1S_CNT          (3000000)
#define WDT_TIMER           (5)

WatchDogTimer *WatchDogTimer::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the WatchDogTimer class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to WatchDogTimer class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
WatchDogTimer* WatchDogTimer::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new WatchDogTimer();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for WatchDogTimer
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void WatchDogTimer::Init (void)
{
    Chip_WWDT_Init(LPC_WWDT);

    Chip_WWDT_SetTimeOut(LPC_WWDT, WDT_TIMER*WDT_1S_CNT);
    // Configure WWDT to reset on timeout
    Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

    // Clear watchdog warning and timeout interrupts
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);

    // Clear watchdog warning and timeout interrupts
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: WatchDogTimer
//
//    Processing: constructor WatchDogTimer
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
WatchDogTimer::WatchDogTimer (void)
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: FeedWDTimer
//
//    Processing: Feed watchdog timer
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void WatchDogTimer::FeedWDTimer (void)
{
    Chip_WWDT_Feed(LPC_WWDT);
}
