#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtInhTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
PtInhTrigger* PtInhTrigger::S_Instance = NULL;
PtInhTrigger* PtInhTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new PtInhTrigger();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()
void PtInhTrigger::EnableTrigger(void)
{
    IsArmed = eFalse;
    Trigger::EnableTrigger();

}   // end EnableTrigger()
E_Bool PtInhTrigger::CheckCondition(void)
{

    if((eTrue == IsArmed) &&(ExhPhaseS_IsInUnrestrictedPhase() == eTrue))
    {
        TriggerDidFire = eTrue;
    }

    return(TriggerDidFire);

}   // end CheckCondition()
E_Bool PtInhTrigger::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if (eTrue == IsEnabled)
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation
        // time is used.
        if (ExhPhaseS_IsInUnrestrictedPhase() == eTrue)
        {

            // If the trigger is armed, the trigger is ready to fire.
            if (eTrue == IsArmed)
            {
                rtnValue = eTrue;
            }
        }
    }

    return(rtnValue);

}   // end PreCheckCondition()
PtInhTrigger::PtInhTrigger(void)
    : Trigger(ePtInhTrigger), IsArmed(eFalse)
{
}   // end PtInhTrigger()
void PtInhTrigger::ReceiveEvent(void)
{
    IsArmed = eTrue;
}
E_Bool PtInhTrigger::GetIsArmed(void)
{
    return IsArmed;
}

class PtInhTriggerUnitMock : public BDTestFixture
{
public:
    PtInhTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtInhTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtInhTrigger = new PtInhTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtInhTrigger;
    }
    PtInhTrigger *MyPtInhTrigger;
};

//test for PtInhTrigger* PtInhTrigger::S_GetInstance(void)
TEST_F(PtInhTriggerUnitMock, S_GetInstance)
{
    MyPtInhTrigger->S_GetInstance();

    EXPECT_NE(MyPtInhTrigger->S_Instance, (void*)0);
}

//test for void PtInhTrigger::EnableTrigger(void)
TEST_F(PtInhTriggerUnitMock, EnableTrigger)
{
    MyPtInhTrigger->EnableTrigger();

    EXPECT_EQ(MyPtInhTrigger->IsArmed, eFalse);
}

//test for E_Bool PtInhTrigger::CheckCondition(void)
TEST_F(PtInhTriggerUnitMock, CheckCondition)
{
    MyPtInhTrigger->IsArmed = eTrue;
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    MyPtInhTrigger->TriggerDidFire = eFalse;

    EXPECT_EQ(MyPtInhTrigger->CheckCondition(), eTrue);
}

//test for E_Bool PtInhTrigger::PreCheckCondition(void)
TEST_F(PtInhTriggerUnitMock, PreCheckCondition)
{
    MyPtInhTrigger->IsEnabled = eTrue;

    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));

    MyPtInhTrigger->IsArmed = eTrue;

    EXPECT_EQ(MyPtInhTrigger->CheckCondition(), eTrue);
}

//test for PtInhTrigger::PtInhTrigger(void)
TEST_F(PtInhTriggerUnitMock, PtInhTrigger)
{
    //no source, not need to test
}

//test for void PtInhTrigger::ReceiveEvent(void)
TEST_F(PtInhTriggerUnitMock, ReceiveEvent)
{
    MyPtInhTrigger->ReceiveEvent();

    EXPECT_EQ(MyPtInhTrigger->IsArmed, eTrue);
}

//test for E_Bool PtInhTrigger::GetIsArmed(void)
TEST_F(PtInhTriggerUnitMock, GetIsArmed)
{
    MyPtInhTrigger->IsArmed = eTrue;

    EXPECT_EQ(MyPtInhTrigger->GetIsArmed(), eTrue);
}

#endif //end define BD_TRIGGER_UNIT_TEST
