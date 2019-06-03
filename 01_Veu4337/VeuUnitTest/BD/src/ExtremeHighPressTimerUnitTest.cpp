#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ExtremeHighPressTimer.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"

ExtremeHighPressTimer::ExtremeHighPressTimer(int  initialTime, bool enabled)
    :Timer("ExtremeHighPressTimer", initialTime, enabled)
{
    isExpired = false;
    isProcessAutoReset = false;
}

void ExtremeHighPressTimer::ExpirationRoutine()
{
    this->Stop();
    isExpired = true;
    if (isProcessAutoReset)
    {
        AlarmResetOpReqTriggerSetIsIgnoreAlarmReset(eTrue);
//        BdTask::S_GetInstance()->Send(eAlarmResetKey);
    }
}
void ExtremeHighPressTimer::initialize()
{
    this->Stop();
    isExpired = false;
}
void ExtremeHighPressTimer::setTimerExpired(bool state)
{
    isExpired = state;
}
bool ExtremeHighPressTimer::getTimerExpired(void)
{
    return isExpired;
}

class ExtremeHighPressTimerUnitMock : public BDTestFixture
{
public:
    ExtremeHighPressTimerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ExtremeHighPressTimerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyExtremeHighPressTimer = new ExtremeHighPressTimer(10, true);
    }
    virtual void TearDown(void)
    {
        delete MyExtremeHighPressTimer;
    }
    ExtremeHighPressTimer *MyExtremeHighPressTimer;
};

//test for ExtremeHighPressTimer::ExtremeHighPressTimer(int  initialTime, bool enabled)
TEST_F(ExtremeHighPressTimerUnitMock, ExtremeHighPressTimer)
{
    EXPECT_EQ(MyExtremeHighPressTimer->isExpired, false);
    EXPECT_EQ(MyExtremeHighPressTimer->isProcessAutoReset, false);
}

//test for void ExtremeHighPressTimer::ExpirationRoutine()
TEST_F(ExtremeHighPressTimerUnitMock, ExpirationRoutine)
{
    MyExtremeHighPressTimer->isProcessAutoReset = true;
    EXPECT_CALL(*_Alarm,  AlarmResetOpReqTriggerSetIsIgnoreAlarmReset(eTrue)).Times(1);

    MyExtremeHighPressTimer->ExpirationRoutine();
}

//test for void ExtremeHighPressTimer::initialize()
TEST_F(ExtremeHighPressTimerUnitMock, initialize)
{
    MyExtremeHighPressTimer->initialize();

    EXPECT_EQ(MyExtremeHighPressTimer->isExpired, false);
}

//test for void ExtremeHighPressTimer::setTimerExpired(bool state)
TEST_F(ExtremeHighPressTimerUnitMock, setTimerExpired)
{
    MyExtremeHighPressTimer->setTimerExpired(true);

    EXPECT_EQ(MyExtremeHighPressTimer->isExpired, true);
}

//test for bool ExtremeHighPressTimer::getTimerExpired(void)
TEST_F(ExtremeHighPressTimerUnitMock, getTimerExpired)
{
    MyExtremeHighPressTimer->isExpired = false;

    EXPECT_EQ(MyExtremeHighPressTimer->getTimerExpired(), false);
}

#endif //end define BD_TRIGGER_UNIT_TEST
