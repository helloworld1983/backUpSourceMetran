#include "EnableTestCase.h"
#ifdef ALARM_UNIT_TEST

#include "AlarmStat.h"
#include "gtest.h"

using namespace ::testing;

class ElevationTimerUnitTest : public ::testing::Test
{
public:
    ElevationTimerUnitTest(void)
    {
    }
    virtual ~ElevationTimerUnitTest(void){}
    virtual void SetUp(void)
    {
        TestElevationTimer = new ElevationTimer(0);
    }
    virtual void TearDown(void)
    {
        delete TestElevationTimer;
    }
    ElevationTimer *TestElevationTimer;
};

//test for ElevationTimer::ElevationTimer (UNSIGNED tim)
ElevationTimer::ElevationTimer (UNSIGNED tim)
{
    ElevationFlag = eFalse;
}

TEST_F(ElevationTimerUnitTest, ElevationTimer_ElevationTimer)
{
    EXPECT_EQ(eFalse, TestElevationTimer->ElevationFlag);
}

//Test for void ElevationTimer::ExpirationRoutine (void)
void ElevationTimer::ExpirationRoutine (void)
{
    ElevationFlag = eTrue;
}

TEST_F(ElevationTimerUnitTest, ElevationTimer_ExpirationRoutine)
{
    TestElevationTimer->ExpirationRoutine();

    EXPECT_EQ(eTrue, TestElevationTimer->ElevationFlag);
}

class AlarmStatUnitTest : public ::testing::Test
{
public:
    AlarmStatUnitTest(void)
    {
    }
    virtual ~AlarmStatUnitTest(void){}
    virtual void SetUp(void)
    {
        TestAlarmStatNULL = new AlarmStat(eNoEventID, eInactive, eInactive, 0, 0);
        TestAlarmStatNotNULL = new AlarmStat(eNoEventID, eInactive, eInactive, 1, 1);
    }
    virtual void TearDown(void)
    {
        delete TestAlarmStatNULL;
        delete TestAlarmStatNotNULL;
    }
    AlarmStat *TestAlarmStatNULL;
    AlarmStat *TestAlarmStatNotNULL;
};

//Test for AlarmStat Constructor
AlarmStat::AlarmStat (E_GUIEventID id, E_AlarmLevel initialPriority,
                      E_AlarmLevel finalPriority, UNSIGNED tim, UNSIGNED occurent)
{
    const int TIME_IS_INVALID = 0;

    Id = id;
    Level = eInactive;
    InitialPriority = initialPriority;
    FinalPriority = finalPriority;
    IsActive = eNotActive;
    OldIsActive = eNotActive;
    Processed = eTrue;

    if(TIME_IS_INVALID != tim)
    {
        //Create a timer to evaluate
        AlarmTimer = new ElevationTimer (tim);
        //Check available creation
//        ASSERTION(NULL != AlarmTimer);
        //Set default for Elevation flag
//        AlarmTimer->ElevationFlag = eFalse;
    }
    else
    {
        AlarmTimer = NULL;
    }

    PreLevel = eInactive;
}

TEST_F(AlarmStatUnitTest, AlarmStat_AlarmStat_NULL)
{
    EXPECT_EQ(eNoEventID,TestAlarmStatNULL->Id);
    EXPECT_EQ(eInactive,TestAlarmStatNULL->Level);
    EXPECT_EQ(eInactive,TestAlarmStatNULL->InitialPriority);
    EXPECT_EQ(eInactive,TestAlarmStatNULL->FinalPriority);
    EXPECT_EQ(eNotActive,TestAlarmStatNULL->IsActive);
    EXPECT_EQ(eNotActive,TestAlarmStatNULL->OldIsActive);
    EXPECT_EQ(eTrue,TestAlarmStatNULL->Processed);
    EXPECT_EQ(eInactive,TestAlarmStatNULL->PreLevel);

    EXPECT_EQ((void*)0,TestAlarmStatNULL->AlarmTimer);
}

TEST_F(AlarmStatUnitTest, AlarmStat_AlarmStat_NotNULL)
{
    EXPECT_EQ(eNoEventID,TestAlarmStatNotNULL->Id);
    EXPECT_EQ(eInactive,TestAlarmStatNotNULL->Level);
    EXPECT_EQ(eInactive,TestAlarmStatNotNULL->InitialPriority);
    EXPECT_EQ(eInactive,TestAlarmStatNotNULL->FinalPriority);
    EXPECT_EQ(eNotActive,TestAlarmStatNotNULL->IsActive);
    EXPECT_EQ(eNotActive,TestAlarmStatNotNULL->OldIsActive);
    EXPECT_EQ(eTrue,TestAlarmStatNotNULL->Processed);
    EXPECT_EQ(eInactive,TestAlarmStatNotNULL->PreLevel);

    EXPECT_NE((void*)0,TestAlarmStatNotNULL->AlarmTimer);
}

//Test for E_ReturnStatus AlarmStat::SetIsActive (E_AlarmStatus stat)
E_ReturnStatus AlarmStat::SetIsActive (E_AlarmStatus stat)
{
    E_ReturnStatus retval;

    if(Processed == eTrue)
    {
        IsActive = stat;
        Processed = eFalse;
        retval = eSuccess;
    }
    else
    {
        retval = eError;
    }

    return (retval);

}

struct AlarmStatSetIsActive
{
    //input of test case
    E_AlarmStatus stat;
    E_Bool bProcessed;
    E_AlarmStatus PreIsActive;

    //output of test case
    E_ReturnStatus eEetval;
    E_Bool CurrentProcess;
    E_AlarmStatus CurrentIsActive;
};
struct AlarmStatSetIsActive AlarmStatSetIsActiveTestCaseList[] =
{
        {eNotActive, eFalse, eNotActive, eError, eFalse, eNotActive},
        {eNotActive, eFalse, eActive, eError, eFalse, eActive},

        {eActive, eFalse, eNotActive, eError, eFalse, eNotActive},
        {eActive, eFalse, eActive, eError, eFalse, eActive},

        {eNotActive, eTrue, eNotActive, eSuccess, eFalse, eNotActive},
        {eNotActive, eTrue, eActive, eSuccess, eFalse, eNotActive},
        {eActive, eTrue, eNotActive, eSuccess, eFalse, eActive},
        {eActive, eTrue, eActive, eSuccess, eFalse, eActive},
};

class TestAlarmStatSetIsActive : public ::testing::TestWithParam<AlarmStatSetIsActive>
{
public:
    TestAlarmStatSetIsActive(void)
    {
    }
    virtual ~TestAlarmStatSetIsActive(void){}
    virtual void SetUp(void)
    {
        TestAlarmStat = new AlarmStat(eNoEventID, eInactive, eInactive, 1, 1);
    }
    virtual void TearDown(void)
    {
        delete TestAlarmStat;
    }
    AlarmStat *TestAlarmStat;
};

TEST_P(TestAlarmStatSetIsActive, AlarmStatSetIsActiveTestCaseList)
{
    //input of test case
    E_AlarmStatus stat  = GetParam().stat;
    E_Bool bProcessed = GetParam().bProcessed;
//    E_AlarmStatus PreIsActive = GetParam().PreIsActive;

    //output of test case
    E_ReturnStatus eEetval = GetParam().eEetval;
    E_Bool CurrentProcess = GetParam().CurrentProcess;
//    E_AlarmStatus CurrentIsActive = GetParam().CurrentIsActive;

    TestAlarmStat->Processed = bProcessed;

    EXPECT_EQ(eEetval , TestAlarmStat->SetIsActive(stat));
    EXPECT_EQ(CurrentProcess , TestAlarmStat->Processed);

}
INSTANTIATE_TEST_CASE_P(TestAlarmStatSetIsActive,
                        TestAlarmStatSetIsActive,
                        testing::ValuesIn(AlarmStatSetIsActiveTestCaseList)
);

//Test for E_AlarmStatus AlarmStat::GetIsActive(void)
E_AlarmStatus AlarmStat::GetIsActive(void)
{
    return IsActive;
}

TEST_F(AlarmStatUnitTest, AlarmStat_GetIsActive)
{
    TestAlarmStatNotNULL->IsActive = eActive;
    EXPECT_EQ(eActive, TestAlarmStatNotNULL->GetIsActive());
}
TEST_F(AlarmStatUnitTest, AlarmStat_GetIsNotActive)
{
    TestAlarmStatNotNULL->IsActive = eNotActive;
    EXPECT_EQ(eNotActive, TestAlarmStatNotNULL->GetIsActive());
}

//Test for E_AlarmLevel AlarmStat::GetLevel (void)
E_AlarmLevel AlarmStat::GetLevel (void)
{
    return Level;
}
TEST_F(AlarmStatUnitTest, AlarmStat_GetLevel)
{
    TestAlarmStatNotNULL->Level = eHigh;
    EXPECT_EQ(eHigh, TestAlarmStatNotNULL->Level);

    TestAlarmStatNotNULL->Level = eMedium;
    EXPECT_EQ(eMedium, TestAlarmStatNotNULL->Level);

    TestAlarmStatNotNULL->Level = eLow;
    EXPECT_EQ(eLow, TestAlarmStatNotNULL->Level);

    TestAlarmStatNotNULL->Level = eAutoReset;
    EXPECT_EQ(eAutoReset, TestAlarmStatNotNULL->Level);

    TestAlarmStatNotNULL->Level = eInactive;
    EXPECT_EQ(eInactive, TestAlarmStatNotNULL->Level);

    TestAlarmStatNotNULL->Level = eLastAlarmLevel;
    EXPECT_EQ(eLastAlarmLevel, TestAlarmStatNotNULL->Level);

}

#endif //end define ALARM_UNIT_TEST
