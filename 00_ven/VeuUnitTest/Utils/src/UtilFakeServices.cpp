#include "UtilFixture.h"
#include "Timer.h"

//fake service for TImer class
BaseType_t xTimerStart(TimerHandle_t handle, TickType_t CmdTimeout)
{
    return UtilTestFixture::_FunctionUsedInTimer->xTimerStart(handle, CmdTimeout);
}
BaseType_t xTimerStop(TimerHandle_t handle, TickType_t CmdTimeout)
{
    return UtilTestFixture::_FunctionUsedInTimer->xTimerStop(handle, CmdTimeout);
}
BaseType_t xTimerReset(TimerHandle_t handle, TickType_t CmdTimeout)
{
    return UtilTestFixture::_FunctionUsedInTimer->xTimerReset(handle, CmdTimeout);
}
BaseType_t xTimerChangePeriod(TimerHandle_t handle, TickType_t NewPeriod, TickType_t CmdTimeout)
{
    return UtilTestFixture::_FunctionUsedInTimer->xTimerChangePeriod(handle, NewPeriod, CmdTimeout);
}
BaseType_t xTimerIsTimerActive(TimerHandle_t handle)
{
    return UtilTestFixture::_FunctionUsedInTimer->xTimerIsTimerActive(handle);
}
