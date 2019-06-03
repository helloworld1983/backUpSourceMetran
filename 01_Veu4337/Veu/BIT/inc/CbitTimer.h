//******************************************************************************/
//$COMMON.H$
//   File Name:  CbitTimer.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the CBIT Timer.  The
//      CbitTimer class is derived from the Nucleus Plus RTOS base Timer class.
/******************************************************************************/
#ifndef CBITTIMER_H
#define CBITTIMER_H

#include "CbitTask.h"
#include "Timer.h"

class CbitTimer : public Timer
{
    public:
        CbitTimer(CHAR* name, UNSIGNED time, E_CbitTestId testId);
        
        virtual void ExpirationRoutine(void);

    protected:

    private:
        CbitTimer(void);
        CbitTimer(const CbitTimer&);

        //$COMMON.ATTRIBUTE$
        //  Name: TestId
        //  Description:  Test identifier.
        //  Units: None
        //  Range: n/a
        E_CbitTestId TestId;
};

#endif CBITTIMER_H


