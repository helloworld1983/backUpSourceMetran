#ifndef INC_TIMERMOCKFUNCTION_H_
#define INC_TIMERMOCKFUNCTION_H_

#include "gmock.h"
#include "Timer.h"

class FunctionUsedInTimer
{
public:
    virtual ~FunctionUsedInTimer(void){};

    MOCK_METHOD2(xTimerStart, BaseType_t(TimerHandle_t, TickType_t));
    MOCK_METHOD2(xTimerStop, BaseType_t(TimerHandle_t, TickType_t));
    MOCK_METHOD2(xTimerReset, BaseType_t(TimerHandle_t, TickType_t));
    MOCK_METHOD3(xTimerChangePeriod, BaseType_t(TimerHandle_t, TickType_t, TickType_t));
    MOCK_METHOD1(xTimerIsTimerActive, BaseType_t(TimerHandle_t));
};

#endif /* INC_TIMERMOCKFUNCTION_H_ */
