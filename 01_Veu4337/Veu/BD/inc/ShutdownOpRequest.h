#ifndef INC_SHUTDOWNOPREQUEST_H_
#define INC_SHUTDOWNOPREQUEST_H_

#include "OpRequest.h"

class ShutdownOpRequest:  public OpRequest
{
    public:
        static ShutdownOpRequest* S_GetInstance(void);

        virtual void ProcessRequest(E_OpRequestCommand command);

    protected:

    private:
        ShutdownOpRequest(void);
        ShutdownOpRequest(const ShutdownOpRequest&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.
        //  Units: None
        //  Range: n/a
        static ShutdownOpRequest* S_Instance;
};

#endif /* INC_SHUTDOWNOPREQUEST_H_ */
