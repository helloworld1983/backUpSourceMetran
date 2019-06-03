#include "AlarmFixture.h"

//include function header to mock

//fake service for TImer class


//for example
//BaseType_t xTimerStart(TimerHandle_t handle, TickType_t CmdTimeout)
//{
//    return UtilTestFixture::_FunctionUsedInTimer->xTimerStart(handle, CmdTimeout);
//}

int GetTime()
{
    return AlarmTestFixture::_oscillator->GetTime();
}


int GpioRead(int portNumber, int pinNumber)
{
    return AlarmTestFixture::_gpio->GpioRead(portNumber, pinNumber);
}


int GpioWrite(int portNumber, int pinNumber , int value)
{
    return AlarmTestFixture::_gpio->GpioWrite(portNumber, pinNumber, value);
}
