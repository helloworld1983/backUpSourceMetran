/******************************************************************************/
//$COMMON.H$
//   File Name:  MgrOpRequest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Operator Request manager class provides a common interface to
//      all Operator Requests such as 100% O2, Manual Inhalation and 
//      Exhalation Hold.
/******************************************************************************/
#ifndef MGROPREQUEST_H
#define MGROPREQUEST_H

#include "BdInterface.h"
#include "OpRequest.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class MgrOpRequest
{
    public:
        static MgrOpRequest* S_GetInstance (void);
        void ReceiveRequest (const BdEvent& bdEvent);

    protected:

    private:
        MgrOpRequest (void);
        MgrOpRequest (const MgrOpRequest&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static MgrOpRequest* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: OpReqPtrList
        //  Description:  
        //      Array of pointers to operator requests.  Each element of the 
        //      array contains a pointer to an operator request object.
        //  Units: None
        //  Range: n/a
        OpRequest* OpReqPtrList[eLastOpRequestId - eFirstOpRequestId + 1];

};

#endif  // MGROPREQUEST_H
