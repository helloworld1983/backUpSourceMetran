#ifndef INC_MOCKFUNCTIONFORALARMCONTROL_H_
#define INC_MOCKFUNCTIONFORALARMCONTROL_H_

#include "gmock.h"

class AlarmControlService
{
public:
    virtual ~AlarmControlService(void){};
    MOCK_METHOD1(Enunciate, void(int));
};



#endif /* INC_MOCKFUNCTIONFORALARMCONTROL_H_ */
