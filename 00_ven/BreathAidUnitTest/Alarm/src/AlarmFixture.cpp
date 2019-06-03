#include "AlarmFixture.h"

// Modules mocks
std::unique_ptr<BaseModuleMock> AlarmTestFixture::_modulesMocks;


//Add Services
std::unique_ptr<OscillatorService> AlarmTestFixture::_oscillator;
std::unique_ptr<GpioService> AlarmTestFixture::_gpio;

//For example
//std::unique_ptr<FunctionUsedInTimer> UtilTestFixture::_FunctionUsedInTimer;
