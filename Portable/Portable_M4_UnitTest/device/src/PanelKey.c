/********************************************************************/
/*                                                                  */
/* File Name    : PanelKey.c	                                    */
/*                                                                  */
/* General       : This module contain function for Initialize and	*/
/* 					detect key push						            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include <assert.h>
#include <Adc.h>
#include <AlarmMsgHandler.h>
#include <AudioMgr.h>
#include <AudioTask.h>
#include <EEPRom.h>
#include <FlowCtrl.h>
#include <Gpio.h>
#include <PanelKey.h>
#include <OpTime.h>
#include <PanelLed.h>
#include <PsaCtrl.h>
#include <RTC.h>
#include <UIMgr.h>
#include <ValveCtrl.h>
#include "Setting.h"
#include "Compressor.h"
#include "ModeHandle.h"
#include "timers.h"
#include "Compressor.h"
#include "Fan.h"
#include "LogMgr.h"
#include "AlarmMgr.h"
#include "BreathTrigger.h"
#include "USBControl.h"
#include "string.h"
#include "Battery.h"
#include "LogTask.h"

//#include "UiTask.h"

#define KEY_PANEL_DELAY_TIME 40
#define KEY_PANEL_DELAY_LOOP_CNT KEY_PANEL_DELAY_TIME/DEVICE_TASK_DELAY_TIME
#define ZERO_SECOND_LOOP_CNT 0/KEY_PANEL_DELAY_TIME
#define ONE_SECOND_LOOP_CNT 1000/KEY_PANEL_DELAY_TIME
#define TWO_SECOND_LOOP_CNT 2000/KEY_PANEL_DELAY_TIME
#define THREE_SECOND_LOOP_CNT 3000/KEY_PANEL_DELAY_TIME
#define FOUR_SECOND_LOOP_CNT 4000/KEY_PANEL_DELAY_TIME
#define FIVE_SECOND_LOOP_CNT 5000/KEY_PANEL_DELAY_TIME
#define SIX_SECOND_LOOP_CNT 6000/KEY_PANEL_DELAY_TIME
#define SEVEN_SECOND_LOOP_CNT 7000/KEY_PANEL_DELAY_TIME
#define POWER_OFF_FC_OPEN_TIME 1000
#define SW1_PRESSED_VOLTAGE 0.5
#define SW2_PRESSED_VOLTAGE 0.1
#define SW3_PRESSED_VOLTAGE 0.80
#define SW4_PRESSED_VOLTAGE 1.7
#define SW5_PRESSED_VOLTAGE 2.6
#define RESET_SW_STATE_LOOP_CNT 100/KEY_PANEL_DELAY_TIME
//#define DEBUG_KEY_EN
#ifdef DEBUG_KEY_EN
#define DEBUG_KEY(...) printf(__VA_ARGS__)
#else
#define DEBUG_KEY(...)
#endif
#define NONE_PRESSED 0
#define SW1_PRESSED 1
#define SW2_PRESSED 2
#define SW3_PRESSED 4
#define SW4_PRESSED 8
#define SW5_PRESSED 16
#define KEY_CNT_WAIT_RELEASE -1
#define KEY_CNT_2S 2000/KEY_PANEL_DELAY_TIME
#define KEY_CNT_05S 500/KEY_PANEL_DELAY_TIME
#define KEY_4S_ID 14
#define KEY_4S_TICK 4000
#define KEY_1S_ID 15
#define KEY_1S_TICK 1000
#define KEY_3100MS_ID 16
#define KEY_3100MS_TICK 3100//2200
#define NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE 3
#define NUMBER_OF_MODE_CHANGE_BUTTON_TOGGLE_UPDATE 2
#define NUMBER_OF_INCREASE_BUTTON_TOGGLE_UPDATE 1
#define NUMBER_OF_DECREASE_BUTTON_TOGGLE_UPDATE 1
#define NUMBER_OF_MUTE_BUTTON_PUSH2S_UPDATE 1

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif
static bool gs_shutdownStatus;//truong adds
static uint8_t gs_numberOfMuteToggle = 0;
static uint8_t gs_numberOfModeChangeToggle = 0;
static uint8_t gs_numberOfIncreaseToggle = 0;
static uint8_t gs_numberOfDecreaseToggle = 0;
static uint8_t gs_numberOfMutePush2Seconds = 0;
static TimerHandle_t gs_key4STimer;
static TimerHandle_t gs_key1STimer;
static TimerHandle_t gs_key3100MSTimer;
static KEY_EVENT gs_SW1state = IDLE;
static KEY_EVENT gs_SW2state = IDLE;
static KEY_EVENT gs_SW3state = IDLE;
static KEY_EVENT gs_SW4state = IDLE;
static KEY_EVENT gs_SW5state = IDLE;
static KEY_EVENT gs_prevSW1state = IDLE;
static KEY_EVENT gs_prevSW2state = IDLE;
static KEY_EVENT gs_prevSW3state = IDLE;
static KEY_EVENT gs_prevSW4state = IDLE;
static KEY_EVENT gs_prevSW5state = IDLE;
static int8_t gs_SW1PushCount = 0;
static int8_t gs_SW2PushCount = 0;
static int8_t gs_SW3PushCount = 0;
static int8_t gs_SW4PushCount = 0;
static int8_t gs_SW5PushCount = 0;
static uint8_t gs_prevKeypressed = 0;
static bool gs_isTimerRun = false;

static void keyPanel_TimerCallback( TimerHandle_t xTimer );
static void keyPanel_CreateTimer(void);
static void keyPanel_PowerOffSystem(void);
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_TimerCallback					              				    */
/*                                                                                      */
/* Details:        Handle time out of all timer in Key_Panel module						*/
/*                                                                                      */
/* Arguments:	(I)	TimerHandle_t xTimer												*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_TimerCallback( TimerHandle_t xTimer )
{
    uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
    switch (timerId)
    {
        case KEY_4S_ID:
            DEBUG_KEY("3s: %d %d %d %d\n",gs_numberOfMuteToggle,gs_numberOfModeChangeToggle,gs_numberOfIncreaseToggle,gs_numberOfDecreaseToggle);
            if((gs_numberOfMuteToggle >= NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE)&&(gs_numberOfModeChangeToggle==NUMBER_OF_MODE_CHANGE_BUTTON_TOGGLE_UPDATE)
                    &&(gs_numberOfIncreaseToggle==NUMBER_OF_INCREASE_BUTTON_TOGGLE_UPDATE)&&(gs_numberOfDecreaseToggle==NUMBER_OF_DECREASE_BUTTON_TOGGLE_UPDATE))
            {
                DEBUG_KEY("start Key 1s timer \n");
                if (xTimerStart(gs_key1STimer,10) != pdPASS)
                {
                    DEBUG_TASK("ERR: Failed to start Key 1s timer \n");
                }
            }
            else
            {
                gs_numberOfMuteToggle=0;
                gs_numberOfModeChangeToggle=0;
                gs_numberOfIncreaseToggle=0;
                gs_numberOfDecreaseToggle=0;
                gs_numberOfMutePush2Seconds=0;
                gs_isTimerRun = false;
            }
            break;
        case KEY_1S_ID:
            DEBUG_KEY("1s: %d\n",gs_numberOfMuteToggle);
            if(gs_numberOfMuteToggle>=NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE+1)
            {
                DEBUG_KEY("start Key 2.2s timer \n");
                if (xTimerStart(gs_key3100MSTimer,10) != pdPASS)
                {
                    DEBUG_TASK("ERR: Failed to start Key 2.2s timer \n");
                }
            }
            else
            {
                gs_numberOfMuteToggle=0;
                gs_numberOfModeChangeToggle=0;
                gs_numberOfIncreaseToggle=0;
                gs_numberOfDecreaseToggle=0;
                gs_numberOfMutePush2Seconds=0;
                gs_isTimerRun = false;
            }
            break;
        case KEY_3100MS_ID:
            DEBUG_KEY("number of mute push 2s %d\n",gs_numberOfMutePush2Seconds);
            if(gs_numberOfMutePush2Seconds==NUMBER_OF_MUTE_BUTTON_PUSH2S_UPDATE)
            {
                DEBUG_KEY("Update flag set \n");
                panelLed_LedDisplay(eDisplayWordSET);

                POR_LOG_T log ={'\0'};
                log.logCode=eUpdateModeSetLogId;
                log.EventOrAlarm = eEvent;
                setting_GetCurrentSetting(&log.oldValue);
                setting_GetCurrentSetting(&log.newValue);
                E_logQueueReceive logQueue;
                logQueue.id = eRequestWriteLog;
                memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
                if(!logTask_SendToLogQueueReceive(logQueue, 10)){
                    DEBUG_TASK("Failed to send Item to logQueueRecive from Key_TimerCallback");
                }
                EEPROM_SetMode(eUpdateMode);
                vTaskDelay(DELAY_TICK_2S);
                UIMgr_UpdateMode();
            }
            gs_numberOfMuteToggle=0;
            gs_numberOfModeChangeToggle=0;
            gs_numberOfIncreaseToggle=0;
            gs_numberOfDecreaseToggle=0;
            gs_numberOfMutePush2Seconds=0;
            gs_isTimerRun = false;
            break;
        default:
            break;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_IncreaseButtonToggle					      				    */
/*                                                                                      */
/* Details:        Handle when increase button is toggled								*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_IncreaseButtonToggle(void)
{
    DEBUG_KEY("INCREASE BUTTON TOGGLE\n");
    // truong edits
    bool isNoALarm = alarmMsgHandler_GetStatusAlarm();
    bool isVolumeButtonOncePress = alarmMsgHandler_GetStatusVolumeButtonOncePress();
    if((isVolumeButtonOncePress)||(isNoALarm))
    {
        mode_IncreaseFlowRate();
        if((gs_isTimerRun==true) && (gs_numberOfMuteToggle==NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE) && (gs_numberOfModeChangeToggle==NUMBER_OF_MODE_CHANGE_BUTTON_TOGGLE_UPDATE))
        {
            gs_numberOfIncreaseToggle++;
        }
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_IncreaseButtonToggleShort				   				    */
/*                                                                                      */
/* Details:        Handle when increase button is toggled short							*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_IncreaseButtonToggleShort(void)
{
    DEBUG_KEY("INCREASE BUTTON TOGGLE SHORT\n");

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_IncreaseButtonToggle2S			     	 				    */
/*                                                                                      */
/* Details:        Handle when increase button is toggled for 2 second					*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_IncreaseButtonToggle2S(void)
{
    DEBUG_KEY("INCREASE BUTTON TOGGLE 2S\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_DecreaseButtonToggle			     	 				    */
/*                                                                                      */
/* Details:        Handle when decrease button is toggled 								*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_DecreaseButtonToggle(void)
{
    DEBUG_KEY("DECREASE BUTTON TOGGLE\n");
    // truong edits
    bool isVolumeButtonOncePress = alarmMsgHandler_GetStatusVolumeButtonOncePress();
    bool isNoALarm = alarmMsgHandler_GetStatusAlarm();
    if((isVolumeButtonOncePress)||(isNoALarm))
    {
        mode_DecreaseFlowRate();
        if((gs_isTimerRun==true) && (gs_numberOfMuteToggle==NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE) && (gs_numberOfModeChangeToggle==NUMBER_OF_MODE_CHANGE_BUTTON_TOGGLE_UPDATE)
                && (gs_numberOfIncreaseToggle==NUMBER_OF_INCREASE_BUTTON_TOGGLE_UPDATE))
        {
            gs_numberOfDecreaseToggle++;
        }
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_DecreaseButtonToggleShort			  	 				    */
/*                                                                                      */
/* Details:        Handle when decrease button is toggled short							*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_DecreaseButtonToggleShort(void)
{
    DEBUG_KEY("DECREASE BUTTON TOGGLE SHORT\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_DecreaseButtonToggle2S			  	 					    */
/*                                                                                      */
/* Details:        Handle when decrease button is toggled for 2 second					*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_DecreaseButtonToggle2S(void)
{
    DEBUG_KEY("DECREASE BUTTON TOGGLE 2S\n");
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_MuteButtonToggle				  	 				 		*/
/*                                                                                      */
/* Details:        Handle when mute button is toggled									*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_MuteButtonToggle(void)
{
    DEBUG_KEY("MUTE BUTTON TOGGLE\n");
    gs_numberOfMuteToggle++;
    if(gs_isTimerRun==false)
    {
        if (xTimerStart(gs_key4STimer,10) != pdPASS)
        {
            DEBUG_TASK("ERR: Failed to start Key 5s timer \n");
        }
        else
        {
            gs_isTimerRun = true;
        }
    }

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_MuteButtonToggleShort			  	 				 		*/
/*                                                                                      */
/* Details:        Handle when mute button is toggled short								*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_MuteButtonToggleShort(void)
{
    DEBUG_KEY("MUTE BUTTON TOGGLE SHORT\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_MuteButtonToggle2S				  	 				 		*/
/*                                                                                      */
/* Details:        Handle when mute button is toggled for 2 second						*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_MuteButtonToggle2S(void)
{
    DEBUG_KEY("MUTE BUTTON TOGGLE 2S\n");
    if((gs_numberOfMuteToggle >= (NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE+1))&&(gs_numberOfModeChangeToggle==NUMBER_OF_MODE_CHANGE_BUTTON_TOGGLE_UPDATE)
            &&(gs_numberOfIncreaseToggle==NUMBER_OF_INCREASE_BUTTON_TOGGLE_UPDATE)&&(gs_numberOfDecreaseToggle==NUMBER_OF_DECREASE_BUTTON_TOGGLE_UPDATE))
    {
        gs_numberOfMutePush2Seconds++;
    }
    AUDIO_T eventAudio;
    eventAudio.ID = eMuteAudio;
    eventAudio.volume = setting_Get(eAudioVolumeSettingID);

    if(!audioTask_SendToAudioQueue(eventAudio, 10)){
        DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
    }
    POR_LOG_T log ={'\0'};
    log.logCode=eMuteLogId;
    log.EventOrAlarm = eEvent;
    setting_GetCurrentSetting(&log.oldValue);
    setting_GetCurrentSetting(&log.newValue);
    E_logQueueReceive logQueue;
    logQueue.id = eRequestWriteLog;
    memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

    if(!logTask_SendToLogQueueReceive(logQueue, 10)){
        DEBUG_TASK("Failed to send Item to logQueueRecive from Key_MuteButtonToggle2S");
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ModeChangeButtonToggle				  	 					*/
/*                                                                                      */
/* Details:        Handle when mode change button is toggled							*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_ModeChangeButtonToggle(void)
{
    DEBUG_KEY("MODE CHANGE BUTTON TOGGLE\n");
    mode_ChangeMode();
    if((gs_isTimerRun==true) && (gs_numberOfMuteToggle==NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE))
    {
        gs_numberOfModeChangeToggle++;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ModeChangeButtonToggleShort		 						*/
/*                                                                                      */
/* Details:        Handle when mode change button is toggled short						*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_ModeChangeButtonToggleShort(void)
{
    DEBUG_KEY("MODE CHANGE BUTTON TOGGLE SHORT\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ModeChangeButtonToggle2S		  	 						*/
/*                                                                                      */
/* Details:        Handle when mode change button is toggled for 2 second				*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_ModeChangeButtonToggle2S(void)
{
    DEBUGOUT("MODE CHANGE BUTTON TOGGLE 2S\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_PowerButtonToggle			  	 							*/
/*                                                                                      */
/* Details:        Handle when power button is toggled 									*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_PowerButtonToggle(void)
{
    DEBUG_KEY("POWER BUTTON TOGGLE\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_PowerButtonToggleShort		  	 							*/
/*                                                                                      */
/* Details:        Handle when power button is toggled short							*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_PowerButtonToggleShort(void)
{
    DEBUG_KEY("POWER BUTTON TOGGLE SHORT\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_PowerButtonToggle2S			  	 						*/
/*                                                                                      */
/* Details:        Handle when power button is toggled for 2 second						*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_PowerButtonToggle2S(void)
{
    DEBUG_KEY("POWER BUTTON TOGGLE 2S \n");
    if(gs_shutdownStatus==false)
    {
        gs_shutdownStatus = true;
        keyPanel_PowerOffSystem();
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_PowerOffSystem				  	 							*/
/*                                                                                      */
/* Details:        Handle system power off												*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_PowerOffSystem(void)
{
    static uint16_t s_cnt = 0;
    uint16_t openTime=POWER_OFF_FC_OPEN_TIME;
    AUDIO_T eventAudio;

    switch(s_cnt)
    {
        case ZERO_SECOND_LOOP_CNT:
        	alarmMsgHandler_DisableAllAlarm();
            compressor_Stop();
            flowCtrl_Stop();
            psa_Stop();
            breathTrigger_Stop();
            valve_openVal1();
            valve_openVal3();
            UIMgr_DisableDisplayBatteryPercentWhenShutdown();
            panelLed_LedDisplay(eWithoutChargeLower);

            panelLed_LedDisplay(eOffAllLeds);

            panelLed_LedDisplay(eDisplayWordE);

            break;
        case ONE_SECOND_LOOP_CNT:
            UIMgr_SetPowerOffFlag(true);
            eventAudio.ID = eTerminateAudio;
            eventAudio.volume = setting_Get(eAudioVolumeSettingID);
            POR_LOG_T log;
            log.logCode=eOperationStopLogId;
            log.EventOrAlarm = eEvent;
            setting_GetCurrentSetting(&log.oldValue);
            setting_GetCurrentSetting(&log.newValue);
            E_logQueueReceive logQueue;
            logQueue.id = eRequestWriteLog;
            memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

            if(!logTask_SendToLogQueueReceive(logQueue, 10)){
                DEBUG_TASK("Failed to send Item to logQueueRecive from Key_PowerOffSystem");
            }

            if(!audioTask_SendToAudioQueue(eventAudio, 10)){
                DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
            }
            panelLed_LedDisplay(eDisplayWordEn);

            break;
        case TWO_SECOND_LOOP_CNT:
            flowCtrl_SendOpenAndCloseSettingCmd(&openTime);
            panelLed_LedDisplay(eDisplayWordEnd);

            break;
        case THREE_SECOND_LOOP_CNT:
            if(alarmMsgHandler_IsGoingToPlayPowerLostAlarm()==false)
            {
                if(EEPROM_ReadMode()!=eUpdateMode)
                    EEPROM_SetMode(eOperationMode);
            }
            else
            {
                EEPROM_SetMode(eVeryLowBatteryMode);
            }
            break;
        case FOUR_SECOND_LOOP_CNT:
            taskENTER_CRITICAL();
            WDT_SystemReset();
            break;
        default:
            break;
    }
    s_cnt++;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ReadValue				   									*/
/*                                                                                      */
/* Details:        check whether each key is pressed by read adc value					*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
static uint8_t keyPanel_ReadValue()
{
    uint8_t Keypressed=NONE_PRESSED;
    float voltVal_Key1=0;
    voltVal_Key1 = adc_Read(KEY1_ADC_CH);
    if(voltVal_Key1 <= SW2_PRESSED_VOLTAGE)
    {
        Keypressed |= SW2_PRESSED;
        DEBUG_KEY("SW2 pressed \n");
    }
    else if(voltVal_Key1 <= SW3_PRESSED_VOLTAGE)
    {
        Keypressed |= SW3_PRESSED;
        DEBUG_KEY("SW3 pressed \n");
    }
    else if(voltVal_Key1 <= SW4_PRESSED_VOLTAGE)
    {
        Keypressed |= SW4_PRESSED;
        DEBUG_KEY("SW4 pressed \n");
    }
    else if(voltVal_Key1 <= SW5_PRESSED_VOLTAGE)
    {
        Keypressed |= SW5_PRESSED;
        DEBUG_KEY("SW5 pressed \n");
    }
    float voltVal_Key0=0;
    voltVal_Key0 = adc_Read(KEY0_ADC_CH);
    if(voltVal_Key0<=SW1_PRESSED_VOLTAGE)
    {
        Keypressed |= SW1_PRESSED;
    }
    return Keypressed;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CheckStateSW1			  	 								*/
/*                                                                                      */
/* Details:        Determine which operation has just acted on SW1						*/
/*                                                                                      */
/* Arguments:	(I)	uint8_t Keypressed													*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CheckStateSW1(uint8_t Keypressed)
{
    if((Keypressed & SW1_PRESSED) == SW1_PRESSED)
    {
        if((gs_prevKeypressed & SW1_PRESSED)!=SW1_PRESSED)
            gs_SW1state = TOGGLE;
        else
        {
            if((gs_prevKeypressed & SW1_PRESSED)==SW1_PRESSED )
            {
                if(gs_SW1PushCount != KEY_CNT_WAIT_RELEASE)
                {
                    gs_SW1PushCount++;
                }
                if(gs_SW1PushCount >= KEY_CNT_2S)
                {
                    gs_SW1PushCount = KEY_CNT_WAIT_RELEASE;
                    gs_SW1state = TOGGLE2S;
                }
            }
            else
            {
                assert(false);
            }
        }
    }
    else
    {
        if((Keypressed & SW1_PRESSED) != SW1_PRESSED)
        {
            if((gs_SW1PushCount<KEY_CNT_2S)&&(gs_SW1PushCount>KEY_CNT_05S))
            {
                gs_SW1state = TOGGLESHORT;
            }
            gs_SW1PushCount=0;
        }
        else
        {
            assert(false);
        }
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CheckStateSW2			  	 								*/
/*                                                                                      */
/* Details:        Determine which operation has just acted on SW2						*/
/*                                                                                      */
/* Arguments:	(I)	uint8_t Keypressed													*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CheckStateSW2(uint8_t Keypressed)
{
    if((Keypressed & SW2_PRESSED) == SW2_PRESSED)
    {
        if((gs_prevKeypressed & SW2_PRESSED)!=SW2_PRESSED)
        {
            gs_SW2state = TOGGLE;
        }
        else
        {
            if((gs_prevKeypressed & SW2_PRESSED)==SW2_PRESSED )
            {
                if(gs_SW2PushCount != KEY_CNT_WAIT_RELEASE)
                {
                    gs_SW2PushCount++;
                }
                if(gs_SW2PushCount >= KEY_CNT_2S)
                {
                    gs_SW2PushCount = KEY_CNT_WAIT_RELEASE;
                    gs_SW2state = TOGGLE2S;
                }
            }
            else
            {
                assert(false);
            }
        }
    }
    else
    {
        if((Keypressed & SW2_PRESSED) != SW2_PRESSED)
        {
            if((gs_SW2PushCount<KEY_CNT_2S)&&(gs_SW2PushCount>KEY_CNT_05S))
            {
                gs_SW2state = TOGGLESHORT;
            }
            gs_SW2PushCount=0;
        }
        else
        {
            assert(false);
        }
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CheckStateSW3			  	 								*/
/*                                                                                      */
/* Details:        Determine which operation has just acted on SW3						*/
/*                                                                                      */
/* Arguments:	(I)	uint8_t Keypressed													*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CheckStateSW3(uint8_t Keypressed)
{
    if((Keypressed & SW3_PRESSED) == SW3_PRESSED)
    {
        if((gs_prevKeypressed & SW3_PRESSED)!=SW3_PRESSED)
            gs_SW3state = TOGGLE;
        else
            if((gs_prevKeypressed & SW3_PRESSED)==SW3_PRESSED )
            {
                if(gs_SW3PushCount != KEY_CNT_WAIT_RELEASE)
                {
                    gs_SW3PushCount++;
                }
                if(gs_SW3PushCount >= KEY_CNT_2S)
                {
                    gs_SW3PushCount = KEY_CNT_WAIT_RELEASE;
                    gs_SW3state = TOGGLE2S;
                }
            }
            else
            {
                assert(false);
            }
    }
    else
        if((Keypressed & SW3_PRESSED)!= SW3_PRESSED)
        {
            if((gs_SW3PushCount<KEY_CNT_2S)&&(gs_SW3PushCount>KEY_CNT_05S))
            {
                gs_SW3state = TOGGLESHORT;
            }
            gs_SW3PushCount=0;
        }
        else
        {
            assert(false);
        }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CheckStateSW4			  	 								*/
/*                                                                                      */
/* Details:        Determine which operation has just acted on SW4						*/
/*                                                                                      */
/* Arguments:	(I)	uint8_t Keypressed													*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CheckStateSW4(uint8_t Keypressed)
{
    if((Keypressed & SW4_PRESSED) == SW4_PRESSED)
    {
        if((gs_prevKeypressed & SW4_PRESSED)!=SW4_PRESSED)
            gs_SW4state = TOGGLE;
        else
            if((gs_prevKeypressed & SW4_PRESSED)==SW4_PRESSED )
            {
                if(gs_SW4PushCount != KEY_CNT_WAIT_RELEASE)
                {
                    gs_SW4PushCount++;
                }
                if(gs_SW4PushCount >= KEY_CNT_2S)
                {
                    gs_SW4PushCount = KEY_CNT_WAIT_RELEASE;
                    gs_SW4state = TOGGLE2S;
                }
            }
            else
            {
                assert(false);
            }
    }
    else
        if((Keypressed & SW4_PRESSED)!= SW4_PRESSED)
        {
            if((gs_SW4PushCount<KEY_CNT_2S)&&(gs_SW4PushCount>KEY_CNT_05S))
            {
                gs_SW4state = TOGGLESHORT;
            }
            gs_SW4PushCount=0;
        }
        else
        {
            assert(false);
        }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CheckStateSW5			  	 								*/
/*                                                                                      */
/* Details:        Determine which operation has just acted on SW5						*/
/*                                                                                      */
/* Arguments:	(I)	uint8_t Keypressed													*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CheckStateSW5(uint8_t Keypressed)
{
    if((Keypressed & SW5_PRESSED) == SW5_PRESSED)
    {
        if((gs_prevKeypressed & SW5_PRESSED)!=SW5_PRESSED)
            gs_SW5state = TOGGLE;
        else
            if((gs_prevKeypressed & SW5_PRESSED)==SW5_PRESSED )
            {
                if(gs_SW5PushCount != KEY_CNT_WAIT_RELEASE)
                {
                    gs_SW5PushCount++;
                }
                if(gs_SW5PushCount >= KEY_CNT_2S)
                {
                    gs_SW5PushCount = KEY_CNT_WAIT_RELEASE;
                    gs_SW5state = TOGGLE2S;
                }
            }
            else
            {
                assert(false);
            }
    }
    else
        if((Keypressed & SW5_PRESSED)!= SW5_PRESSED)
        {
            if((gs_SW5PushCount<KEY_CNT_2S)&&(gs_SW5PushCount>KEY_CNT_05S))
            {
                gs_SW5state = TOGGLESHORT;
            }
            gs_SW5PushCount=0;
        }
        else
        {
            assert(false);
        }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CheckState				  	 								*/
/*                                                                                      */
/* Details:        Determine which state each key is at									*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CheckState()
{
    static uint8_t s_counter=0;
    uint8_t Keypressed = keyPanel_ReadValue();
    keyPanel_CheckStateSW1(Keypressed);
    keyPanel_CheckStateSW2(Keypressed);
    keyPanel_CheckStateSW3(Keypressed);
    keyPanel_CheckStateSW4(Keypressed);
    keyPanel_CheckStateSW5(Keypressed);
    gs_prevKeypressed = Keypressed;
    s_counter++;
    if(s_counter == RESET_SW_STATE_LOOP_CNT)
    {
        if(gs_prevSW1state!=IDLE)
            gs_prevSW1state=IDLE;
        if(gs_prevSW2state!=IDLE)
            gs_prevSW2state=IDLE;
        if(gs_prevSW3state!=IDLE)
            gs_prevSW3state=IDLE;
        if(gs_prevSW4state!=IDLE)
            gs_prevSW4state=IDLE;
        if(gs_prevSW5state!=IDLE)
            gs_prevSW5state=IDLE;
        s_counter=0;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ProcessEventSW1			  								*/
/*                                                                                      */
/* Details:        Call function base on the operation has just acted on SW1			*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_ProcessEventSW1()
{
    switch(gs_SW1state)
    {
        case TOGGLE:
        	keyPanel_PowerButtonToggle();
            gs_prevSW1state=TOGGLE;
            gs_SW1state = IDLE;
            break;
        case TOGGLESHORT:
        	keyPanel_PowerButtonToggleShort();
            gs_prevSW1state=TOGGLESHORT;
            gs_SW1state = IDLE;
            break;
        case TOGGLE2S:
        	keyPanel_PowerButtonToggle2S();
            gs_prevSW1state=TOGGLE2S;
            gs_SW1state = IDLE;
            break;
        default:
            break;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ProcessEventSW2			  	 							*/
/*                                                                                      */
/* Details:        Call function base on the operation has just acted on SW2			*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_ProcessEventSW2()
{
    switch(gs_SW2state)
    {
        case TOGGLE:
        	keyPanel_IncreaseButtonToggle();
            gs_prevSW2state=TOGGLE;
            gs_SW2state = IDLE;
            break;
        case TOGGLESHORT:
        	keyPanel_IncreaseButtonToggleShort();
            gs_prevSW2state=TOGGLESHORT;
            gs_SW2state = IDLE;
            break;
        case TOGGLE2S:
        	keyPanel_IncreaseButtonToggle2S();
            gs_prevSW2state=TOGGLE2S;
            gs_SW2state = IDLE;
            break;
        default:
            break;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ProcessEventSW3			  	 							*/
/*                                                                                      */
/* Details:        Call function base on the operation has just acted on SW3			*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_ProcessEventSW3()
{
    switch(gs_SW3state)
    {
        case TOGGLE:
        	keyPanel_DecreaseButtonToggle();
            gs_prevSW3state=TOGGLE;
            gs_SW3state = IDLE;
            break;
        case TOGGLESHORT:
        	keyPanel_DecreaseButtonToggleShort();
            gs_prevSW3state=TOGGLESHORT;
            gs_SW3state = IDLE;
            break;
        case TOGGLE2S:
        	keyPanel_DecreaseButtonToggle2S();
            gs_prevSW3state=TOGGLE2S;
            gs_SW3state = IDLE;
            break;
        default:
            break;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ProcessEventSW4			  	 							*/
/*                                                                                      */
/* Details:        Call function base on the operation has just acted on SW4			*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_ProcessEventSW4()
{
    switch(gs_SW4state)
    {
        case TOGGLE:
        	keyPanel_MuteButtonToggle();
            gs_prevSW4state=TOGGLE;
            gs_SW4state = IDLE;
            break;
        case TOGGLESHORT:
        	keyPanel_MuteButtonToggleShort();
            gs_prevSW4state=TOGGLESHORT;
            gs_SW4state = IDLE;
            break;
        case TOGGLE2S:
        	keyPanel_MuteButtonToggle2S();
            gs_prevSW4state=TOGGLE2S;
            gs_SW4state = IDLE;
            break;
        default:
            break;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ProcessEventSW5			  	 							*/
/*                                                                                      */
/* Details:        Call function base on the operation has just acted on SW5			*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_ProcessEventSW5()
{

    switch(gs_SW5state)
    {
        case TOGGLE:
        	keyPanel_ModeChangeButtonToggle();
            gs_prevSW5state=TOGGLE;
            gs_SW5state = IDLE;
            break;
        case TOGGLESHORT:
        	keyPanel_ModeChangeButtonToggleShort();
            gs_prevSW5state=TOGGLESHORT;
            gs_SW5state = IDLE;
            break;
        case TOGGLE2S:
        	keyPanel_ModeChangeButtonToggle2S();
            gs_prevSW5state=TOGGLE2S;
            gs_SW5state = IDLE;
            break;
        default:
            break;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_ProcessEvent				  								*/
/*                                                                                      */
/* Details:        	process event for each key											*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_ProcessEvent()
{
    if((gs_SW4state == TOGGLE && gs_SW1state==TOGGLE)
            || (gs_SW4state == TOGGLE && gs_prevSW1state==TOGGLE)
            ||(gs_prevSW4state == TOGGLE && gs_SW1state==TOGGLE))
    {
    	alarmMsgHandler_StopDisplayErrorCode();// Truong adds
        OperationTime_Display();
    }
    keyPanel_ProcessEventSW1();
    keyPanel_ProcessEventSW2();
    keyPanel_ProcessEventSW3();
    keyPanel_ProcessEventSW4();
    keyPanel_ProcessEventSW5();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_CreateTimer					  							*/
/*                                                                                      */
/* Details:        	Create all timer for Key_panel module								*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
static void keyPanel_CreateTimer(void)
{
    gs_key4STimer = xTimerCreate
            ( /* Just a text name, not used by the RTOS
				                      kernel. */
                    "Key 4S Timer",
                    /* The timer period in ticks, must be
				                      greater than 0. */
                    pdMS_TO_TICKS(KEY_4S_TICK),
                    /* The timers will auto-reload themselves
				                      when they expire. */
                    pdFALSE,
                    /* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
                    ( void * ) KEY_4S_ID,
                    /* Each timer calls the same callback when
				                      it expires. */
					keyPanel_TimerCallback
            );
    gs_key1STimer = xTimerCreate
            ( /* Just a text name, not used by the RTOS
				                      kernel. */
                    "Key 1S Timer",
                    /* The timer period in ticks, must be
				                      greater than 0. */
                    pdMS_TO_TICKS(KEY_1S_TICK),
                    /* The timers will auto-reload themselves
				                      when they expire. */
                    pdFALSE,
                    /* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
                    ( void * ) KEY_1S_ID,
                    /* Each timer calls the same callback when
				                      it expires. */
					keyPanel_TimerCallback
            );
    gs_key3100MSTimer = xTimerCreate
            ( /* Just a text name, not used by the RTOS
				                      kernel. */
                    "Key 3.1S Timer",
                    /* The timer period in ticks, must be
				                      greater than 0. */
                    pdMS_TO_TICKS(KEY_3100MS_TICK),
                    /* The timers will auto-reload themselves
				                      when they expire. */
                    pdFALSE,
                    /* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
                    ( void * ) KEY_3100MS_ID,
                    /* Each timer calls the same callback when
				                      it expires. */
					keyPanel_TimerCallback
            );
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_Init					  									*/
/*                                                                                      */
/* Details:        	Initialize by create Timer for Key module							*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
void keyPanel_Init()
{
	keyPanel_CreateTimer();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_Handle								              			*/
/*                                                                                      */
/* Details:    	Key handle function is call for update Key status						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_Handle()
{
    static uint8_t s_cnt = 0;
    if(s_cnt>=KEY_PANEL_DELAY_LOOP_CNT)
    {
        if(gs_shutdownStatus==true)
        {
        	keyPanel_PowerOffSystem();
        }
        else
        {
        	keyPanel_CheckState();
        	keyPanel_ProcessEvent();
        }
        s_cnt = 0;
    }
    else
    {
        s_cnt++;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	keyPanel_GetButtonStatus						              		*/
/*                                                                                      */
/* Details:    	Get current status of each Key on the panel								*/
/*                                                                                      */
/* Arguments:	(I) KEY_EVENT* status													*/
/*              (O) KEY_EVENT* status                                                   */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_GetButtonStatus(KEY_EVENT* status)
{
    status[ePowerButtonStatus] 	  	= 	gs_prevSW1state;
    status[eIncreaseButtonStatus] 	= 	gs_prevSW2state;
    status[eDecreaseButtonStatus] 	= 	gs_prevSW3state;
    status[eMuteButtonStatus]     	= 	gs_prevSW4state;
    status[eModeChangeButtonStatus] = 	gs_prevSW5state;
}
