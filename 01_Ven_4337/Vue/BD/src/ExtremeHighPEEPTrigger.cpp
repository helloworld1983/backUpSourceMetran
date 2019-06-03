/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeHighPEEPTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ExtremeHighPEEPTrigger
/******************************************************************************/
#include <DeviceInterface.h>
#include "ExtremeHighPEEPTrigger.h"
#include "MgrBdSetting.h"
#include "PhaseMgr.h"
#include "ExhPhase.h"
#include "HighPressMode.h"
#include "AlarmTask.h"

ExtremeHighPEEPTrigger* ExtremeHighPEEPTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the Hip2AlarmTrigger object, it invokes
//      Hip2AlarmTrigger::S_GetInstance(), which returns a pointer
//      to the Hip2AlarmTrigger object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ExtremeHighPEEPTrigger* - pointer to the instance of this class
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
ExtremeHighPEEPTrigger* ExtremeHighPEEPTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ExtremeHighPEEPTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExtremeHighPEEPTrigger(void)
//
//    Processing: constructor
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//      None
//
/******************************************************************************/
ExtremeHighPEEPTrigger::ExtremeHighPEEPTrigger()
: Trigger(eExtremeHighPEEPTrigger)
{
    HighPressAlarm  = 20 * SCALE;
    InactiveTimer = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: Check condition and process action
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//      None
//
/******************************************************************************/
E_Bool ExtremeHighPEEPTrigger::CheckCondition(void)
{

    static const USHORT ABOVE15_PRESS_TIME = 15;
    static const USHORT ABOVE15_PRESS_TIME_NCPAP = 200;
    static LONG ExtremeHighPEEPCount = 0;
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    E_PhaseId phase = PhaseMgr::S_GetInstance()->GetCurrentPhaseId();
    LONG ExtremeHighPEEP_Offset =  mgrBdSettingPtr->GetCurrentValue(eExtremeHighPEEP_Autoset);

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    if(!((phase == eExhPhase) || (phase == eNCPAPPhase)))
    {
        TriggerDidFire = eFalse;
        return TriggerDidFire;
    }

    // Add NCPAP case here.
    if (phase == eNCPAPPhase)
    {
        HighPressAlarm = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighNCPAPLimit);

        LONG InhPressSensor;
        LONG ExhPressSensor;

        DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &InhPressSensor);
        DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &ExhPressSensor);

        if ((InhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset)||
                (ExhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset))
        {
            ExtremeHighPEEPCount++;
            if(ExtremeHighPEEPCount >= ABOVE15_PRESS_TIME_NCPAP)
            {
                TriggerDidFire = eTrue;
                HighPressMode::S_GetInstance()->SetCauseOfAlarm(eExtremeHighPeep);
                AlarmTask::S_GetInstance()->SetIsActive(eExtremeHighPeep, eActive);
            }
        }
        else
        {
            if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eExtremeHighPeep))
            {
                AlarmTask::S_GetInstance()->SetIsActive(eExtremeHighPeep, eNotActive);
            }
            ExtremeHighPEEPCount = 0;
        }
    }
    else if(phase == eExhPhase)
    {
        if(ExhPhase::S_CalcTimeSincePhaseStart() >= 500)//200ms
        {
            HighPressAlarm = mgrBdSettingPtr->GetCurrentValue(eHighBaselineLimit);

            LONG InhPressSensor;
            LONG ExhPressSensor;

            DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &InhPressSensor);
            DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &ExhPressSensor);

            if ((InhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset)||
                    (ExhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset))
            {
                ExtremeHighPEEPCount++;
                if(ExtremeHighPEEPCount >= ABOVE15_PRESS_TIME)
                {
                    ExtremeHighPEEPCount = 0;
                    // If the pressure condition is met, set the TriggerDidFire attribute
                    // and update the alarm status.

                    TriggerDidFire = eTrue;
                    HighPressMode::S_GetInstance()->SetCauseOfAlarm(eExtremeHighPeep);
                    AlarmTask::S_GetInstance()->SetIsActive(eExtremeHighPeep, eActive);
                }
            }
            else
            {
                if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eExtremeHighPeep))
                {
                    AlarmTask::S_GetInstance()->SetIsActive(eExtremeHighPeep, eNotActive);
                }
                ExtremeHighPEEPCount = 0;
            }
        }
    }

    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetInactiveTimer(USHORT value)
//
//    Processing: set inactive timer value
//
//    Input Parameters:
//      USHORT value - new value of Timer
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//      None
//
/******************************************************************************/
void ExtremeHighPEEPTrigger::SetInactiveTimer(USHORT value)
{
    InactiveTimer = value;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHighPressAlarm()
//
//    Processing: Get high pressure alarm
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//      None
//
/******************************************************************************/
LONG ExtremeHighPEEPTrigger::GetHighPressAlarm(void)
{
    return HighPressAlarm;
}
