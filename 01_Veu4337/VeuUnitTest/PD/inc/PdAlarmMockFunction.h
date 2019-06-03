#ifndef INC_PDALARMMOCKFUNCTION_H_
#define INC_PDALARMMOCKFUNCTION_H_

#include "gmock.h"
#include "PdAlarm.h"

class FunctionUsedInPdAlarm
{
public:
    virtual ~FunctionUsedInPdAlarm(void){};

    MOCK_METHOD1(GetIsActive, E_AlarmStatus(int));
    MOCK_METHOD2(SetIsActive, E_ReturnStatus(int, E_AlarmStatus));

};

#endif /* INC_PDALARMMOCKFUNCTION_H_ */
