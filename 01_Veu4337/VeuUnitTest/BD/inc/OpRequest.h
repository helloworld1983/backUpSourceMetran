/******************************************************************************/
//$COMMON.H$
//   File Name:  OpRequest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the base class Operator Request.
//      All operator request classes are derived from this base class.
/******************************************************************************/
#ifndef OPREQUEST_H
#define OPREQUEST_H

#include "BdInterface.h"
//#include "Led.h"

/******************************************************************************/
//      TYPE DEFINITIONS
/******************************************************************************/

// E_OpReqState is used to define the current state of an operator request.
enum E_OpReqState
{
    eReqPending, eReqIdle, eReqActive, eReqRestricted // cannot be requested during this state
};

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class OpRequest
{
    public:
        virtual void ProcessRequest (E_OpRequestCommand command);
        OpRequest (void);

//        OpRequest(Led* ledPtr = NULL);
        void AcceptRequest(E_State state = eOn);
        void RejectRequest (void);
        void CompleteRequest (void);

        OpRequest (const OpRequest&);

        //$COMMON.ATTRIBUTE$
        //  Name: LedPtr
        //  Description:  Pointer to the LED object associated with this 
        //      operator request.  If this value is NULL there is no
        //      associated LED.
        //  Units: None
        //  Range: n/a
//        Led* LedPtr;
};

#endif  // OPREQUEST_H

