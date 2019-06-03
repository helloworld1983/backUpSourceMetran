#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HardwareErrorAutoresetTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#define TWO_SECOND 2000
HardwareErrorAutoresetTrigger* HardwareErrorAutoresetTrigger::S_Instance = NULL;
HardwareErrorAutoresetTrigger* HardwareErrorAutoresetTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HardwareErrorAutoresetTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end HardwareErrorAutoresetTrigger()
HardwareErrorAutoresetTrigger::HardwareErrorAutoresetTrigger(void)
    : Trigger(eHardwareErrorAutoresetTrigger)
{
    TimeCounter = 0;
    IsArmed = eFalse;
    two_second = 0;
}   // end HardwareErrorAutoresetTrigger()
E_Bool HardwareErrorAutoresetTrigger::CheckCondition(void)
{

//    TriggerDidFire =(E_Bool)(!(HardwareChecking::S_GetInstance()->checkHFODoor()
//                               | HardwareChecking::S_GetInstance()->CheckHFOPiston()
//
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(O2Valve,eO2MotorError,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(AirValve,eAirMotorError,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(ExhValve,eExhMotorError,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(HfoValve,eHFOMotorError,eNoLogEvent)
//
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(O2Valve,eO2MotorLostCommunication,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(AirValve,eAirMotorLostCommunication,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(ExhValve,eExhMotorLostCommunication,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(HfoValve,eHFOMotorLostCommunication,eNoLogEvent)
//
//                               | ValveStuck::S_GetInstance()->CheckO2ValveStuckOpen()
//                               | ValveStuck::S_GetInstance()->CheckAirValveStuckOpen()
//                               | ValveStuck::S_GetInstance()->CheckO2ValveStuckClosed()
//                               | ValveStuck::S_GetInstance()->CheckAirValveStuckClosed()
//                               | HardwareChecking::S_GetInstance()->CheckFlowSensor()
//                               ));
    two_second = two_second + 2;
    if(two_second >= TWO_SECOND)
    {
        two_second = 0;
//        TriggerDidFire =(E_Bool)(TriggerDidFire & (!HardwareChecking::S_GetInstance()->CheckDACLoopback()));
    }

    return(TriggerDidFire);

}   // end CheckCondition()
void HardwareErrorAutoresetTrigger::EnableTrigger(void)
{
    IsArmed = eFalse;
    TimeCounter = 0;
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()

class HardwareErrorAutoresetTriggerUnitMock : public BDTestFixture
{
public:
    HardwareErrorAutoresetTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~HardwareErrorAutoresetTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHardwareErrorAutoresetTrigger = new HardwareErrorAutoresetTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyHardwareErrorAutoresetTrigger;
    }
    HardwareErrorAutoresetTrigger *MyHardwareErrorAutoresetTrigger;
};

//test for HardwareErrorAutoresetTrigger* HardwareErrorAutoresetTrigger::S_GetInstance(void)
TEST_F(HardwareErrorAutoresetTriggerUnitMock, S_GetInstance)
{
   MyHardwareErrorAutoresetTrigger->S_GetInstance();

   EXPECT_NE(MyHardwareErrorAutoresetTrigger->S_Instance, (void*)0);
}

//test for HardwareErrorAutoresetTrigger::HardwareErrorAutoresetTrigger(void)
TEST_F(HardwareErrorAutoresetTriggerUnitMock, HardwareErrorAutoresetTrigger)
{
    EXPECT_EQ(MyHardwareErrorAutoresetTrigger->TimeCounter, 0);
    EXPECT_EQ(MyHardwareErrorAutoresetTrigger->two_second, 0);
    EXPECT_EQ(MyHardwareErrorAutoresetTrigger->IsArmed, eFalse);
}

//test for E_Bool HardwareErrorAutoresetTrigger::CheckCondition(void)
TEST_F(HardwareErrorAutoresetTriggerUnitMock, CheckCondition)
{
    MyHardwareErrorAutoresetTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyHardwareErrorAutoresetTrigger->CheckCondition(), eTrue);
}

//test for void HardwareErrorAutoresetTrigger::EnableTrigger(void)
TEST_F(HardwareErrorAutoresetTriggerUnitMock, EnableTrigger)
{
    MyHardwareErrorAutoresetTrigger->EnableTrigger();

    EXPECT_EQ(MyHardwareErrorAutoresetTrigger->IsArmed, eFalse);
    EXPECT_EQ(MyHardwareErrorAutoresetTrigger->TimeCounter, 0);
}

#endif //end define BD_TRIGGER_UNIT_TEST
