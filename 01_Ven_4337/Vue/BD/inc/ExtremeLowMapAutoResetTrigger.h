/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeLowMapAutoResetTrigger.h
//
//   Class: 
//      This file contains the declaration for the ExtremeLowMapAutoResetTrigger
//      class.  The ExtremeLowMapAutoResetTrigger  is derived from Trigger
//      class 
//
//   Revision History:
/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef EXTREMELOWMAPAUTORESETTRIGGER_H
#define EXTREMELOWMAPAUTORESETTRIGGER_H

#include "Trigger.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class ExtremeLowMapAutoResetTrigger : public Trigger
{
    public:
        static inline ExtremeLowMapAutoResetTrigger* S_GetInstance(void);
        virtual E_Bool CheckCondition(void);
		virtual void EnableTrigger(void);
		virtual inline void ProcessTrigger(Mode* mode);

    protected:

    private:
        ExtremeLowMapAutoResetTrigger(void);
        ExtremeLowMapAutoResetTrigger(const ExtremeLowMapAutoResetTrigger&);



        //$COMMON.ATTRIBUTE$
        //  Name: TimeCounter
        //  Description: Delay 1 sec after entering Extreme Low Map , then check pressure
        //  Units: TICKS
        //  Range: 0 - maximum UNSIGNED value
        UNSIGNED TimeCounter;


        //$COMMON.ATTRIBUTE$
        //  Name: OneSecTimeCounter
        //  Description:  System Time that the condition was met; used to
        //      insure the criteria are met for a minimum amount of time
        //  Units: TICKS
        //  Range: 0 - maximum UNSIGNED value
        UNSIGNED OneSecTimeCounter;



		//$COMMON.ATTRIBUTE$
        //  Name: IsArmed
        //  Description:  Indicates if the trigger has fired but
        //  not honored yet.
        //  Units: None
        //  Range: eTrue, eFalse
        E_Bool IsArmed;

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static ExtremeLowMapAutoResetTrigger* S_Instance;
};

/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ExtremeLowMapAutoResetTrigger() operation of 
//      the HFO mode.
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
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
inline void ExtremeLowMapAutoResetTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the High MAP_2 trigger in the current mode.
    mode->ProcessExtremeLowMapAutoResetTrigger();

}   // end ProcessTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the HighMAP_2AutoResetTrigger object, it invokes 
//      ExtremeLowMapAutoResetTrigger::S_GetInstance(), which returns a pointer 
//      to the ExtremeLowMapAutoResetTrigger object.
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
//      ExtremeLowMapAutoResetTrigger* - pointer to the instance of this class
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
inline ExtremeLowMapAutoResetTrigger* ExtremeLowMapAutoResetTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new ExtremeLowMapAutoResetTrigger(); 
        ASSERTION(S_Instance != NULL);
    }
   
    return(S_Instance);

}   // end ExtremeLowMapAutoResetTrigger()

#endif // HIGHMAP_2AUTORESETTRIGGER_H
#endif // HFO_SYSTEM
