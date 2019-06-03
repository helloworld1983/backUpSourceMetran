/******************************************************************************/
//$COMMON.H$
//   File Name:  OilPumpOpRequest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the Oil Pump Operator
//      Request.  OilPumpOpRequest is derived from the Nucleus Plus RTOS Timer
//      class and the OpRequest class.
//
/******************************************************************************/
#ifndef INC_OILPUMPOPREQUEST_H_
#define INC_OILPUMPOPREQUEST_H_

#include "Timer.h"
#include "OpRequest.h"

class OilPumpOpRequest :  public OpRequest, public Timer
{
    public:
        static OilPumpOpRequest* S_GetInstance(void);

        virtual void ProcessRequest(E_OpRequestCommand command);
        virtual void ExpirationRoutine(void);

    protected:

    private:
        OilPumpOpRequest(void);
        OilPumpOpRequest(const OilPumpOpRequest&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.
        //  Units: None
        //  Range: n/a
        static OilPumpOpRequest* S_Instance;
};

#endif /* INC_OILPUMPOPREQUEST_H_ */
