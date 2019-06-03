/******************************************************************************/
//$COMMON.CPP$
//   File Name:  InhTestBreathPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: InhVABreathPhase is the Mand Press Inh phase in VA mode
//
//
/******************************************************************************/
#include "InhVABreathPhase.h"
#include "TriggerMgr.h"
#include "MgrBreathData.h"
#include "ptdata.h"
#include "MgrBdSetting.h"
#include "AlarmTask.h"
#include "VA_SIMV_Mode.h"
#include "ModeMgr.h"

#define TWO_cmH20 200
#define ONE_cmH2O 100 //for new VA
#define P7_cmH2O 70 //for new VA
#define P4_cmH2O 40 //for new VA
#define Kp 20//30
#define Ki 9//25
#define Kd 10//15

#define Kp3 4 //for new VA
#define Ki3 2 //for new VA
#define Kd3 2 //for new VA
#define Kp2 7 //for new VA
#define Ki2 2 //for new VA
#define Kd2 2 //for new VA
#define Kp1 17 //for new VA
#define Ki1 4 //for new VA
#define Kd1 4 //for new VA
#define Kp0 20 //for new VA
#define Ki0 9 //for new VA
#define Kd0 10 //for new VA

#define PLUS_DIR	1
#define MINUS_DIR	2
#define EVEN_DIR	0

InhVABreathPhase* InhVABreathPhase::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
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
//      * - pointer to the object instantiated from this class
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
InhVABreathPhase* InhVABreathPhase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new InhVABreathPhase(ePcvInhPress,eBaselinePress);
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: InhVABreathPhase()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
InhVABreathPhase::InhVABreathPhase
(
    E_BdSettingId targetPressId,
    E_BdSettingId offsetPressId
    )
    : PressInhPhase(targetPressId, offsetPressId)
{
    mPrevPIP = 0.7*TargetPressure;
    mPIP = TargetPressure;

    Vt_integral = 0;
    Vt_derivative = 0;
    prev_Vt_error =0;
    IsFirstBreath = true;
    breathCounter = 0;
    currentVte = 0;
    PIP = 0;

    Vt_dir = EVEN_DIR;

    prevInhType = eMandInhVABreathPhase;
    prevVt_buf[0] = 0;
    prevVt_buf[1] = 0;
    prevVt_buf[2] = 0;
    alarmHighCnt = alarmLowCnt = 0;

    avg_chg_vol = 0;//for new VA
    gKp = Kp;//for new VA
    gKi = Ki;//for new VA
    gKd = Kd;//for new VA

    mPrevPIPset = MgrBdSetting::S_GetInstance()->GetCurrentValue(ePcvInhPress);//for new VA
    mPrevVtset = MgrBdSetting::S_GetInstance()->GetCurrentValue(eTidalVolume);//for new VA
    mPrevBRset = MgrBdSetting::S_GetInstance()->GetCurrentValue(eRr);//for new VA
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the start of every mandatory or supported
//      pressure based inhalation phase only in VA mode
//      This operation invokes the base class InhPhase::Start() operation to
//      perform processing common to all inhalation phases.  It then calls
//      MgrBdSetting::GetCurrentValue() to retrieve the inhalation, baseline
//      and offset pressures based upon the OffsetPressureId and
//      InhPressId attributes.
//
//      The Target Pressure, Initial Pressure  and Rise Time are determined by
//      this operation based upon operator settings.
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
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void InhVABreathPhase::Start(void)
{
    E_BreathPhaseIndicator breathType;

    breathType = (Mode::S_IsInhPtTriggered() == eTrue) ? eAssisted : eMandatory;

    PressInhPhase::Start();

    mPIP = CompPIP();

//    CMVServoController->SetInhalationTarget(mPIP/10,RiseTime);

    // Get a handle to the Trigger Manager
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->EnableTrigger(eMandPressInhTimeTrigger);

    // Update patient data;
    (MgrBreathData::S_GetInstance())->InhalationStart(breathType);

    prevInhType = eMandInhVABreathPhase;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void InhVABreathPhase::End(void)
{
    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableTrigger(eMandPressInhTimeTrigger);

    triggerMgrPtr->DisableTrigger(eHip2AlarmTrigger);

    triggerMgrPtr->DisableTrigger(eHipAlarmTrigger);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetPrevPIP (LONG prevPIP)
//
//    Processing: Set previous PIP
//
//
//    Input Parameters:
//      LONG prevPIP - PIP value
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
void InhVABreathPhase::SetPrevPIP (LONG prevPIP)
{
    mPrevPIP = prevPIP;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: Get PIP target
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
LONG InhVABreathPhase::GetPIPTarget (void)
{
    return mPIP;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void InhVABreathPhase::SetPrevInhType (void)
{
    prevInhType = eSpontInhPhase;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void InhVABreathPhase::ResetAlarmCnt (void)
{
    alarmHighCnt = 0;
    alarmLowCnt = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompPIP()
//
//    Processing:
//      This Function uses PID method to compute the PIP, the target is Tidal Volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
LONG InhVABreathPhase::CompPIP()
{
    static LONG PIPcomp = 0;
    LONG PIPSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(ePcvInhPress);

    LONG VtTarget = MgrBdSetting::S_GetInstance()->GetCurrentValue(eTidalVolume);

    LONG BrSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eRr);//for new VA

    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();

    LONG lowPIPAlarmSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLipLimit);
    LONG PEEPSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaselinePress);
    LONG lowLimit = lowPIPAlarmSetting >= PEEPSetting ? lowPIPAlarmSetting : PEEPSetting;//for new VA

    LONG limit_one_breath;//add limnit //for new VA
   
    LONG errorVt = VtTarget - currentVte;
    E_PhaseId phaseId = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();
#ifndef VA_WITH_PS
    //without PS control side
    if(currentMode == eVaMixedMode)
    {
        if(phaseId != eSpontInhPhase)
        {
            currentVte =  Ptdata::S_GetInstance()->GetAverageVte12();//for new VA
        }
        //case of Pressure support
        else
        {
            return mPrevPIP;
        }
    }
    else
    {
        currentVte =  Ptdata::S_GetInstance()->GetAverageVte12();//for new VA
    }
#else
    //with PS control side
    currentVte =  Ptdata::S_GetInstance()->GetAverageVte12();//for new VA
#endif
    //if less than 5% it is margine
    if(100*abs(errorVt)/VtTarget < 5)
    {//for new VA
        errorVt = 0;
    }
    prev_Vt_error = 0;
    //get average amount volume of change
    avg_chg_vol = ((prevVt_buf[2] - prevVt_buf[1]) + (prevVt_buf[1] - prevVt_buf[0]) + (prevVt_buf[0] - Ptdata::S_GetInstance()->GetVte()))/3;

    prevVt_buf[2] = prevVt_buf[1];
    prevVt_buf[1] = prevVt_buf[0];
    prevVt_buf[0] = Ptdata::S_GetInstance()->GetLastMandExhVt();    //latest Vte


    //Judge of Vte direction
    if(prevVt_buf[0]/10>prevVt_buf[1]/10 && prevVt_buf[1]/10>prevVt_buf[2]/10)
    {
        //increase direction of Vt
        Vt_dir = PLUS_DIR;
    }
    else if(prevVt_buf[0]/10<prevVt_buf[1]/10 && prevVt_buf[1]/10<prevVt_buf[2]/10)
    {
        //decrease direction of Vt
        Vt_dir = MINUS_DIR;
    }
    else
    {
        //stable direction of Vt
        Vt_dir = EVEN_DIR;
    }

    //for new VA
    //braking function by predict amount of change
    if(prevVt_buf[0] != 0 && prevVt_buf[1] != 0&&prevVt_buf[2] != 0)
    {
        //case of minus direction
        if(Vt_dir == MINUS_DIR)
        {
            if(VtTarget + (VtTarget*5/100) > prevVt_buf[0] - avg_chg_vol)
            {
                //5% margine example target 1000 less than 1050 use prediction error for PID control break
                errorVt = (VtTarget+ (VtTarget*5/100)) -(prevVt_buf[0] - avg_chg_vol);
            }
        }
        //case of plus direction
        else if(Vt_dir == PLUS_DIR)
        {
            if(VtTarget - (VtTarget*5/100) < prevVt_buf[0] + avg_chg_vol)
            {
                //5% margine example target 100 greater than 950 use prediction error for PID control break
                errorVt = (VtTarget- (VtTarget*5/100)) -(prevVt_buf[0] + avg_chg_vol);
            }
        }
    }

    //for new VA
    //clear data when PIP, Vt, Br setting change
    if(mPrevPIPset!=PIPSetting || mPrevVtset != VtTarget || mPrevBRset !=BrSetting)
    {
        Ptdata::S_GetInstance()->ClearVteBuffer();
        mPrevPIPset=PIPSetting;
        mPrevVtset = VtTarget;
        mPrevBRset = BrSetting;
        prev_Vt_error = errorVt;
    }

    //get compliance 
    double comp = Ptdata::S_GetInstance()->GetAverageCompliance12();//for new VA

    //for new VA
    //change the coeffient by compliance
    //comp 500 = 5.00ml/cmH2O
    if(comp > 500){
        gKp = Kp3;
        gKi = Ki3;
        gKd = Kd3;
    }
    else if(comp >= 300)
    {
        gKp = Kp2;
        gKi = Ki2;
        gKd = Kd2;
    }
    else if(comp >= 100)
    {
        gKp = Kp1;
        gKi = Ki1;
        gKd = Kd1;
    }
    else
    {
        gKp = Kp0;
        gKi = Ki0;
        gKd = Kd0;
    }
    
    if(mPrevPIP <= 0)
    {
        if (errorVt < 0)
        {
            errorVt = -errorVt;
        }
    }
    if(100*abs(VtTarget-prevVt_buf[0])/VtTarget < 5)
    {
        errorVt = 0;
        prev_Vt_error = 0;
    }
    //for new VA
    if(IsFirstBreath)
    {
        PIP = TargetPressure*0.7;

        if(3 > VA_AC_Mode::S_GetInstance()->mTestBreathCount)
        {
            VA_AC_Mode::S_GetInstance()->mTestBreathCount++;
            mPrevPIP = PIP;
            return PIP;
        }
        else
        {
            Vt_derivative = errorVt;
            Vt_integral = (LONG)((100.0*PIP - (gKp+gKd)*errorVt)/gKi);
            IsFirstBreath = false;
        }
    }
    else
    {
        Vt_integral = Vt_integral + errorVt*2;
        Vt_derivative = (errorVt - prev_Vt_error)/2;
    }

    if(PIPcomp < 0)
    {
        Vt_integral = 0;
    }

    PIP = (gKp*errorVt + gKi*Vt_integral + gKd*Vt_derivative)/100.0;//for new VA

    PIPcomp = PIP;

    prev_Vt_error = errorVt;

    //for new VA
    if(comp > 500)
    {
        limit_one_breath = P4_cmH2O;
    }
    else if(comp > 300)
    {
        limit_one_breath = P7_cmH2O;
    }
    else if(comp > 100)
    {
        limit_one_breath = ONE_cmH2O;
    }
    else
    {
        limit_one_breath = TWO_cmH20;
    }

    //for new VA
   //one breath changable limitation, should be continuous control. so PID does not cancelling
   if(abs(PIP-mPrevPIP) > limit_one_breath)
   {
        PIP = mPrevPIP + ((PIP-mPrevPIP)/(abs(PIP-mPrevPIP)))*limit_one_breath;

        Vt_derivative = errorVt =0;
        Vt_integral = (LONG)((100.0*PIP - (gKp+gKd)*errorVt)/gKi); // cancelling compensate to PID control, should be stop control at this condition.

    }

    if(PIP > PIPSetting)
    {
        //for new VA
        Vt_derivative = errorVt = 0;
        Vt_integral = (LONG)((100.0*PIP - (gKp+gKd)*errorVt)/gKi); // cancelling compensate to PID control, should be stop control at this condition.

        PIP = PIPSetting;

        if(alarmHighCnt < 6)
            alarmHighCnt++;

        if(alarmHighCnt >= 3)
        {
            if(eHigh != AlarmTask::S_GetInstance()->GetLevel(eOverLimitPressure))
            {
                AlarmTask::S_GetInstance()->SetIsActive(eOverLimitPressure, eActive);
            }
        }
    }
    else
    {
        AlarmTask::S_GetInstance()->SetIsActive(eOverLimitPressure, eNotActive);
        alarmHighCnt = 0;
    }

    if(PIP < lowLimit)
    {
        if(alarmLowCnt < 6)
            alarmLowCnt++;
        Vt_derivative = errorVt=0;//for new VA
        Vt_integral = (LONG)((100.0*PIP - (gKp+gKd)*errorVt)/gKi);  // cancelling compensate to PID control, should be stop control at this condition.
        PIP=lowLimit;
        
        if(alarmLowCnt>=3)
        {

            if(eHigh != AlarmTask::S_GetInstance()->GetLevel(eLowerLimitPressure))
            {
                AlarmTask::S_GetInstance()->SetIsActive(eOverLimitPressure, eActive);
            }
        }

    }
    else
    {
        AlarmTask::S_GetInstance()->SetIsActive(eLowerLimitPressure, eNotActive);
        alarmLowCnt = 0;
    }


    mPrevPIP = PIP;

    return PIP;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearPIDParameter()
//
//    Processing:
//      This function clears all static value used by PID method in CompPIP() function
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
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void InhVABreathPhase::ClearPIDParameter()
{
    Vt_integral = 0;
    Vt_derivative = 0;
    prev_Vt_error =0;
    IsFirstBreath = true;
    breathCounter=0;
    //variable for Vt direction
    Vt_dir = EVEN_DIR;
    prevVt_buf[0] = 0;
    prevVt_buf[1] = 0;
    prevVt_buf[2] = 0;
    avg_chg_vol = 0;//for new VA
    
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void InhVABreathPhase::ProcessBdCycle()
{
    MgrBdSetting::S_GetInstance()->PhaseInPIPSetting();
}

