#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HardwareErrorTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#define TWO_SECOND 2000

HardwareErrorTrigger* HardwareErrorTrigger::S_Instance = NULL;

HardwareErrorTrigger* HardwareErrorTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HardwareErrorTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
HardwareErrorTrigger::HardwareErrorTrigger()
    : Trigger(eHardwareErrorAlarmTriger)
{
    two_second = 0;
}

E_Bool HardwareErrorTrigger::CheckCondition(void)
{

//    TriggerDidFire =(E_Bool)(HardwareChecking::S_GetInstance()->checkHFODoor()
//                             | HardwareChecking::S_GetInstance()->CheckHFOPiston()
//                             | HardwareChecking::S_GetInstance()->CheckFan(eFanError)
//
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(AirValve,eAirMotorError,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(O2Valve,eO2MotorError,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(ExhValve,eExhMotorError,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(HfoValve,eHFOMotorError,eEventLog)
//
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(AirValve,eAirMotorLostCommunication,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(O2Valve,eO2MotorLostCommunication,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(ExhValve,eExhMotorLostCommunication,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(HfoValve,eHFOMotorLostCommunication,eEventLog)
//
//                             | ValveStuck::S_GetInstance()->CheckO2ValveStuckOpen()
//                             | ValveStuck::S_GetInstance()->CheckAirValveStuckOpen()
//                             | ValveStuck::S_GetInstance()->CheckO2ValveStuckClosed()
//                             | ValveStuck::S_GetInstance()->CheckAirValveStuckClosed()
//
//                             | HardwareChecking::S_GetInstance()->CheckFlowSensor()
//
//
//                             );
    two_second = two_second + 2;
    if(two_second >= TWO_SECOND)
    {
        two_second = 0;
//        TriggerDidFire =(E_Bool)(TriggerDidFire | HardwareChecking::S_GetInstance()->CheckDACLoopback());
    }

    return TriggerDidFire;

}

class HardwareErrorTriggerUnitMock : public BDTestFixture
{
public:
    HardwareErrorTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~HardwareErrorTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHardwareErrorTrigger = new HardwareErrorTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyHardwareErrorTrigger;
    }
    HardwareErrorTrigger *MyHardwareErrorTrigger;
};

//test for HardwareErrorTrigger* HardwareErrorTrigger::S_GetInstance(void)
TEST_F(HardwareErrorTriggerUnitMock, S_GetInstance)
{
    MyHardwareErrorTrigger->S_GetInstance();

    EXPECT_NE(MyHardwareErrorTrigger->S_Instance, (void*)0);
}

//test for HardwareErrorTrigger::HardwareErrorTrigger()
TEST_F(HardwareErrorTriggerUnitMock, HardwareErrorTrigger)
{
    EXPECT_EQ(MyHardwareErrorTrigger->two_second, 0);
}

//test for E_Bool HardwareErrorTrigger::CheckCondition(void)
TEST_F(HardwareErrorTriggerUnitMock, CheckCondition)
{
    MyHardwareErrorTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyHardwareErrorTrigger->CheckCondition(), eTrue);
}

#endif //end define BD_TRIGGER_UNIT_TEST
