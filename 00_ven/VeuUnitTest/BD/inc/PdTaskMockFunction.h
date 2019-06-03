#ifndef INC_PDTASKMOCKFUNCTION_H_
#define INC_PDTASKMOCKFUNCTION_H_

#include "gmock.h"

class PdMockFunction
{
public :
    virtual ~PdMockFunction(){};

    MOCK_METHOD1(PdTaskS_SetEventFlag, void(int));

};

#endif /* INC_PDTASKMOCKFUNCTION_H_ */
