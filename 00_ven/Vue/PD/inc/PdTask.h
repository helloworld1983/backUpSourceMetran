//$COMMON.H$
//    File Name: PdTask.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: PdTask - Patient Data Task
//    Class: O2Timer - Timer for scheduling O2 processing 
/******************************************************************************/
#ifndef PDTASK_H
#define PDTASK_H

//Utils includes
#include "CommonConstants.h"
#include "DebugTrace.h"

//PD includes
#include "PdInterface.h"

#include "Timer.h"

// Patient Data Task
class PdTask
{
public:
    // static instance of patient data task
    static PdTask* S_GetInstance (void);

    // event flags used by the patient data task
    static STATUS S_SetEventFlag (UNSIGNED eventFlags);

    //Create PD task
    void PDCreateTask (void);

protected:
    // static instance of patient data
    static PdTask* S_Instance;

    // Entry Point into Patient Data Task
    static void Entry (void* pvParameters);

private:
    // Patient Data Task constructor
    PdTask (void);

};

#endif //PDTASK_H
