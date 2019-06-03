#ifndef SERVICEMOCKFORFUNTIONNEEDTOTEST_H_
#define SERVICEMOCKFORFUNTIONNEEDTOTEST_H_

#include "gmock.h"
//class mock for GPIO
class GpioService
{
public:
    virtual ~GpioService(void) {} // IMPORTANT: This is needed by std::unique_ptr

    MOCK_METHOD2(GpioRead, int(int, int));
    MOCK_METHOD3(GpioWrite, int(int, int, int));
};

//class mock for GetTime by OscillatorService
class OscillatorService
{
public:
    virtual ~OscillatorService(void) {} // IMPORTANT: This is needed by std::unique_ptr

    MOCK_METHOD0(GetTime, int());
};
#endif /* SERVICEMOCKFORFUNTIONNEEDTOTEST_H_ */
