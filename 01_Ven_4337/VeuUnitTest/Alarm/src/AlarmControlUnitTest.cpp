#include "EnableTestCase.h"
#ifdef ALARM_UNIT_TEST

#include "AlarmControl.h"
#include "gtest.h"
#include "gmock.h"
#include "DebugTrace.h"
#include "AlarmFakeService.h"
#include "BaseModuleMock.h"
#include "AlarmFixture.h"

const LONG SILENCETIME = (60 * 2);      // 2 mins
#define TIMETOGETSEM (( TickType_t ) 10)
static const int NONEWALARMS = -1;

AlarmControl * AlarmControl::S_Instance = NULL;

AlarmControl* AlarmControl::S_GetInstance (void)
{
    //Create object if it is null
    if(S_Instance == NULL)
    {
        S_Instance = new AlarmControl ();
    }

    //Check opinter is null or not
    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

inline E_Bool AlarmControl::GetResetFlag (void)
{
    return (ResetFlag);
}

inline void AlarmControl::SetResetFlag (E_Bool val)
{
    //Suspend all system
//    vTaskSuspendAll ();
    //Set Flag
    ResetFlag = val;
    //Resume all system
//    xTaskResumeAll ();
}

inline E_Bool AlarmControl::GetSilenceFlag (void)
{
    return (SilenceFlag);
}

inline void AlarmControl::SetSilenceFlag (E_Bool val)
{
    //Suspend all system
//    vTaskSuspendAll ();
    //Set flag
    SilenceFlag = val;
    //Resume all system
//    xTaskResumeAll ();
}

inline E_Bool AlarmControl::GetSilenceButtonStatus (void)
{
    return SilenceOn;
}

inline void AlarmControl::SetSilenceButtonStatus (E_Bool val)
{
    SilenceOn = val;
}

AlarmControl::AlarmControl (void)
                : Timer("AlarmControlTimer", SILENCETIME*TICKS_PER_SEC, pdTRUE)
{
    ResetFlag = eFalse;
    SilenceFlag = eFalse;
    SilenceOn = eFalse;

    //initialize for audio
    //  sndPtr = Audible::S_GetInstance();

}

void AlarmControl::ExpirationRoutine (void)
{
//    MgrAlarmStat * ptr = NULL;

    CancelSilence ();

//    ptr = MgrAlarmStat::S_GetInstance ();

    Enunciate (NONEWALARMS);

}

void AlarmControl::CancelSilence (void)
{

//  STATUS stat;
//
//  stat = Disable();
//  ASSERT(stat == NU_SUCCESS);
//
//  AlarmSilenceLED->SetState(eOff);

    SilenceOn = eFalse;
}

E_ReturnStatus AlarmControl::Send (E_DeviceId id)
{

    switch (id)
    {
        case eAlarmResetKey:
            // no audible sound made here
            // provided by Bd
            SetResetFlag (eTrue);
            break;

        case eAlarmSilenceKey:
//          sndPtr->SetFeedback(eGoodBeep);
            SetSilenceFlag (eTrue);
            break;

        default:
            // should never get any other keys!
            ASSERTION(eFalse);
            break;
    };

    return (eSuccess);

}   // end Send()

class AlarmControlUnitTest : public ::testing::Test
{
public:
    AlarmControlUnitTest(void)
    {
    }
    virtual ~AlarmControlUnitTest(void){}
    virtual void SetUp(void)
    {
        TestAlarmControl = new AlarmControl();
    }
    virtual void TearDown(void)
    {
        delete TestAlarmControl;
    }
    AlarmControl *TestAlarmControl;
};

//Test for AlarmControl* AlarmControl::S_GetInstance (void)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_S_GetInstance)
{
    TestAlarmControl->S_Instance = (AlarmControl*)0;
    EXPECT_NE((AlarmControl*)0, TestAlarmControl->S_GetInstance());

    EXPECT_NE((AlarmControl*)0, TestAlarmControl->S_GetInstance());

}

//Test for inline E_Bool AlarmControl::GetResetFlag (void)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_GetResetFlag)
{
    TestAlarmControl->ResetFlag = eFalse;
    EXPECT_EQ(eFalse, TestAlarmControl->GetResetFlag());

    TestAlarmControl->ResetFlag = eTrue;
    EXPECT_EQ(eTrue, TestAlarmControl->GetResetFlag());

}

//Test for inline void AlarmControl::SetResetFlag (E_Bool val)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_SetResetFlag)
{
    TestAlarmControl->ResetFlag = eFalse;
    TestAlarmControl->SetResetFlag(eTrue);
    EXPECT_EQ(eTrue, TestAlarmControl->ResetFlag);

    TestAlarmControl->ResetFlag = eTrue;
    TestAlarmControl->SetResetFlag(eFalse);
    EXPECT_EQ(eFalse, TestAlarmControl->ResetFlag);
}

//Test for inline E_Bool AlarmControl::GetSilenceFlag (void)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_GetSilenceFlag)
{
    TestAlarmControl->SilenceFlag = eTrue;
    EXPECT_EQ(eTrue, TestAlarmControl->GetSilenceFlag());

    TestAlarmControl->SilenceFlag = eFalse;
    EXPECT_EQ(eFalse, TestAlarmControl->GetSilenceFlag());

}

//test for inline void AlarmControl::SetSilenceFlag (E_Bool val)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_SetSilenceFlag)
{
    TestAlarmControl->SilenceFlag = eFalse;
    TestAlarmControl->SetSilenceFlag(eTrue);
    EXPECT_EQ(eTrue, TestAlarmControl->SilenceFlag);

    TestAlarmControl->ResetFlag = eTrue;
    TestAlarmControl->SetSilenceFlag(eFalse);
    EXPECT_EQ(eFalse, TestAlarmControl->SilenceFlag);
}

//Test for inline E_Bool AlarmControl::GetSilenceButtonStatus (void)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_GetSilenceButtonStatus)
{
    TestAlarmControl->SilenceOn = eTrue;
    EXPECT_EQ(eTrue, TestAlarmControl->GetSilenceButtonStatus());

    TestAlarmControl->SilenceOn = eFalse;
    EXPECT_EQ(eFalse, TestAlarmControl->GetSilenceButtonStatus());

}

//test for inline void AlarmControl::SetSilenceButtonStatus (E_Bool val)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_SetSilenceButtonStatus)
{
    TestAlarmControl->SilenceOn = eFalse;
    TestAlarmControl->SetSilenceButtonStatus(eTrue);
    EXPECT_EQ(eTrue, TestAlarmControl->SilenceOn);

    TestAlarmControl->SilenceOn = eTrue;
    TestAlarmControl->SetSilenceButtonStatus(eFalse);
    EXPECT_EQ(eFalse, TestAlarmControl->SilenceOn);
}

//test for AlarmControl::AlarmControl (void)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_AlarmControl)
{
    EXPECT_EQ(eFalse, TestAlarmControl->ResetFlag);
    EXPECT_EQ(eFalse, TestAlarmControl->SilenceFlag);
    EXPECT_EQ(eFalse, TestAlarmControl->SilenceOn);
}

//test for void AlarmControl::CancelSilence (void)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_CancelSilence)
{
    TestAlarmControl->SilenceOn = eTrue;
    TestAlarmControl->CancelSilence();

    EXPECT_EQ(eFalse, TestAlarmControl->SilenceOn);
}

//test for E_ReturnStatus AlarmControl::Send (E_DeviceId id)
TEST_F(AlarmControlUnitTest, AlarmControlUnitTest_Send)
{
    TestAlarmControl->Send(eAlarmResetKey);
    EXPECT_EQ(eTrue, TestAlarmControl->ResetFlag);

    TestAlarmControl->Send(eAlarmSilenceKey);
    EXPECT_EQ(eTrue, TestAlarmControl->SilenceFlag);
}

//test for void AlarmControl::ExpirationRoutine (void)
class AlarmControlUnitTestExpirationRoutine : public AlarmTestFixture
{
public:
    AlarmControlUnitTestExpirationRoutine(void) : AlarmTestFixture(new BaseModuleMock)
{
}
    virtual ~AlarmControlUnitTestExpirationRoutine(void){}
    virtual void SetUp(void)
    {
        TestAlarmControl = new AlarmControl();
    }
    virtual void TearDown(void)
    {
        delete TestAlarmControl;
    }
    AlarmControl *TestAlarmControl;
};

TEST_F(AlarmControlUnitTestExpirationRoutine, AlarmControlUnitTestExpirationRoutine)
{
    EXPECT_CALL(*_AlarmControlMocks, Enunciate(NONEWALARMS)).Times(1);
    TestAlarmControl->ExpirationRoutine();

    EXPECT_EQ(eFalse, TestAlarmControl->SilenceOn);
}

#endif //end define ALARM_UNIT_TEST
