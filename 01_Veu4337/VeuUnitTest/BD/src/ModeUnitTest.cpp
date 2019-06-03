#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

E_TriggerId Mode::S_LastTriggerId = eNoTrigger;

Mode::Mode (E_Bool doesManualReset) :
                        DoesManualReset (doesManualReset)
{

}   // end Mode()

inline Mode::Mode (const Mode&)
{

}

inline void Mode::S_SetLastTriggerId (E_TriggerId id)
{
    S_LastTriggerId = id;

}   // end S_SetLastTriggerId()

E_Bool Mode::S_IsInhPtTriggered (void)
{
    E_Bool rtnValue = eFalse;

    // If the inhalation was triggered by the patient, set the return value to
    // TRUE.
    if((ePtInhPressTrigger == S_LastTriggerId)
            || (ePtInhFlowTrigger == S_LastTriggerId))
    {
        rtnValue = eTrue;
    }

    return (rtnValue);

}   // end S_IsInhPtTriggered()

void Mode::CreateSafeState()
{
    O2ValveSetStepPosition(0);
    AirValveSetStepPosition(0);
    ExhValveSetStepPosition(0);
    HfoValveSetStepPosition(0);
}   //end CreateSafeState()

void Mode::ProcessTriggerID (E_TriggerId id)
{
    // This method should not be invoked; derived classes that enable this
    // trigger and expect it to fire should provide the process method!
    ASSERTION(eFalse);
}   // end ProcessTrigger()


void Mode::Enter (E_VentilationMode fromModeId)
{
}

void Mode::Exit (void)
{
}

void Mode::UpdateAlarmStatus (void)
{
}

class ModeUnitMock : public BDTestFixture
{
public:
    ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyMode = new Mode(eTrue);
    }
    virtual void TearDown(void)
    {
        delete MyMode;
    }
    Mode *MyMode;
};

//test for Mode::Mode (E_Bool doesManualReset)
TEST_F(ModeUnitMock, Mode)
{
    EXPECT_EQ(MyMode->DoesManualReset, eTrue);
}

//test for inline Mode::Mode (const Mode&)
TEST_F(ModeUnitMock, ModeCopyConstructor)
{
    //not test
}

//test for E_Bool Mode::S_IsInhPtTriggered (void)
TEST_F(ModeUnitMock, S_IsInhPtTriggered)
{
    MyMode->S_LastTriggerId = ePtExhTrigger;

    EXPECT_EQ(MyMode->S_LastTriggerId, ePtExhTrigger);
}

//test for E_Bool Mode::S_IsInhPtTriggered (void)
TEST_F(ModeUnitMock, S_IsInhPtTriggered_0)
{
    MyMode->S_LastTriggerId  = ePtInhPressTrigger;

    MyMode->S_IsInhPtTriggered();

    EXPECT_EQ(MyMode->S_IsInhPtTriggered(), eTrue);
}
TEST_F(ModeUnitMock, S_IsInhPtTriggered_1)
{
    MyMode->S_LastTriggerId  = ePtInhFlowTrigger;

    MyMode->S_IsInhPtTriggered();

    EXPECT_EQ(MyMode->S_IsInhPtTriggered(), eTrue);
}
TEST_F(ModeUnitMock, S_IsInhPtTriggered_2)
{
    MyMode->S_LastTriggerId  = eAPRVLowPressTimeTrigger;

    MyMode->S_IsInhPtTriggered();

    EXPECT_EQ(MyMode->S_IsInhPtTriggered(), eFalse);
}

//test for void Mode::CreateSafeState()
TEST_F(ModeUnitMock, CreateSafeState)
{
    EXPECT_CALL(*_StepMotor, O2ValveSetStepPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveSetStepPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveSetStepPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, HfoValveSetStepPosition(0)).Times(1);

    MyMode->CreateSafeState();
}

//test for void Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(ModeUnitMock, ProcessTriggerID)
{
    //pure virtual function in class > not test
}
//test for void Mode::Enter (E_VentilationMode fromModeId)
TEST_F(ModeUnitMock, Enter)
{
    //pure virtual function in class > not test
}
//test for void Mode::Exit (void)
TEST_F(ModeUnitMock, Exit)
{
    //pure virtual function in class > not test
}
//test for void Mode::UpdateAlarmStatus (void)
TEST_F(ModeUnitMock, UpdateAlarmStatus)
{
    //pure virtual function in class > not test
}

#endif      //end define BD_MODE_UNIT_TEST
