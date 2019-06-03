#include "Fixture.h"

int GetTime()
{
    return TestFixture::_oscillator->GetTime();
}


int GpioRead(int portNumber, int pinNumber)
{
    return TestFixture::_gpio->GpioRead(portNumber, pinNumber);
}


int GpioWrite(int portNumber, int pinNumber , int value)
{
    return TestFixture::_gpio->GpioWrite(portNumber, pinNumber, value);
}

