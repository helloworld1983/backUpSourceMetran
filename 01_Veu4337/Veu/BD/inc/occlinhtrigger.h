/******************************************************************************/
//$COMMON.H$
//   File Name:  OcclInhTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the Occlusion In Inhalation
//      Alarm Trigger class.  This class is derived from the OcclTrigger
//      class.
/******************************************************************************/
#ifndef OCCLINHTRIGGER_H
#define OCCLINHTRIGGER_H

#include "OcclTrigger.h"
#include "DebugTrace.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class OcclInhTrigger : public OcclTrigger
{
    public:
        static inline OcclInhTrigger* S_GetInstance (void);

    protected:

    private:
        OcclInhTrigger (void);
        OcclInhTrigger (const OcclInhTrigger&);
        virtual LONG GetFlow (void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static OcclInhTrigger* S_Instance;

};

/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If another object wants access
//      to the OcclInhTrigger object, it calls OcclInhTrigger::S_GetInstance(),
//      which returns a pointer to the OcclInhTrigger object.
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
//      OcclInhTrigger* - pointer to the object instantiated from this class
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
inline OcclInhTrigger* OcclInhTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new OcclInhTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

#endif // OCCLINHTRIGGER_H
