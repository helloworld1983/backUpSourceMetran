/*
 * Led_Test.cpp
 *
 *  Created on: Feb 1, 2018
 *      Author: WINDOWS
 */

#include "stdafx.h"
#include "Led.c"


struct TestCaseList_SetBrightness
{
	int levelOut;
	E_LEVEL_BRIGHTNESSS levelIn;
};

TestCaseList_SetBrightness MyTestCaseList_SetBrightness[3] =
{
        {0,eLevelBrightnessLoww},
        {1,eLevelBrightnessMedd},
        {2,eLevelBrightnessHighh},
};


class TestLed_SetBrightness : public ::testing::TestWithParam<TestCaseList_SetBrightness>
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


TEST_P(TestLed_SetBrightness, led_SetBrightness)
{
	int levelOut = GetParam().levelOut;
	E_LEVEL_BRIGHTNESSS levelIn = GetParam().levelIn;
    EXPECT_EQ(levelOut, led_SetBrightness(levelIn));
}

INSTANTIATE_TEST_CASE_P(led_SetBrightness,TestLed_SetBrightness, testing::ValuesIn(MyTestCaseList_SetBrightness));



struct TestCaseList_DisplayWord
{
	uint8_t output;
	E_ID_WORDD word;
};

TestCaseList_DisplayWord MyTestCaseList_DisplayWord[14] =
{
        {0x7C,eUSB},
        {0x00,eA},
        {0x44,eSPI},
        {0x00,eOFF},
        {0x00,eA},
        {0x44,eSPI},
        {0x00,eU},
        {0x00,eUP},
        {0x79,eUPd},
		{0x6E,eCHE},
		{0x00,eC},
        {0x00,eCH},
		{0x00, eOnFlowLed},
        {0x00, eOffAllLed}
};

class TestLed_DisplayWord : public ::testing::TestWithParam<TestCaseList_DisplayWord>
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


TEST_P(TestLed_DisplayWord, led_DisplayWord)
{
	uint8_t output = GetParam().output;
	E_ID_WORDD word = GetParam().word;
    EXPECT_EQ(output, led_DisplayWord(word));
}

INSTANTIATE_TEST_CASE_P(led_DisplayWord,TestLed_DisplayWord, testing::ValuesIn(MyTestCaseList_DisplayWord));





