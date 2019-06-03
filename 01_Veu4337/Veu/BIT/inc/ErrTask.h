//******************************************************************************/
//$COMMON.H$
//   File Name:  ErrTask.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the ErrTask.  The
//      ErrTask class is derived from the Nucleus Plus RTOS base Task class.
//
//   Revision History:
/******************************************************************************/
#ifndef ERRTASK_H
#define ERRTASK_H

#include "CommonConstants.h"
#include "LogEntry.h"

#include "DebugTrace.h"

// This value defines the maximum message size for a message on the Err Queue.
// The size is specified in the number of UNSIGNED data elements.  
// If the message is not an exact multiple of an UNSIGNED, it is rounded up
// to the next size.  
static const UNSIGNED ERR_Q_MSG_SIZE = 
    (sizeof(E_EventCode) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);
//
// Forward Class Reference
//
//class Csi;

class ErrTask //: public Task
{
    //friend class Csi;   // CSI ISR() needs access to event flags

    public:

        //$COMMON.ATTRIBUTE$
        //  Name: AirMotorError
        //  Description:  Flag indicating air motor got an error  
        //  Units: None
        //  Range: n/a
        static E_Bool AirMotorError;

        //$COMMON.ATTRIBUTE$
        //  Name: AirMotorError
        //  Description:  Flag indicating O2 motor got an error  
        //  Units: None
        //  Range: n/a
        static E_Bool O2MotorError;

        //$COMMON.ATTRIBUTE$
        //  Name: ExhMotorError
        //  Description:  Flag indicating exhaltion motor got an error  
        //  Units: None
        //  Range: n/a
        static E_Bool ExhMotorError;

        static inline ErrTask* S_GetInstance(void);

        STATUS Send(E_EventCode code, UNSIGNED suspend = NU_NO_SUSPEND);

		void CheckMotorErrors(void);


    protected:
        virtual void Entry(void);

    private:
        ErrTask(void);
        ErrTask(const ErrTask&);

		void DoEventCode(E_EventCode code);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static ErrTask* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: QueuePointer
        //  Description:  Pointer to Err Task's Queue
        //  Units: None
        //  Range: n/a
//        Q* QueuePtr;

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the ErrTask object, it invokes 
//      ErrTask::S_GetInstance(), which returns a pointer to the 
//      ErrTask object.
//
//      If the object has not already been created, this method instantiates 
//      it and returns a pointer to the object.  If the object has already been 
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ErrTask* - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
inline ErrTask* ErrTask::S_GetInstance(void)
{
    // If the object has not already been created, create it.

    if (NULL == S_Instance)    
    {
        S_Instance = new ErrTask();
        ASSERTION(S_Instance != NULL);
    }
   
    return(S_Instance);

}   // end S_GetInstance()







#endif // ERRTASK_H



