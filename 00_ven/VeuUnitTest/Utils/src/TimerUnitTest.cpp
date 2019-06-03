#include "EnableTestCase.h"
#ifdef UTIL_UNIT_TEST

#include "Timer.h"
#include "gtest.h"
#include "gmock.h"
#include "UtilFixture.h"


//$COMMON.ATTRIBUTE$
//    Name: handle
//    Description: Timer is used in Timer Class
//
//    Units:  none
//    Range:  n/a
TimerHandle_t handle;

using namespace ::testing;

class TimerUnitTest : public UtilTestFixture
{
public:
    TimerUnitTest(void) : UtilTestFixture(new BaseModuleMock)
{
}
    virtual ~TimerUnitTest(void){}
    virtual void SetUp(void){}
    virtual void TearDown(void){}
};

//Test for bool Timer::IsAcitve ()
bool IsAcitve (void)
{
    bool Status;
    //Check the timer is active or not
    if(pdFALSE == xTimerIsTimerActive (handle))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

TEST_F(TimerUnitTest, TimerUnitTest_IsAcitve)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerIsTimerActive(handle)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, IsAcitve());
}

TEST_F(TimerUnitTest, TimerUnitTest_IsInAcitve)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerIsTimerActive(handle)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, IsAcitve());
}

//Test for bool Timer::Stop (TickType_t CmdTimeout)
bool Stop (TickType_t CmdTimeout)
{
    bool Status;
    //Stop timer
    if(pdFALSE == xTimerStop(handle, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

TEST_F(TimerUnitTest, TimerUnitTest_Stop_ZeroTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStop(handle, 0)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, Stop(0));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStop(handle, 0)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, Stop(0));

}

TEST_F(TimerUnitTest, TimerUnitTest_Stop_1SecTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStop(handle, 1)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, Stop(1));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStop(handle, 1)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, Stop(1));
}

//Test for Timer::Reset (TickType_t CmdTimeout)
bool Reset (TickType_t CmdTimeout)
{
    bool Status;
    //Stop timer
    if(pdFALSE == xTimerReset(handle, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

TEST_F(TimerUnitTest, TimerUnitTest_Reset_ZeroTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerReset(handle, 0)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, Reset(0));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerReset(handle, 0)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, Reset(0));

}

TEST_F(TimerUnitTest, TimerUnitTest_Reset_1SecTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerReset(handle, 1)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, Reset(1));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerReset(handle, 1)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, Reset(1));
}

//Test for bool Timer::Start (TickType_t CmdTimeout)
bool Start (TickType_t CmdTimeout)
{
    bool Status;
    //Start the timer
    if(pdFALSE == xTimerStart(handle, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

TEST_F(TimerUnitTest, TimerUnitTest_Start_ZeroTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStart(handle, 0)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, Start(0));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStart(handle, 0)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, Start(0));

}

TEST_F(TimerUnitTest, TimerUnitTest_Start_1SecTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStart(handle, 1)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, Start(1));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerStart(handle, 1)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, Start(1));
}

//test for bool Timer::SetPeriod (TickType_t NewPeriod, TickType_t CmdTimeout)
bool SetPeriod (TickType_t NewPeriod, TickType_t CmdTimeout)
{
    bool Status;
    //Set new period for timer
    if(pdFALSE == xTimerChangePeriod(handle, NewPeriod, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

TEST_F(TimerUnitTest, TimerUnitTest_SetPeriod_ZeroTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerChangePeriod(handle, 0, 0)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, SetPeriod(0, 0));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerChangePeriod(handle, 0, 0)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, SetPeriod(0, 0));

}

TEST_F(TimerUnitTest, TimerUnitTest_SetPeriod_1SecTimeOut)
{
    EXPECT_CALL(*_FunctionUsedInTimer, xTimerChangePeriod(handle, 1, 1)).Times(1).WillOnce(Return(pdFALSE));

    EXPECT_EQ(false, SetPeriod(1, 1));

    EXPECT_CALL(*_FunctionUsedInTimer, xTimerChangePeriod(handle, 1, 1)).Times(1).WillOnce(Return(pdTRUE));

    EXPECT_EQ(true, SetPeriod(1, 1));
}

#endif  //end define UTIL_UNIT_TEST
