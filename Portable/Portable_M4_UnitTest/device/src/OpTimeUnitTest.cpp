/********************************************************************/
/*                                                                  */
/* File Name     : OpTimeUnitTest.cpp                               */
/*                                                                  */
/* General       : This module contain function for Unit Test       */
/*                                                                  */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* Company       : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*                                                                  */
/*                                                                  */
/********************************************************************/
#include "gtest.h"
using ::testing::TestWithParam;

#define DEVICE_TASK_DELAY_TIME      5
#define AFTER_0MS_LOOP_CNT 0/DEVICE_TASK_DELAY_TIME
#define AFTER_200MS_LOOP_CNT 200/DEVICE_TASK_DELAY_TIME
#define AFTER_1200MS_LOOP_CNT 1200/DEVICE_TASK_DELAY_TIME
#define AFTER_1400MS_LOOP_CNT 1400/DEVICE_TASK_DELAY_TIME
#define AFTER_2400MS_LOOP_CNT 2400/DEVICE_TASK_DELAY_TIME
#define AFTER_2600MS_LOOP_CNT 2600/DEVICE_TASK_DELAY_TIME

uint8_t OperationTime_HandleDisplay(uint16_t s_cnt)
{

    switch(s_cnt)
    {
    case AFTER_0MS_LOOP_CNT:
        return (uint8_t)AFTER_0MS_LOOP_CNT;
        break;
    case AFTER_200MS_LOOP_CNT:
        return (uint8_t)AFTER_200MS_LOOP_CNT;
        break;
    case AFTER_1200MS_LOOP_CNT:
        return (uint8_t)AFTER_1200MS_LOOP_CNT;
        break;
    case AFTER_1400MS_LOOP_CNT:
        return (uint8_t)AFTER_1400MS_LOOP_CNT;
        break;
    case AFTER_2400MS_LOOP_CNT:
        return (uint8_t)AFTER_2400MS_LOOP_CNT;
        break;
    case AFTER_2600MS_LOOP_CNT:
        return (uint8_t)AFTER_2600MS_LOOP_CNT;
        break;
    default:
        break;
    }
}
struct HandleDisplayTest
{
        uint16_t s_cnt;
        uint8_t result;
};
struct HandleDisplayTest HandleDisplayTestCase[] =
{
        {AFTER_0MS_LOOP_CNT, (uint8_t)AFTER_0MS_LOOP_CNT},
        {AFTER_200MS_LOOP_CNT, (uint8_t)AFTER_200MS_LOOP_CNT},
        {AFTER_1200MS_LOOP_CNT, (uint8_t)AFTER_1200MS_LOOP_CNT},
        {AFTER_1400MS_LOOP_CNT, (uint8_t)AFTER_1400MS_LOOP_CNT},
        {AFTER_2400MS_LOOP_CNT, (uint8_t)AFTER_2400MS_LOOP_CNT},
        {AFTER_2600MS_LOOP_CNT, (uint8_t)AFTER_2600MS_LOOP_CNT}

};
class HandleDisplayUnitTest : public ::testing::TestWithParam<HandleDisplayTest>
{
};

TEST_P(HandleDisplayUnitTest, HandleDisplayTestCaseList)
{
    EXPECT_EQ(GetParam().result, OperationTime_HandleDisplay(GetParam().s_cnt));
}
INSTANTIATE_TEST_CASE_P(HandleDisplayUnitTest,
                        HandleDisplayUnitTest,
                        testing::ValuesIn(HandleDisplayTestCase)
);


