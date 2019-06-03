#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ProxyDisconnectTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "StepMotorMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "ModeMgrMockHeader.h"

ProxyDisconnectTrigger::ProxyDisconnectTrigger():Trigger(eProxyDisconnectTrigger)
{
}
E_Bool ProxyDisconnectTrigger::CheckCondition()
{
    ProxyStatus Status = ProxyFlowSensorgetLastStatus();

    if((Status ==eCalibrated)||(Status==eResetAndRecovered))
    {
        TriggerDidFire = eTrue;
    }
    else
    {
        TriggerDidFire = eFalse;
    }

    return TriggerDidFire;
}
void ProxyDisconnectTrigger::ProcessTrigger(Mode* mode)
{
    DisableTrigger();
    TriggerMgrEnableTrigger(eProxyDisconnectTrigger);
    modeProcessTriggerID(eProxyDisconnectTrigger);

}

class ProxyDisconnectTriggerUnitMock : public BDTestFixture
{
public:
    ProxyDisconnectTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ProxyDisconnectTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyProxyDisconnectTrigger = new ProxyDisconnectTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyProxyDisconnectTrigger;
    }
    ProxyDisconnectTrigger *MyProxyDisconnectTrigger;
};

//test for ProxyDisconnectTrigger::ProxyDisconnectTrigger(void):Trigger(eProxyDisconnectTrigger)
TEST_F(ProxyDisconnectTriggerUnitMock, ProxyDisconnectTrigger)
{
    //no source, not need to test
}

//test for E_Bool ProxyDisconnectTrigger::CheckCondition()
TEST_F(ProxyDisconnectTriggerUnitMock, ProxyDisconnectTrigger_0)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_EQ(MyProxyDisconnectTrigger->CheckCondition(), eTrue);
}
TEST_F(ProxyDisconnectTriggerUnitMock, ProxyDisconnectTrigger_1)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_EQ(MyProxyDisconnectTrigger->CheckCondition(), eTrue);
}
TEST_F(ProxyDisconnectTriggerUnitMock, ProxyDisconnectTrigger_2)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_EQ(MyProxyDisconnectTrigger->CheckCondition(), eFalse);
}

//test for void ProxyDisconnectTrigger::ProcessTrigger(Mode* mode)
TEST_F(ProxyDisconnectTriggerUnitMock, ProcessTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eProxyDisconnectTrigger)).Times(1);
    EXPECT_CALL(*_ModeMgr, modeProcessTriggerID(eProxyDisconnectTrigger)).Times(1);

    MyProxyDisconnectTrigger->ProcessTrigger((Mode*)0);
}
#endif //end define BD_TRIGGER_UNIT_TEST
