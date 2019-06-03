#ifndef INC_TIMERMOCKFUNCTION_H_
#define INC_TIMERMOCKFUNCTION_H_

#include "gmock.h"

class TimerMockFunction
{
public:
    MOCK_METHOD0(mStart, void(void));
    MOCK_METHOD0(mStop, void(void));
    MOCK_METHOD0(mReset, void(void));
};


#endif /* INC_TIMERMOCKFUNCTION_H_ */
