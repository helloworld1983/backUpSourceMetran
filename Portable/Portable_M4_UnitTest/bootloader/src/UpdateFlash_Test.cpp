/*
 * UpdateFlash_Test.cpp
 *
 *  Created on: Feb 7, 2018
 *      Author: WINDOWS
 */

#include "stdafx.h"
#include "UpdateFlash.c"


struct TestCaseList_PowerButtonPressed
{
	bool isPressed;
	float voltVal;
};

TestCaseList_PowerButtonPressed MyTestCaseList_PowerButtonPressed[4] =
{
        {true,0},
		{true,0.5},
		{false,0.6},
		{false,0.7},
};

class TestUpdate_isPowerButtonPressed : public ::testing::TestWithParam<TestCaseList_PowerButtonPressed>
{
 protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
    // Code here will be called immediately after each test
    // (right before the destructor).
  }
};


TEST_P(TestUpdate_isPowerButtonPressed, update_isPowerButtonPressed)
{
	bool isPressed = GetParam().isPressed;
	float voltVal = GetParam().voltVal;

    EXPECT_EQ(isPressed, update_isPowerButtonPressed(voltVal));
}

INSTANTIATE_TEST_CASE_P(update_isPowerButtonPressed,TestUpdate_isPowerButtonPressed, testing::ValuesIn(MyTestCaseList_PowerButtonPressed));



