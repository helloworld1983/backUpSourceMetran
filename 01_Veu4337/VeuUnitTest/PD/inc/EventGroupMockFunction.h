#ifndef INC_EVENTGROUPMOCKFUNCTION_H_
#define INC_EVENTGROUPMOCKFUNCTION_H_

#include "gmock.h"
#include "EventGroup.h"

class FunctionUsedInEvenGroup
{
public:
    virtual ~FunctionUsedInEvenGroup(void){};

    MOCK_METHOD0(xEventGroupCreate, EventGroupHandle_t(void));
    MOCK_METHOD2(xEventGroupSetBits, EventBits_t(EventGroupHandle_t, const EventBits_t));
    MOCK_METHOD1(xEventGroupGetBits, EventBits_t(EventGroupHandle_t));
    MOCK_METHOD2(xEventGroupClearBits, EventBits_t(EventGroupHandle_t, const EventBits_t));
    MOCK_METHOD5(xEventGroupWaitBits, EventBits_t(EventGroupHandle_t, const EventBits_t,
                                                  const BaseType_t, const BaseType_t, TickType_t));

};

#endif /* INC_EVENTGROUPMOCKFUNCTION_H_ */
