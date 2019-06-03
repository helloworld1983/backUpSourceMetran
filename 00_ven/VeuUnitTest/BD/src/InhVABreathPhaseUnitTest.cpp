#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"

#include "DebugTrace.h"
#include "InhVABreathPhase.h"
using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "AlarmMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "ModeMgrMockHeader.h"

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

#define PLUS_DIR    1
#define MINUS_DIR   2
#define EVEN_DIR    0

InhVABreathPhase* InhVABreathPhase::S_Instance = NULL;

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

    mPrevPIPset =BDMgrBdSettingGetCurrentValue(ePcvInhPress);//for new VA
    mPrevVtset = BDMgrBdSettingGetCurrentValue(eTidalVolume);//for new VA
    mPrevBRset = BDMgrBdSettingGetCurrentValue(eRr);//for new VA
}
void InhVABreathPhase::Start(void)
{
    E_BreathPhaseIndicator breathType;

    breathType = (ModeS_IsInhPtTriggered() == eTrue) ? eAssisted : eMandatory;

//    PressInhPhase::Start();

//    mPIP = CompPIP();

//    CMVServoController->SetInhalationTarget(mPIP/10,RiseTime);

    // Get a handle to the Trigger Manager
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger);

    // Update patient data;
    MgrBreathDataInhalationStart(breathType);

    prevInhType = eMandInhVABreathPhase;
}
void InhVABreathPhase::End(void)
{
//    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

//    triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(eMandPressInhTimeTrigger);

    triggerMgrPtrDisableTrigger(eHip2AlarmTrigger);

    triggerMgrPtrDisableTrigger(eHipAlarmTrigger);
}
void InhVABreathPhase::SetPrevPIP (LONG prevPIP)
{
    mPrevPIP = prevPIP;
}
LONG InhVABreathPhase::GetPIPTarget (void)
{
    return mPIP;
}
void InhVABreathPhase::SetPrevInhType (void)
{
    prevInhType = eSpontInhPhase;
}
void InhVABreathPhase::ResetAlarmCnt (void)
{
    alarmHighCnt = 0;
    alarmLowCnt = 0;
}
LONG InhVABreathPhase::CompPIP()
{
    static LONG PIPcomp = 0;
    LONG PIPSetting = BDMgrBdSettingGetCurrentValue(ePcvInhPress);

    LONG VtTarget = BDMgrBdSettingGetCurrentValue(eTidalVolume);

    LONG BrSetting = BDMgrBdSettingGetCurrentValue(eRr);//for new VA

    E_VentilationMode currentMode = BDModeMgrGetCurrentModeId();

    LONG lowPIPAlarmSetting = BDMgrBdSettingGetCurrentValue(eLipLimit);
    LONG PEEPSetting = BDMgrBdSettingGetCurrentValue(eBaselinePress);
    LONG lowLimit = lowPIPAlarmSetting >= PEEPSetting ? lowPIPAlarmSetting : PEEPSetting;//for new VA

    LONG limit_one_breath;//add limnit //for new VA

    LONG errorVt = VtTarget - currentVte;
    E_PhaseId phaseId = BDPhaseMgrGetCurrentPhaseId();
#ifndef VA_WITH_PS
    //without PS control side
    if(currentMode == eVaMixedMode)
    {
        if(phaseId != eSpontInhPhase)
        {
//            currentVte =  PtdataGetAverageVte12();//for new VA
        }
        //case of Pressure support
        else
        {
            return mPrevPIP;
        }
    }
    else
    {
//        currentVte =  PtdataGetAverageVte12();//for new VA
    }
#else
    //with PS control side
    currentVte =  PtdataGetAverageVte12();//for new VA
#endif
    //if less than 5% it is margine
    if(100*abs(errorVt)/VtTarget < 5)
    {//for new VA
        errorVt = 0;
    }
    prev_Vt_error = 0;
    //get average amount volume of change
//    avg_chg_vol = ((prevVt_buf[2] - prevVt_buf[1]) + (prevVt_buf[1] - prevVt_buf[0]) + (prevVt_buf[0] - PtdataGetVte()))/3;

    prevVt_buf[2] = prevVt_buf[1];
    prevVt_buf[1] = prevVt_buf[0];
//    prevVt_buf[0] = PtdataGetLastMandExhVt();    //latest Vte


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
//        PtdataClearVteBuffer();
        mPrevPIPset=PIPSetting;
        mPrevVtset = VtTarget;
        mPrevBRset = BrSetting;
        prev_Vt_error = errorVt;
    }

    //get compliance
    double comp = PtdataGetAverageCompliance12();//for new VA

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

        if(3 > mTestBreathCount)
        {
            mTestBreathCount++;
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
            if(eHigh != BDAlarmTaskGetLevel(eOverLimitPressure))
            {
                BDAlarmTaskSetIsActive(eOverLimitPressure, eActive);
            }
        }
    }
    else
    {
        BDAlarmTaskSetIsActive(eOverLimitPressure, eNotActive);
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

            if(eHigh != BDAlarmTaskGetLevel(eLowerLimitPressure))
            {
                BDAlarmTaskSetIsActive(eOverLimitPressure, eActive);
            }
        }

    }
    else
    {
        BDAlarmTaskSetIsActive(eLowerLimitPressure, eNotActive);
        alarmLowCnt = 0;
    }


    mPrevPIP = PIP;

    return PIP;
}

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
void InhVABreathPhase::ProcessBdCycle()
{
    MgrBdSettingPhaseInPIPSetting();
}

class InhVABreathPhaseUnitMock : public BDTestFixture
{
public:
    InhVABreathPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~InhVABreathPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyInhVABreathPhase = new InhVABreathPhase(eNumBdSettings, eNumBdSettings);
    }
    virtual void TearDown(void)
    {
        delete MyInhVABreathPhase;
    }
    InhVABreathPhase *MyInhVABreathPhase;
};

//test for InhVABreathPhase* InhVABreathPhase::S_GetInstance(void)
TEST_F(InhVABreathPhaseUnitMock, S_GetInstance)
{
    MyInhVABreathPhase->S_GetInstance();

    EXPECT_NE(MyInhVABreathPhase->S_Instance, (void*)0);
}

//test for InhVABreathPhase::InhVABreathPhase
TEST_F(InhVABreathPhaseUnitMock, InhVABreathPhase)
{
    EXPECT_EQ(MyInhVABreathPhase->Vt_integral, 0);
    EXPECT_EQ(MyInhVABreathPhase->Vt_derivative, 0);
    EXPECT_EQ(MyInhVABreathPhase->prev_Vt_error, 0);
    EXPECT_EQ(MyInhVABreathPhase->IsFirstBreath, true);
    EXPECT_EQ(MyInhVABreathPhase->breathCounter, 0);
    EXPECT_EQ(MyInhVABreathPhase->currentVte, 0);
    EXPECT_EQ(MyInhVABreathPhase->PIP, 0);
    EXPECT_EQ(MyInhVABreathPhase->prevInhType, eMandInhVABreathPhase);

}

//test for void InhVABreathPhase::Start(void)
TEST_F(InhVABreathPhaseUnitMock, Start_0)
{
    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger)).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eAssisted)).Times(1);

    MyInhVABreathPhase->Start();
}
TEST_F(InhVABreathPhaseUnitMock, Start_1)
{
    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eFalse));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger)).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eMandatory)).Times(1);

    MyInhVABreathPhase->Start();
}

//test for void InhVABreathPhase::End(void)
TEST_F(InhVABreathPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr,  triggerMgrPtrDisableTrigger(eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr,  triggerMgrPtrDisableTrigger(eHip2AlarmTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr,  triggerMgrPtrDisableTrigger(eHipAlarmTrigger)).Times(1);

    MyInhVABreathPhase->End();
}

//test for void InhVABreathPhase::SetPrevPIP (LONG prevPIP)
TEST_F(InhVABreathPhaseUnitMock, SetPrevPIP)
{
    MyInhVABreathPhase->mPrevPIP = 10;

    MyInhVABreathPhase->SetPrevPIP(1);

    EXPECT_EQ(MyInhVABreathPhase->mPrevPIP, 1);
}

//test for LONG InhVABreathPhase::GetPIPTarget (void)
TEST_F(InhVABreathPhaseUnitMock, GetPIPTarget)
{
    MyInhVABreathPhase->mPIP = 10;

    EXPECT_EQ(MyInhVABreathPhase->GetPIPTarget(), 10);
}

//test for void InhVABreathPhase::SetPrevInhType (void)
TEST_F(InhVABreathPhaseUnitMock, SetPrevInhType)
{
    MyInhVABreathPhase->SetPrevInhType();

    EXPECT_EQ(MyInhVABreathPhase->prevInhType, eSpontInhPhase);
}

//test for void InhVABreathPhase::ResetAlarmCnt (void)
TEST_F(InhVABreathPhaseUnitMock, ResetAlarmCnt)
{
    MyInhVABreathPhase->ResetAlarmCnt();

    EXPECT_EQ(MyInhVABreathPhase->alarmHighCnt, 0);
    EXPECT_EQ(MyInhVABreathPhase->alarmLowCnt, 0);
}

//test for void InhVABreathPhase::ClearPIDParameter()
TEST_F(InhVABreathPhaseUnitMock, ClearPIDParameter)
{
    MyInhVABreathPhase->ClearPIDParameter();

    EXPECT_EQ(MyInhVABreathPhase->Vt_integral, 0);
    EXPECT_EQ(MyInhVABreathPhase->Vt_derivative, 0);
    EXPECT_EQ(MyInhVABreathPhase->prev_Vt_error, 0);
    EXPECT_EQ(MyInhVABreathPhase->IsFirstBreath, true);
    EXPECT_EQ(MyInhVABreathPhase->breathCounter, 0);
    EXPECT_EQ(MyInhVABreathPhase->avg_chg_vol, 0);
}

//test for void InhVABreathPhase::ProcessBdCycle()
TEST_F(InhVABreathPhaseUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInPIPSetting()).Times(1);

    MyInhVABreathPhase->ProcessBdCycle();
}

#endif //end define BD_PHASE_UNIT_TEST
