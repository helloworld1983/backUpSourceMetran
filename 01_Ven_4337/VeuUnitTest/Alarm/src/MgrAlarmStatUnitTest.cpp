#include "EnableTestCase.h"
#ifdef ALARM_UNIT_TEST

#include "gtest.h"
#include "CommonTypes.h"
#include "AlarmInterface.h"
#include "MgrAlarmStat.h"

using namespace ::testing;

static int AlarmCounter[NUMALARMLEVELS];

MgrAlarmStat* MgrAlarmStat::S_Instance = NULL;

static AlarmControl *cntrPtr = NULL;

inline MgrAlarmStat* MgrAlarmStat::S_GetInstance (void)
{
    if(S_Instance == NULL)
    {
        S_Instance = new MgrAlarmStat ();
    }

    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

void MgrAlarmStat::StatusUpdate ()

{
    register int i;

    for (i = 0; i < NUMALARMS; i++)
    {
        // don't process alarms that
        if(Alarm[i]->Processed == eTrue)
            continue;

        // skip eHighInspPressure only process HIP1 and HIP2
        if(ALARM_IDX(eHighInspPressure) == i)
            continue;

        if(CorruptStatus (i))
        {
            Alarm[i]->Processed = eTrue;
            continue;
        }

        CheckStatus (i);

        CheckElevation (i);

        Alarm[i]->Processed = eTrue;
    }
}

E_Bool MgrAlarmStat::NonSilenceableActive (void)
{
    E_Bool retval;

    if ((Alarm[ALARM_IDX(eOcclusion)]->GetIsActive() == eNotActive) &&
            (Alarm[ALARM_IDX(eHighInspPressure)]->GetIsActive() == eNotActive) &&
#ifdef HFO_SYSTEM
            (Alarm[ALARM_IDX(eExtremeHighMAP)]->GetIsActive() == eNotActive) &&
#endif
            (Alarm[ALARM_IDX(eGasSuppliesLost)]->GetIsActive() == eNotActive))
    {
        retval = eFalse;
    }
    else
    {
        retval = eTrue;
    }
    return(retval);
}

void MgrAlarmStat::Enunciate (int i)
{
    //    AlarmStatusGUIEvent AlarmMsg;

    UCHAR ind = 0;

    // build alarm count indicator to generate correct response
    ind = BuildIndicator();

    switch (ind)
    {
        //****************** no alarm *******************
        case 0:
            currentAlarm = eInactive;
            //        AlarmMsg.Sound = eNoSound;
            //        pendingSound = eNoSound;
            break;
        case 1:
            currentAlarm = eAutoReset; //prevAlarm;
            //
            //        AlarmMsg.Sound = eNoSound;
            //        pendingSound = eNoSound;
            break;

            // ****************** low alarm *****************

        case 2:
        case 3:
            currentAlarm = eLow;
            //        pendingSound = eLowSound;
            //        if (cntrPtr->GetSilenceButtonStatus())
            //            AlarmMsg.Sound = eNoSound;
            //        else
            //            AlarmMsg.Sound = eLowSound;

            break;

            // ******************* medium alarm ***************
        case 4:
        case 5:
        case 6:
        case 7:
            currentAlarm = eMedium;
            //        pendingSound = eMediumSound;
            //        if(cntrPtr->GetSilenceButtonStatus())
            //            AlarmMsg.Sound = eNoSound;
            //        else
            //            AlarmMsg.Sound = eMediumSound;
            //
            //        break;
            // ******************** high alarm *****************
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            currentAlarm = eHigh;
            //        pendingSound = eHighSound;
            //        if(cntrPtr->GetSilenceButtonStatus())
            //            AlarmMsg.Sound = eNoSound;
            //        else
            //            AlarmMsg.Sound = eHighSound;

            break;
        default:
            // should never reach this statement!
            ASSERTION(eFalse);
            break;

    } // switch(ind)

    //control led alarm
    //    AlarmLedCtrl->doLedAlarm(currentAlarm);

    if (i != NONEWALARMS)
    {
        // Build Alarm Msg
        //        AlarmMsg.Status = Alarm[i]->Level;
        //        AlarmMsg.PreStatus = Alarm[i]->PreLevel;
        // send message and wait
        //        AlarmMsg.ErrNumber = Alarm[i]->ErrNumber;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<AlarmStatusGUIEvent>(Alarm[i]->Id, AlarmMsg);

    }

}

void MgrAlarmStat::DoReset (void)
{
    //    PdAlarms::S_GetInstance()->ResetAlarmActiveTime();
    //    ApneaAlarmTimeTrigger::S_GetInstance()->ReEnableTrigger();
    //    AzTask::S_GetInstance()->ClearAlarmCnt();
    //    InhVABreathPhase::S_GetInstance()->ResetAlarmCnt();
    register int i;

    for (i = 0; i < NUMALARMS; i++)
    {
        if(i != ALARM_IDX(e100percentO2Enable)) // do not reset e100percentO2Enable
        {
            if (Alarm[i]->Level != eInactive)
            {
                MakeInactive(i);
                Enunciate(i);
            }
        }
        else
        {
            if(( Alarm[i]->IsActive == eNotActive)&&(Alarm[i]->Level != eInactive ))
            {
                MakeInactive(i);
                Enunciate(i);
            }
        }
    }
    /* When push and hold the mute alarm widget the alarm led need to turn off */
    //    AlarmLedCtrl->doLedAlarm(eInactive);
}

MgrAlarmStat::MgrAlarmStat ()
{
    register int i;

    //  guiPtr = GUITask::S_GetInstance();
    //  ASSERTION (guiPtr != NULL);

    cntrPtr = AlarmControl::S_GetInstance ();
    ASSERTION (cntrPtr != NULL);

    //  sndPtr = Audible::S_GetInstance();
    //  ASSERTION (sndPtr != NULL);

    for (i = 0; i < NUMALARMLEVELS; i++)
    {
        AlarmCounter[i] = 0;
    }

    AlarmCounter[eInactive] = NUMALARMS;

    // turn off horns and LEDs
    Enunciate (NONEWALARMS);

}

E_Bool MgrAlarmStat::CorruptStatus (int i)
{
    E_Bool retval;

    if ( (Alarm[i]->IsActive != eActive && Alarm[i]->IsActive != eNotActive) ||
            (Alarm[i]->OldIsActive != eActive && Alarm[i]->OldIsActive != eNotActive))
    {
        Alarm[i]->IsActive = eNotActive;
        Alarm[i]->OldIsActive = eNotActive;
        retval = eTrue;
    }
    else
    {
        retval = eFalse;
    }

    return(retval);
}

void MgrAlarmStat::CheckElevation (int i)
{
    // skip alarms without timers

    if (Alarm[i]->AlarmTimer != NULL)
    {
        if (Alarm[i]->AlarmTimer->ElevationFlag)
        {
            // subtract one from current alarm counter

            AlarmCounter[Alarm[i]->Level]--;

            // elevate the current alarm
            ElevateAlarm(i);

            // add one to the current alarm   counter
            AlarmCounter[Alarm[i]->Level]++;

            // indicate the new change
            Enunciate(i);

            Alarm[i]->AlarmTimer->ElevationFlag = eFalse;

            // if alarm is not at its final priority
            // reset it's timer
            if (Alarm[i]->Level != Alarm[i]->FinalPriority)
            {
                //                Alarm[i]->AlarmTimer->Reset(ELEVATIONTIME*TICKS_PER_SEC,0,TRUE);
                Alarm[i]->AlarmTimer->Reset();
            }
        }
    }

}

void MgrAlarmStat::CheckStatus (int i)
{
    //    LONG alarmId;
    LONG adex;       // alarm index

    //    alarmId = eFirstAlarmType + i;
    adex = i;

    // if status changed or a HIP occurred
    if (Alarm[i]->IsActive != Alarm[i]->OldIsActive)
    {
        AlarmCounter[Alarm[adex]->Level]--;

        if (Alarm[i]->IsActive == eActive)
        {
            ActivateAlarm(i);
        }
        else
        {
            DeactivateAlarm(i);
        }

        Alarm[i]->OldIsActive = Alarm[i]->IsActive;

        AlarmCounter[Alarm[adex]->Level]++;

        Enunciate(adex);

    }
}
void MgrAlarmStat::ActivateAlarm (int i)

{
    //    LONG alarmId;
    //
    //    alarmId = eFirstAlarmType + i;

    // process all other alarms
    Alarm[i]->Level = Alarm[i]->InitialPriority;

    // if it is elevatable start timer
    if (Alarm[i]->AlarmTimer != NULL)
    {
        //        Alarm[i]->AlarmTimer->Reset(ELEVATIONTIME*TICKS_PER_SEC,0,eTrue);
    }

}

void MgrAlarmStat::DeactivateAlarm (int i)
{
    //    LONG alarmId;

    //    alarmId = eFirstAlarmType + i;

    if (Alarm[i]->AlarmTimer)
    {
        Alarm[i]->AlarmTimer->Stop();
        Alarm[i]->AlarmTimer->ElevationFlag = eFalse;
    }
    Alarm[i]->PreLevel = Alarm[i]->Level;
    Alarm[i]->Level = eAutoReset;
}

void MgrAlarmStat::ElevateAlarm (int i)
{
    if(Alarm[i]->Level == eLow)
    {
        Alarm[i]->Level = eMedium;
    }
    else if(Alarm[i]->Level == eMedium)
    {
        Alarm[i]->Level = eHigh;
    }
}

UCHAR MgrAlarmStat::BuildIndicator ()
{
    struct
    {
        UCHAR rset :1;      // bit 0
        UCHAR low :1;       // bit 1
        UCHAR med :1;       // bit 2
        UCHAR hi :1;        // bit 3
    } ind;

    // overlay indicator and bit fields

    UCHAR *indicator = (UCHAR *) &ind;

    // clear all bits with one shot

    *indicator = 0;

    if(AlarmCounter[eHigh])
        ind.hi = 1;
    if(AlarmCounter[eMedium])
        ind.med = 1;
    if(AlarmCounter[eLow])
        ind.low = 1;
    if(AlarmCounter[eAutoReset])
        ind.rset = 1;

    return (*indicator);

}

void MgrAlarmStat::DoSilence (void)
{
    // get silence time from system NVRAM,
    // future enhancement not done currently
    //  stat = Reset(SILENCETIME*TICKS_PER_SEC, 0, TRUE);

    cntrPtr->SetSilenceButtonStatus(eTrue);

    // Turn on Silence LED call to DataIO
    //  AlarmSilenceLED->SetState(eOn);

    Enunciate (NONEWALARMS);
}

void MgrAlarmStat::ClearAllAlarm (void)
{

    for (int i = eFirstAlarmType; i <= eLastAlarmType; i++)
    {
        Alarm[ALARM_IDX(i)]->SetIsActive(eNotActive);
    }
}

void MgrAlarmStat::ClearHFOAlarm (void)
{
//    for (int i = eFirstHFOAlarmType; i <= eLastHFOAlarmType; i++)
//    {
//        if ( eActive == Alarm[ALARM_IDX(i)]->IsActive)
//        {
//            Alarm[ALARM_IDX(i)]->SetIsActive(eNotActive);
//        }
//
//    }
}

void MgrAlarmStat::ClearIMVAlarm (void)
{
//    for (int i = eFirstIMVAlarmType; i <= eLastIMVAlarmType; i++)
//    {
//        Alarm[ALARM_IDX(i)]->SetIsActive(eNotActive);
//    }
}

void MgrAlarmStat::MakeInactive (int i)
{

    if(Alarm[i]->Level == eInactive)
        return;

    if(Alarm[i]->AlarmTimer)
    {
        //      ASSERTION(Alarm[i]->AlarmTimer->Disable() == NU_SUCCESS);
        Alarm[i]->AlarmTimer->ElevationFlag = eFalse;
    }
    Alarm[i]->Processed = eTrue;

    // if HIP1 or HIP2 don't update counters

    //    if(Hip (eFirstAlarmType + i) == eFalse)
    //    {
    //        AlarmCounter[Alarm[i]->Level]--;
    //        AlarmCounter[eInactive]++;
    //    }

    Alarm[i]->Level = eInactive;
    Alarm[i]->IsActive = eNotActive;
    Alarm[i]->OldIsActive = eNotActive;

}

#endif //end define ALARM_UNIT_TEST
