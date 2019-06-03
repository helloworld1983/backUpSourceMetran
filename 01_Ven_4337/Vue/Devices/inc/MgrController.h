/******************************************************************************/
//$COMMON.H$
//    Filename: MgrController.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  MgrController
//    This class manage VCVCOntroller and CMVController
/******************************************************************************/
#ifndef INC_MGRCONTROLLER_H_
#define INC_MGRCONTROLLER_H_

#include "CMVController.h"
#include "VCVController.h"

class MgrController
{
public:
    // static instance of MgrController class
    static MgrController *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of CMVController class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static CMVController *CMVServoController;

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of VCVController class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static VCVController *VCVServoController;

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of WatchDogTimer class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static MgrController *S_Instance;

    //WatchDogTimer constructor
    MgrController(void);
};

#endif /* INC_MGRCONTROLLER_H_ */
