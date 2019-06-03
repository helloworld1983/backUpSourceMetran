#include "gtest.h"
#include "gmock.h"
#include "DebugTrace.h"
#include "Standard.h"
#include "EnableTestCase.h"

#ifdef UTIL_UNIT_TEST

class UtilUnitTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
    };

    virtual void TearDown()
    {
    };
};

//Test for unsigned long CalcElapsedTime (unsigned long startTime)
unsigned long CalcElapsedTime (unsigned long startTime, UNSIGNED FakeCurrentTime, bool Status)
{
    const UNSIGNED MAX_UINT = ~0L;
    UNSIGNED currentTime = FakeCurrentTime;
    UNSIGNED elapsedTime;

    if(Status)
    {
        if (currentTime >= startTime)
        {
            elapsedTime = currentTime - startTime;
        }

        // Deal with rollover
        else
        {
            elapsedTime = (MAX_UINT - startTime + currentTime);
        }
        DEBUG_UTIL("Current time is %Ds \n", elapsedTime);
    }
    else
    {
        DEBUG_UTIL("Can Not Get Current Time From RTC\n");
        elapsedTime = 0;
    }

    return(elapsedTime);
}

TEST_F(UtilUnitTest, CalcElapsedTime)
{
    EXPECT_EQ(0, CalcElapsedTime(1, 1, false));

    EXPECT_EQ(4294967293, CalcElapsedTime(10, 8, true));
    EXPECT_EQ(0, CalcElapsedTime(10, 10, true));
    EXPECT_EQ(3, CalcElapsedTime(7, 10, true));

}

//Test for E_Bool InDiagnosticMode()
E_Bool InDiagnosticMode()
{
    return(eFalse);
}

TEST_F(UtilUnitTest, InDiagnosticMode)
{
    EXPECT_EQ(eFalse, InDiagnosticMode());
}

#endif  //end define UTIL_UNIT_TEST
