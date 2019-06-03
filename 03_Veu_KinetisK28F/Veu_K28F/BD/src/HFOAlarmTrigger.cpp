/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HFOAlarmTrigger.cpp
//   Purpose: 
//      This file contains the definition of the High MAP_2 Alarm Trigger.
//      The High MAP_2 Alarm Trigger is derived from the Trigger.It is used
//        to detect the High MAP_2 condition during HFO mode.
//
//   Interfaces:
//      The CheckCondition() operation retrieves the current pressure measured 
//        by calling HfoController::GetFilterPressIntegrator() and it retrieves 
//        the operator setting for High MAP_2 Limit by calling 
//        MgrBdSetting::GetCurrentValue().
//   Restrictions:
//      None
//
//   Revision History:
//
//
/******************************************************************************/
#ifdef HFO_SYSTEM
#include "HFOAlarmTrigger.h"
#include "HFOHighMAP_2Mode.h"
#include "Hfo_cnt.h"
#include "MgrBdSetting.h"
#include "mainhandlerdelegate.h"
#include "AlarmStat.h"
#include "MgrBreathData.h"
#include "VirtualPressure.h"
#include "proxyflowsens.h"
#define  HIGH_SI_OFFSET 500
#define  LOW_SI_OFFSET 200
#define  EXTREME_HIGH_SI_OFFSET 2000
#define FLOW_LIMIT 3000

static const Int32 NUM_OF_SAMPLE[13] ={30,28,22,20,18,16,14,12,12,10,10,10,8}; 		//sample number for 1/3 cycle data
static const Int32 NUM_OF_Check[13] ={12,10,8,6,6,6,6,6,6,5,5,4,4}; 				//check number for flat form
int Flow_Buffer[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	//buffer for data storage

HFOAlarmTrigger* HFOAlarmTrigger::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HFOAlarmTriger()
//
//    Processing: 
//      This operation is the constructor for the HFOAlarmTrigger.
//      This operation invokes the Trigger base class constructor.
//
//    Input Parameters:
//      triggerId - Trigger ID 
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
HFOAlarmTrigger::HFOAlarmTrigger(void)
    : Trigger(eHighMAP_2AlarmTrigger)
{
    InactiveTimer = 5000;
    Index = 0;			//counter for sample buffer
    IsFlatPattern = false;			//flag for flat pattern
    IsFlowOverLimit = false;		//flag for flow over limit  if you have disconnect ET tube, flow patter have overshoot and flat, but overshoot is not stable.
    Counter500msec = 0;		//counter for 1sec
    FlowOverLimitCnt = 0;		//counter for flow over limit
    CheckCnt = 0;
    AlarmActiveCnt = 0;		//alarm active counter
    AlarmResetCnt = 0;
    IsAmpChecking = false;

    delayMapAlarmCheck = 100;//100ms
}   // end HFOAlarmTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation retrieves the current pressure measured by calling 
//        HfoController::GetFilterPressIntegrator() and it retrieves
//        the operator setting for High MAP_2 Limit by calling HfoController::GetHighMAP_2Limit()
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool HFOAlarmTrigger::CheckCondition(void)
{

    static short SIdelay = 0;

    if(((HfoController->GetSIRequest()== eTrue)||(HfoController->GetHIMP()== eTrue))||(SIdelay !=0))
    {
        if((HfoController->GetSIRequest()== eTrue)||(HfoController->GetHIMP()== eTrue))
        //If SI, Check Extreme High SI
        {
            SIdelay = 1500;//3s
            TriggerDidFire = CheckExtremeHighSIAlarm(eExtremeHighSIPress);
            if(TriggerDidFire == eTrue)
            {
                SIdelay=0;
                return (TriggerDidFire);
            }
        }
        else // delay 3s after stopping SI
        {
            if(SIdelay>0)
                SIdelay--;
            TriggerDidFire = CheckExtremeHighSIAlarm(eExtremeHighMAP);
            if(TriggerDidFire == eTrue)
            {
                SIdelay=0;
                return (TriggerDidFire);
            }
        }


    }
    else
    {
        TriggerDidFire = CheckExtremeHighMapAlarm();
        if(TriggerDidFire == eTrue)
            return (TriggerDidFire);

    }

    if ( InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    if((HfoController->GetSIRequest()== eTrue)||(HfoController->GetHIMP()== eTrue))
    {
        CheckSIAlarm();
    }
    else if(SIdelay == 0)
    {
        CheckMapAlarm();
        CheckAmplitudeAlarm();
        CheckProximalLeakage();
    }

    return (TriggerDidFire);
}   // end CheckCondition()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckExtremeHighSIAlarm()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
E_Bool HFOAlarmTrigger::CheckExtremeHighSIAlarm(E_GUIEventID alarmType)
{
    static USHORT extremeHighSICounter = 0;
    LONG map;
    LONG level2_SIPressLimit;
    E_Bool result = eFalse;

    level2_SIPressLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSI) + EXTREME_HIGH_SI_OFFSET;
    map = HfoController->GetFilterPressIntegrator();

    if (map > level2_SIPressLimit)
    {
        extremeHighSICounter++;
        if ( extremeHighSICounter >= 1) // 1 * 2ms servo = 2ms
        {
            result = eTrue;
            HFOHighMAP_2Mode::S_GetInstance()->SetCauseOfAlarm(alarmType);
            Alarm[ALARM_IDX(alarmType)]->SetIsActive(eActive);
            return (result);
        }
    }
    else
    {
        extremeHighSICounter = 0;
    }
    return result;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckExtremeHighMapAlarm()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if Extreme High Map Alarm happened or not
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
E_Bool HFOAlarmTrigger::CheckExtremeHighMapAlarm()
{
    static USHORT extremeHighMapCounter = 0;

    E_Bool result = eFalse;
    LONG map;
    LONG level2PressLimit;

    map = HfoController->GetFilterPressIntegrator();
    level2PressLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHiMAP_1Limit) +
            MgrBdSetting::S_GetInstance()->GetCurrentValue(eExtremeHighMap_Autoset);


    if ((map > level2PressLimit))
    {
        result = eTrue;
        HFOHighMAP_2Mode::S_GetInstance()->SetCauseOfAlarm(eExtremeHighMAP);
        Alarm[ALARM_IDX(eExtremeHighMAP)]->SetIsActive(eActive);
        return (result);
    }
    else
    {
        extremeHighMapCounter = 0;
    }
    return (result);

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckProximalLeakage()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void HFOAlarmTrigger::CheckProximalLeakage()
{
    int sum=0;
    int counter=0;

    ProxyStatus Status = proxySensor->getLastStatus();
    if((Status==eCalibrated)||(Status==eResetAndRecovered))
    {
        LONG freq = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHFOFrequency)/SCALE;
        unsigned short numofsample = NUM_OF_SAMPLE[freq - 5];
        LONG flow = proxySensor->GetLastReading();
        //eto modify
        Flow_Buffer[Index]=flow;		//get flow data to buffer
        Index++;
        if(Index >= numofsample)
        {
            Index = 0;
        }

        //sum of sample data. when mode change these data need reset.
        for(int i=0; i < numofsample; i++)
        {
            sum+=Flow_Buffer[i];
        }

        //check for flat pattern
        for(int i=0; i < numofsample; i++)
        {
            //judge margine 50 is 0.5lpm
            if(abs(sum/numofsample - Flow_Buffer[i]) <50){
                counter++;
            }

        }

        //check for flat pattern or not
        if(counter>=NUM_OF_Check[freq-5])
        {
            CheckCnt++;
        }


        //check for flow over condition or not
        if(abs(flow) > FLOW_LIMIT)
        {
            FlowOverLimitCnt++;
        }

        //check for over limit flow count in 500msec
        Counter500msec++;
        if(Counter500msec > 250){
            //
            if(FlowOverLimitCnt>freq/4){
                IsFlowOverLimit = true;
            }
            else{
                IsFlowOverLimit = false;
            }

            if(CheckCnt>freq/4){
                IsFlatPattern = true;
            }
            else{
                IsFlatPattern = false;
            }

            FlowOverLimitCnt=0;
            Counter500msec =0;
            CheckCnt=0;
        }

        //check for immediately change condition
        //It is looking at 2 counts of 500msec in fact
        if((IsFlowOverLimit == true) && (IsFlatPattern == true))
        {
            AlarmActiveCnt++;
            AlarmResetCnt = 0;
        }
        else
        {
            AlarmActiveCnt = 0;
            AlarmResetCnt++;
        }

        if(AlarmActiveCnt >500){	//this alarm is inactive during SI, before amplitude stable.
            if ( Alarm[ALARM_IDX(eETTubeDisconnect)]->GetIsActive() != eActive){
                Alarm[ALARM_IDX(eETTubeDisconnect)]->SetIsActive(eActive);
            }
            AlarmActiveCnt = 0;
        }

        if(AlarmResetCnt > 500)
        {
            if ( Alarm[ALARM_IDX(eETTubeDisconnect)]->GetIsActive() == eActive){
                Alarm[ALARM_IDX(eETTubeDisconnect)]->SetIsActive(eNotActive);
            }
            AlarmResetCnt=0;
        }

    }


}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckAmplitudeAlarm()
//
//    Processing:
//      Get current amplitude then compare it to HiAmplitudeLimit and Low Amplitude Limit
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void HFOAlarmTrigger::CheckAmplitudeAlarm()
{
    if(IsAmpChecking == false)
    {
        Alarm[ALARM_IDX(eHighAmplitude)]->SetIsActive(eNotActive);
        Alarm[ALARM_IDX(eLowAmplitude)]->SetIsActive(eNotActive);
        return;
    }

    static USHORT lowAmplitudeCounter = 0;
    static USHORT highAmplitudeCounter = 0;

    LONG maxpress      = MgrBreathData::S_GetInstance()->GetMaxPresData();
    LONG minpress      = MgrBreathData::S_GetInstance()->GetMinPresData();
    LONG amplitude      = maxpress - minpress;
    LONG LowAmplitudeLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowAmplitudeLimit);
    LONG HighAmplitudeLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighAmplitudeLimit);

    if(HighAmplitudeLimit == BLANKED_LONG_VALUE)
        return;

    if(LowAmplitudeLimit == BLANKED_LONG_VALUE)
        return;

    if(amplitude < 0) amplitude = 0;

    if(LowAmplitudeLimit> amplitude)
    {
        lowAmplitudeCounter++;
        if(lowAmplitudeCounter>250)
        {
            if ( Alarm[ALARM_IDX(eLowAmplitude)]->GetIsActive() != eActive)
                Alarm[ALARM_IDX(eLowAmplitude)]->SetIsActive(eActive);
            lowAmplitudeCounter=0;
        }
    }
    else
    {
        lowAmplitudeCounter=0;
        if (Alarm[ALARM_IDX(eLowAmplitude)]->GetIsActive() != eNotActive)
            Alarm[ALARM_IDX(eLowAmplitude)]->SetIsActive(eNotActive);
    }

    if(HighAmplitudeLimit< amplitude)
    {
        highAmplitudeCounter++;
        if(highAmplitudeCounter>250)
        {
            if ( Alarm[ALARM_IDX(eHighAmplitude)]->GetIsActive() != eActive)
                Alarm[ALARM_IDX(eHighAmplitude)]->SetIsActive(eActive);
            highAmplitudeCounter = 0;
        }
    }
    else
    {
        highAmplitudeCounter = 0;
        if (Alarm[ALARM_IDX(eHighAmplitude)]->GetIsActive() != eNotActive)
            Alarm[ALARM_IDX(eHighAmplitude)]->SetIsActive(eNotActive);
    }

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckMapAlarm()
//
//    Processing:
//      Get Map value from HfoController->GetFilterPressIntegrator()
//      and compare this value to HiMapLimit and LowMapLimit
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void HFOAlarmTrigger::CheckMapAlarm()
{
    if(delayMapAlarmCheck > 0)
    {
        delayMapAlarmCheck = delayMapAlarmCheck - 2;
        return;
    }
    static USHORT highMapCounter = 0;
    static USHORT lowMapCounter = 0;
    LONG map;
    map = HfoController->GetFilterPressIntegrator();
    if(map > MgrBdSetting::S_GetInstance()->GetCurrentValue(eHiMAP_1Limit))
    {
        highMapCounter++;
        if (highMapCounter >50)
            if ( Alarm[ALARM_IDX(eHighMAP)]->GetIsActive() != eActive)
                Alarm[ALARM_IDX(eHighMAP)]->SetIsActive(eActive);

    }
    else
    {
        highMapCounter = 0;
        if (Alarm[ALARM_IDX(eHighMAP)]->GetIsActive() != eNotActive)
            Alarm[ALARM_IDX(eHighMAP)]->SetIsActive(eNotActive);
    }

    if(map < MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowMAPLimit))
    {
        lowMapCounter++;
        if(lowMapCounter>50)
            if ( Alarm[ALARM_IDX(eLowMAP)]->GetIsActive() != eActive)
                Alarm[ALARM_IDX(eLowMAP)]->SetIsActive(eActive);

    }
    else
    {
        lowMapCounter = 0;
        if (Alarm[ALARM_IDX(eLowMAP)]->GetIsActive() != eNotActive)
            Alarm[ALARM_IDX(eLowMAP)]->SetIsActive(eNotActive);
    }

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckSIAlarm()
//
//    Processing:
//      Get SI value from ActivePressureSensor->GetLastReading()
//      and compare this value to HiSILimit and LowSILimit
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void HFOAlarmTrigger::CheckSIAlarm()
{
    static USHORT highSICounter = 0;
    static USHORT lowSICounter = 0;
    LONG map;
    map = ActivePressureSensor->GetLastReading();
    if(map > MgrBdSetting::S_GetInstance()->GetCurrentValue(eSI) + HIGH_SI_OFFSET)
    {
        highSICounter++;
        if (highSICounter >5)
            if ( Alarm[ALARM_IDX(eHighSIPress)]->GetIsActive() != eActive)
                Alarm[ALARM_IDX(eHighSIPress)]->SetIsActive(eActive);

    }
    else
    {
        highSICounter = 0;
        if (Alarm[ALARM_IDX(eHighSIPress)]->GetIsActive() != eNotActive)
            Alarm[ALARM_IDX(eHighSIPress)]->SetIsActive(eNotActive);
    }

    if(map < MgrBdSetting::S_GetInstance()->GetCurrentValue(eSI) - LOW_SI_OFFSET)
    {
        lowSICounter++;
        if(lowSICounter>50)
            if ( Alarm[ALARM_IDX(eLowSIPress)]->GetIsActive() != eActive)
                Alarm[ALARM_IDX(eLowSIPress)]->SetIsActive(eActive);

    }
    else
    {
        lowSICounter = 0;
        if (Alarm[ALARM_IDX(eLowSIPress)]->GetIsActive() != eNotActive)
            Alarm[ALARM_IDX(eLowSIPress)]->SetIsActive(eNotActive);
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing:
//      DisableTrigger
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void HFOAlarmTrigger::DisableTrigger()
{
    Index = 0;			//counter for sample buffer
    IsFlatPattern = false;			//flag for flat pattern
    IsFlowOverLimit = false;;		//flag for flow over limit  if you have disconnect ET tube, flow patter have overshoot and flat, but overshoot is not stable.
    Counter500msec = 0;		//counter for 1sec
    FlowOverLimitCnt = 0;		//counter for flow over limit
    CheckCnt = 0;
    AlarmActiveCnt = 0;		//alarm active counter
    AlarmResetCnt = 0;
    for(int i =0; i<29;i++)
    {
        Flow_Buffer[i] = 0;
    }
    Trigger::DisableTrigger();
}

#endif // HFO_SYSTEM


