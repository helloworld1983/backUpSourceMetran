#include "gmock.h"
#include "gtest.h"

//function needed to test
#include "FunctionNeedToTest.h"

//register tests service
#include "AlarmFixture.h"

using namespace ::testing;

class CallingServicesUnitTest : public AlarmTestFixture
{
public:
    CallingServicesUnitTest() : AlarmTestFixture(new BaseModuleMock)
{
}
};


TEST_F(CallingServicesUnitTest, myFunctionTest)
{
    EXPECT_CALL(*_oscillator, GetTime()).Times(3).WillOnce(Return(10))
                                                        .WillOnce(Return(30))
                                                        .WillOnce(Return(120));

    EXPECT_CALL(*_gpio, GpioRead(0, 1)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_gpio, GpioWrite(1 ,1, 1)).Times(1);


    int timeout = 100;
    myFunction(timeout);
}

