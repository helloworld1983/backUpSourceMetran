#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtExhFlowSuppTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"

PtExhFlowSuppTrigger::PtExhFlowSuppTrigger(void):Trigger(ePtExhFlowSuppTrigger)
{
    BiasFlow = 10;
    IsConditionMet = eFalse;
    TimeConditionFirstMet = 0;
    PrevFilteredDelFlow = 0;
    PrevFilteredProxFlow = 0;
}   // end PtExhFlowSuppTrigger()
E_Bool PtExhFlowSuppTrigger::CheckCondition(void)
{
    const UNSIGNED INIT_FLOW_CRITERIA_PHASE = 200/MS_PER_TICK;  // 200 ms in TICKS

    LONG filteredDelFlow;
    LONG filteredProxFlow;
    USHORT exhSens;

    // Get ta pointer to the MgrBreathData object
//    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance();

    //biasFlow (ml/s * SCALE)
    exhSens = BDMgrBdSettingGetCurrentValue(eExhSens)/100;
    BiasFlow = BDMgrBdSettingGetCurrentValue(eBaseFlow);

    // Filter the flow value using the equation:
    // y(n) = 0.5 * x(n) + 0.5 * y(n-1)
    LONG DeliveredFlow = mgrBreathDataPtrGetDeliveredFlow();
    LONG ProxyFlow = mgrBreathDataPtrGetProxyFlow();
    filteredDelFlow = (PrevFilteredDelFlow >> 1) + (DeliveredFlow >> 1);
    filteredProxFlow = (PrevFilteredProxFlow >> 1) + (ProxyFlow >> 1);

    // Update the Previous value to prepare for the next cycle
    PrevFilteredDelFlow = filteredDelFlow;
    PrevFilteredProxFlow = filteredProxFlow;

    // Check if the time elapsed since the start of inhalation is greater
    // than or equal to the initial phase.  During the initial part of
    // inhalation, the criteria that cause this trigger to fire must be
    // met for a minimum amount of time.  But after the initial
    // time period, the trigger criteria is met right away, so set
    // the TriggerDidFire attribute
    if (InhPhaseS_CalcTimeSincePhaseStart() >= INIT_FLOW_CRITERIA_PHASE)
    {
        // Compare the delivered flow for the current cycle to the exhalation
        // sensitivity (percentage) of the peak delivered flow.
        // The divide by 100 is needed since ExhSens is a percentage.
        LONG limit=0;
        LONG PeakFlow = mgrBreathDataPtrGetPeakDeliveredFlow();

        E_VentilationMode currenMode = BDModeMgrGetCurrentModeId();

        if (currenMode == eNivMode)
        {
            limit = ((exhSens * (PeakFlow - BiasFlow)) / 100) + BiasFlow;

            if (filteredDelFlow < limit)
            {
                TriggerDidFire = eTrue;
            }
        }
        else
        {
            ProxyStatus Status = ProxyFlowSensorgetLastStatus();

            if((Status == eCalibrated) || (Status == eResetAndRecovered))
            {
                LONG leak = PtdataS_GetInstanceGetExhLeak()*SCALE;
                PeakFlow = mgrBreathDataPtrGetProxyFlow();
                limit = ((exhSens * (PeakFlow - leak)) / 100) + leak;

                if (filteredProxFlow < limit)
                {
                    TriggerDidFire = eTrue;
                }
            }
            else
            {
                limit = ((exhSens * (PeakFlow - BiasFlow)) / 100) + BiasFlow;

                if (filteredDelFlow < limit)
                {
                    TriggerDidFire = eTrue;
                }
            }
        }
    }
    return(TriggerDidFire);

}   // end CheckCondition()

void PtExhFlowSuppTrigger::EnableTrigger(void)
{
    // Initialize the previous filtered flow value
    PrevFilteredDelFlow = 0;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()
void PtExhFlowSuppTrigger::PhaseInSettingChange(LONG value)
{
    // Update the trigger sensitivity
    BiasFlow = (value * 100) / 6;
    LONG BiasValue = value / 10;
    BDRequestDevice(eCMVServoControllerSetBiasFlow);

}// end PhaseInSettingChange()

class PtExhFlowSuppTriggerUnitMock : public BDTestFixture
{
public:
    PtExhFlowSuppTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtExhFlowSuppTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtExhFlowSuppTrigger = new PtExhFlowSuppTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtExhFlowSuppTrigger;
    }
    PtExhFlowSuppTrigger *MyPtExhFlowSuppTrigger;
};

//test for PtExhFlowSuppTrigger::PtExhFlowSuppTrigger(void):Trigger(ePtExhFlowSuppTrigger)
TEST_F(PtExhFlowSuppTriggerUnitMock, PtExhFlowSuppTrigger)
{
    EXPECT_EQ(MyPtExhFlowSuppTrigger->BiasFlow, 10);
    EXPECT_EQ(MyPtExhFlowSuppTrigger->IsConditionMet, eFalse);
    EXPECT_EQ(MyPtExhFlowSuppTrigger->TimeConditionFirstMet, 0);
    EXPECT_EQ(MyPtExhFlowSuppTrigger->PrevFilteredDelFlow, 0);
    EXPECT_EQ(MyPtExhFlowSuppTrigger->PrevFilteredProxFlow, 0);
}

//test for E_Bool PtExhFlowSuppTrigger::CheckCondition(void)
TEST_F(PtExhFlowSuppTriggerUnitMock, CheckCondition_0)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eExhSens)).Times(1).WillOnce(Return(100));
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetProxyFlow()).Times(1).WillOnce(Return(2));

    MyPtExhFlowSuppTrigger->PrevFilteredDelFlow = 2;
    MyPtExhFlowSuppTrigger->PrevFilteredProxFlow = 2;

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(200/MS_PER_TICK));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(201));
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eNivMode));


    EXPECT_EQ(MyPtExhFlowSuppTrigger->CheckCondition(), eTrue);
}
TEST_F(PtExhFlowSuppTriggerUnitMock, CheckCondition_1)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eExhSens)).Times(1).WillOnce(Return(100));
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetProxyFlow()).Times(2)
            .WillOnce(Return(2))
            .WillOnce(Return(2000));

    MyPtExhFlowSuppTrigger->PrevFilteredDelFlow = 2;
    MyPtExhFlowSuppTrigger->PrevFilteredProxFlow = 2;

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(200/MS_PER_TICK));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eApneaBackupMode));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_BreathMgr, PtdataS_GetInstanceGetExhLeak()).Times(1).WillOnce(Return(1/SCALE));
//    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetProxyFlow()).Times(1).WillOnce(Return(2000));


    EXPECT_EQ(MyPtExhFlowSuppTrigger->CheckCondition(), eTrue);
}
TEST_F(PtExhFlowSuppTriggerUnitMock, CheckCondition_2)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eExhSens)).Times(1).WillOnce(Return(100));
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetProxyFlow()).Times(2)
            .WillOnce(Return(2))
            .WillOnce(Return(2000));

    MyPtExhFlowSuppTrigger->PrevFilteredDelFlow = 2;
    MyPtExhFlowSuppTrigger->PrevFilteredProxFlow = 2;

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(200/MS_PER_TICK));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eApneaBackupMode));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_BreathMgr, PtdataS_GetInstanceGetExhLeak()).Times(1).WillOnce(Return(1/SCALE));
//    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetProxyFlow()).Times(1).WillOnce(Return(2000));


    EXPECT_EQ(MyPtExhFlowSuppTrigger->CheckCondition(), eTrue);
}
TEST_F(PtExhFlowSuppTriggerUnitMock, CheckCondition_3)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eExhSens)).Times(1).WillOnce(Return(100));
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetProxyFlow()).Times(1)
            .WillOnce(Return(2));


    MyPtExhFlowSuppTrigger->PrevFilteredDelFlow = 2;
    MyPtExhFlowSuppTrigger->PrevFilteredProxFlow = 2;

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(200/MS_PER_TICK));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(2001));
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eApneaBackupMode));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));


    EXPECT_EQ(MyPtExhFlowSuppTrigger->CheckCondition(), eTrue);
}

//test for void PtExhFlowSuppTrigger::EnableTrigger(void)
TEST_F(PtExhFlowSuppTriggerUnitMock, EnableTrigger)
{
    MyPtExhFlowSuppTrigger->EnableTrigger();

    EXPECT_EQ(MyPtExhFlowSuppTrigger->PrevFilteredDelFlow, 0);
}

//test for void PtExhFlowSuppTrigger::PhaseInSettingChange(LONG value)
TEST_F(PtExhFlowSuppTriggerUnitMock, PhaseInSettingChange)
{
    MyPtExhFlowSuppTrigger->PhaseInSettingChange(6);

    EXPECT_EQ(MyPtExhFlowSuppTrigger->BiasFlow, (6*100) / 6);
}

#endif //end define BD_TRIGGER_UNIT_TEST
