#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ProxyConnectTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "StepMotorMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "ModeMgrMockHeader.h"

ProxyConnectTrigger::ProxyConnectTrigger(void):Trigger(eProxyConnectTrigger)
{
}

E_Bool ProxyConnectTrigger::CheckCondition()
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
void ProxyConnectTrigger::ProcessTrigger(Mode* mode)
{
    DisableTrigger();
    TriggerMgrEnableTrigger(eProxyDisconnectTrigger);
    modeProcessTriggerID(eProxyConnectTrigger);
}

class ProxyConnectTriggerUnitMock : public BDTestFixture
{
public:
    ProxyConnectTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ProxyConnectTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyProxyConnectTrigger = new ProxyConnectTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyProxyConnectTrigger;
    }
    ProxyConnectTrigger *MyProxyConnectTrigger;
};

//test for ProxyConnectTrigger::ProxyConnectTrigger(void):Trigger(eProxyConnectTrigger)
TEST_F(ProxyConnectTriggerUnitMock, ProxyConnectTrigger)
{
    //no source, not need to test
}

//test for E_Bool ProxyConnectTrigger::CheckCondition()
TEST_F(ProxyConnectTriggerUnitMock, ProxyConnectTrigger_0)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_EQ(MyProxyConnectTrigger->CheckCondition(), eTrue);
}
TEST_F(ProxyConnectTriggerUnitMock, ProxyConnectTrigger_1)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_EQ(MyProxyConnectTrigger->CheckCondition(), eTrue);
}
TEST_F(ProxyConnectTriggerUnitMock, ProxyConnectTrigger_2)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_EQ(MyProxyConnectTrigger->CheckCondition(), eFalse);
}

//test for void ProxyConnectTrigger::ProcessTrigger(Mode* mode)
TEST_F(ProxyConnectTriggerUnitMock, ProcessTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eProxyDisconnectTrigger)).Times(1);
    EXPECT_CALL(*_ModeMgr, modeProcessTriggerID(eProxyConnectTrigger)).Times(1);

    MyProxyConnectTrigger->ProcessTrigger((Mode*)0);
}

#endif //end define BD_TRIGGER_UNIT_TEST
