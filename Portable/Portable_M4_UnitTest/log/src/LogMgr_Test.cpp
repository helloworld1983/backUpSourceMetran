/*
 * UpdateFlash_Test.cpp
 *
 *  Created on: Feb 7, 2018
 *      Author: WINDOWS
 */

#include "stdafx.h"
#include "LogMgr.c"


POR_PRESS_LOG_T log1 = {1999};
POR_PRESS_LOG_T log3 = {2000};
POR_PRESS_LOG_T log4 = {2001};
POR_PRESS_LOG_T log5 = {2018};
POR_PRESS_LOG_T log6 = {2019};


struct TestCaseList_logMgr_CheckTimePressLogValidReturnYear
{
	uint16_t year;
	POR_PRESS_LOG_T* log;
};

static TestCaseList_logMgr_CheckTimePressLogValidReturnYear MyTestCaseList_CheckTimePressLogValidReturnYear[5] =
{
        {2000,&log1},
		{2000,&log3},
		{2001,&log4},
		{2018,&log5},
		{2019,&log6},
};

class TestUpdate_logMgr_CheckTimePressLogValid : public ::testing::TestWithParam<TestCaseList_logMgr_CheckTimePressLogValidReturnYear>
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


TEST_P(TestUpdate_logMgr_CheckTimePressLogValid, logMgr_CheckTimePressLogValidReturnYear)
{
	uint16_t year = GetParam().year;
	POR_PRESS_LOG_T *log = GetParam().log;

    EXPECT_EQ(year, logMgr_CheckTimePressLogValidReturnYear(log));
}

INSTANTIATE_TEST_CASE_P(logMgr_CheckTimePressLogValidReturnYear,
						TestUpdate_logMgr_CheckTimePressLogValid,
						testing::ValuesIn(MyTestCaseList_CheckTimePressLogValidReturnYear));















POR_LOG_T log7 = {0, 0, 0};
POR_LOG_T log8 = {0, 0, 1};
POR_LOG_T log9 = {0, 0, 2};
POR_LOG_T log11 = {0, 0, 31};
POR_LOG_T log12 = {0, 0, 32};


struct TestCaseList_logMgr_CheckTimeLogValidReturnDay
{
	uint16_t day;
	POR_LOG_T* log;
};

static TestCaseList_logMgr_CheckTimeLogValidReturnDay MyTestCaseList_CheckTimeLogValidReturnDay[5] =
{
        {1, &log7},
		{1, &log8},
		{2, &log9},
		{31, &log11},
		{1, &log12},
};

class TestUpdate_logMgr_CheckTimeLogValid : public ::testing::TestWithParam<TestCaseList_logMgr_CheckTimeLogValidReturnDay>
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


TEST_P(TestUpdate_logMgr_CheckTimeLogValid, logMgr_CheckTimeLogValidReturnDay)
{
	uint16_t day = GetParam().day;
	POR_LOG_T *log = GetParam().log;

    EXPECT_EQ(day, logMgr_CheckTimeLogValidReturnDay(log));
}

INSTANTIATE_TEST_CASE_P(logMgr_CheckTimeLogValidReturnDay,
						TestUpdate_logMgr_CheckTimeLogValid,
						testing::ValuesIn(MyTestCaseList_CheckTimeLogValidReturnDay));







static POR_PRESS_LOG_T log13 = {2018, 2, 8, 9, 11, 30, 1010};
static uint8_t logArray[LOG_PRESS_LENGTH];
static uint8_t dataExpect[10] = {0xAC, 2018 % 256, 2018/256, 2, 8, 9, 11, 30, 1010 % 256, 1010/256};

class TestLogMgr_ConvertPressureLogToBinaryArray : public ::testing::Test
{
public:
	TestLogMgr_ConvertPressureLogToBinaryArray()
    {

    }
};


TEST(TestLogMgr_ConvertPressureLogToBinaryArray, logMgr_ConvertPressureLogToBinaryArray)
{
	logMgr_ConvertPressureLogToBinaryArray(&log13, logArray);
	for (int i = 0 ; i < 10; i++)
    	EXPECT_EQ(dataExpect[i], logArray[i]);
}





static POR_LOG_T log14 = {2018, 2, 8, 9, 11, 30, 1010, 9 , 8, {1,2,3}, {4,5,6}};
static uint8_t logArray1[LOG_LENGTH];
static uint8_t dataExpect1[18] = {0xAB, 2018 % 256, 2018/256, 2, 8, 9, 11, 30, 1010 % 256, 1010/256,
								  9, 8, 1, 2, 3, 4, 5, 6};

class TestLogMgr_ConvertLogToBinaryArray : public ::testing::Test
{
public:
	TestLogMgr_ConvertLogToBinaryArray()
    {

    }
};


TEST(TestLogMgr_ConvertLogToBinaryArray, logMgr_ConvertLogToBinaryArray)
{
	logMgr_ConvertLogToBinaryArray(log14, logArray1);
	for (int i = 0 ; i < 18; i++)
    	EXPECT_EQ(dataExpect1[i], logArray1[i]);
}


TEST(TestLogMgr_GetNumberOfLog, logMgr_GetNumberOfLog)
{
    	EXPECT_EQ(500, logMgr_GetNumberOfLog(500));
    	EXPECT_EQ(1000, logMgr_GetNumberOfLog(1000));
    	EXPECT_EQ(1000, logMgr_GetNumberOfLog(1500));
}



