#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtInhFlowTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "ModeMgrMockHeader.h"

PtInhFlowTrigger::PtInhFlowTrigger (void) :
        Trigger (ePtInhFlowTrigger)
{
    PrevState = CurState = eOff;
    InUnrestrictedPhase = eFalse;
    timerCnt = 0;

}   // end PtInhFlowTrigger()
E_Bool PtInhFlowTrigger::CheckCondition (void)
{
    if(MgrBdSettingGetFlowTriggOn())
    {
        ProxyStatus Status = ProxyFlowSensorgetLastStatus();

        if((Status == eCalibrated)||(Status == eResetAndRecovered))
        {
            if(BDModeMgrGetCurrentModeId() == eAPRVMode)
            {
                TriggerDidFire = eTrue;//APRV_CheckCondition();
            }
            else
            {
                TriggerDidFire = eFalse;//IMV_CheckCondition();
            }
        }
    }
    return(TriggerDidFire);

}   // end CheckCondition()
E_Bool PtInhFlowTrigger::PreCheckCondition (void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    return(rtnValue);

}   // end PreCheckCondition()
void PtInhFlowTrigger::EnableTrigger (void)
{
    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.
    InUnrestrictedPhase = eFalse;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()
void PtInhFlowTrigger::PhaseInSettingChange (LONG value)
{
    // If the trigger is enabled, update the trigger sensitivity and the
    // bias flow
    if(eTrue == IsEnabled)
    {
        FlowLimit = 0 - value;

    }

}   // end PhaseInSettingChange()
E_Bool PtInhFlowTrigger::IMV_CheckCondition()
{
    const unsigned int NINE_SEC = 9000;
    const unsigned int THREE_SEC = 3000;
    const unsigned int SIX_SEC = 6000;
    const unsigned int STEP = 2;
    LONG triggerBaseLine = 0;

    if(timerCnt < NINE_SEC)//9 seconds
    {
        timerCnt += STEP;
    }


    if(timerCnt <= THREE_SEC)//3 seconds
    {
        triggerBaseLine = (LONG)((LONG)PtdataGetExhLeak()*SCALE/2);
    }
    else if(timerCnt <= SIX_SEC)//6 seconds
    {
        triggerBaseLine = (LONG)((LONG)PtdataGetExhLeak()*SCALE/4);
    }
    else
    {
        triggerBaseLine = 0;
    }


    E_Bool triggerDidFire = eFalse;

    UNSIGNED timeInh =  MgrBreathDataGetBdBreathDataPtrGetTimeInInh();
    if (ExhPhaseS_IsInUnrestrictedPhase() == eTrue)
    {
        LONG flow = ProxyFlowSensorGetLastReading();
        if(flow > BDMgrBdSettingGetCurrentValue(eFlowSens)+triggerBaseLine)
        {
            triggerDidFire = eTrue;
        }
    }
    return triggerDidFire;
}
E_Bool PtInhFlowTrigger::APRV_CheckCondition()
{
    E_Bool triggerDidFire = eFalse;
    E_PhaseId currentPhase = BDPhaseMgrGetCurrentPhaseId();

    UNSIGNED timeInh = 0;
    E_Bool IsInUnrestrictedPhase = eFalse;

    if(currentPhase == eAPRVHighPhase)
    {
        timeInh = MgrBreathDataGetBdBreathDataPtrGetTimeInExh();
        IsInUnrestrictedPhase = APRV_PHighPhaseS_IsInUnrestrictedPhase();
    }
    else if(currentPhase == eAPRVExhPhase_InPHigh)
    {
        timeInh = APRV_SupportedInhPhase_InPHighGetTimeElapsed();
        IsInUnrestrictedPhase = APRV_ExhPhase_InPHighS_IsInUnrestrictedPhase(timeInh);
    }
    else if(currentPhase == eAPRVLowPhase)
    {
        timeInh = 500;
        IsInUnrestrictedPhase = APRV_PLowPhaseS_IsInUnrestrictedPhase(timeInh);
    }
    else if(currentPhase == eAPRVExhPhase_InPLow)
    {
        timeInh = APRV_SupportedInhPhase_InPLowGetTimeElapsed();
        IsInUnrestrictedPhase = APRV_ExhPhase_InPLowS_IsInUnrestrictedPhase(timeInh);
    }


    LONG triggerBaseLine = 0;
    if(timerCnt<9000)//9 seconds
    {
        timerCnt += 2;
    }


    if(timerCnt<=3000)//3 seconds
    {
        triggerBaseLine = (LONG)((LONG)PtdataGetExhLeak()*SCALE/2);
    }
    else if(timerCnt<=6000)//6 seconds
    {
        triggerBaseLine = (LONG)((LONG)PtdataGetExhLeak()*SCALE/4);
    }
    else
    {
        triggerBaseLine = 0;
    }

    if (IsInUnrestrictedPhase == eTrue)
    {
        if(ProxyFlowSensorGetLastReading() > BDMgrBdSettingGetCurrentValue(eFlowSens)+triggerBaseLine)
        {
            triggerDidFire = eTrue;
        }
    }
    return triggerDidFire;
}
void PtInhFlowTrigger::DisableTrigger(void)
{

    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.
    InUnrestrictedPhase = eFalse;

    // Invoke Base Class Enable Method
    Trigger::DisableTrigger();

    timerCnt = 0;
}   // end EnableTrigger()

class PtInhFlowTriggerUnitMock : public BDTestFixture
{
public:
    PtInhFlowTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtInhFlowTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtInhFlowTrigger = new PtInhFlowTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtInhFlowTrigger;
    }
    PtInhFlowTrigger *MyPtInhFlowTrigger;
};

//test for PtInhFlowTrigger::PtInhFlowTrigger (void) :
TEST_F(PtInhFlowTriggerUnitMock, PtInhFlowTrigger)
{
    EXPECT_EQ(MyPtInhFlowTrigger->PrevState, eOff);
    EXPECT_EQ(MyPtInhFlowTrigger->CurState, eOff);
    EXPECT_EQ(MyPtInhFlowTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyPtInhFlowTrigger->timerCnt, 0);
}

//test for E_Bool PtInhFlowTrigger::CheckCondition (void)
TEST_F(PtInhFlowTriggerUnitMock, CheckCondition_0)
{
    MyPtInhFlowTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eAPRVMode));

    EXPECT_EQ(MyPtInhFlowTrigger->CheckCondition(), eTrue);
}
TEST_F(PtInhFlowTriggerUnitMock, CheckCondition_1)
{
    MyPtInhFlowTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eAPRVMode));

    EXPECT_EQ(MyPtInhFlowTrigger->CheckCondition(), eTrue);
}
TEST_F(PtInhFlowTriggerUnitMock, CheckCondition_2)
{
    MyPtInhFlowTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(ePcvACMode));

    EXPECT_EQ(MyPtInhFlowTrigger->CheckCondition(), eFalse);
}
TEST_F(PtInhFlowTriggerUnitMock, CheckCondition_3)
{
    MyPtInhFlowTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(ePcvACMode));

    EXPECT_EQ(MyPtInhFlowTrigger->CheckCondition(), eFalse);
}

//test for E_Bool PtInhFlowTrigger::PreCheckCondition (void)
TEST_F(PtInhFlowTriggerUnitMock, PreCheckCondition)
{
    EXPECT_EQ(MyPtInhFlowTrigger->PreCheckCondition(), eFalse);
}

//test for void PtInhFlowTrigger::EnableTrigger (void)
TEST_F(PtInhFlowTriggerUnitMock, EnableTrigger)
{
    MyPtInhFlowTrigger->EnableTrigger();

    EXPECT_EQ(MyPtInhFlowTrigger->InUnrestrictedPhase, eFalse);
}

//test for void PtInhFlowTrigger::PhaseInSettingChange (LONG value)
TEST_F(PtInhFlowTriggerUnitMock, PhaseInSettingChange)
{
    MyPtInhFlowTrigger->IsEnabled = eTrue;

    MyPtInhFlowTrigger->PhaseInSettingChange(1);

    EXPECT_EQ(MyPtInhFlowTrigger->FlowLimit, -1);
}
//test for void PtInhFlowTrigger::DisableTrigger(void)
TEST_F(PtInhFlowTriggerUnitMock, DisableTrigger)
{
    MyPtInhFlowTrigger->DisableTrigger();

    EXPECT_EQ(MyPtInhFlowTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyPtInhFlowTrigger->timerCnt, 0);
}
//test for E_Bool PtInhFlowTrigger::APRV_CheckCondition()
TEST_F(PtInhFlowTriggerUnitMock, APRV_CheckCondition)
{

}

//test for E_Bool PtInhFlowTrigger::IMV_CheckCondition()
TEST_F(PtInhFlowTriggerUnitMock, IMV_CheckCondition)
{

}

#endif //end define BD_TRIGGER_UNIT_TEST
