/******************************************************************************/
//$COMMON.H$
//   File Name:  HighMAP_2AlarmTrigger.h
//
//   Class: 
//      This file contains the declaration for the HighMAP_2 Alarm Trigger
//      class.The HighMAP_2 Alarm Trigger is derived from Trigger class 
//
//   Revision History:
/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef HIGHMAP_2ALARMTRIGGER_H
#define HIGHMAP_2ALARMTRIGGER_H

#include "Trigger.h"
#include <QDebug>
/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class HighMAP_2AlarmTrigger : public Trigger
{
    public:
        static inline HighMAP_2AlarmTrigger* S_GetInstance(void);
        virtual E_Bool CheckCondition(void);
		virtual inline void ProcessTrigger(Mode* mode);
                inline void SetInactiveTimer(USHORT value){ InactiveTimer = value;}

    protected:

    private:
        HighMAP_2AlarmTrigger(void);
        HighMAP_2AlarmTrigger(const HighMAP_2AlarmTrigger&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static HighMAP_2AlarmTrigger* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: InactiveTimer
        //  Description:  Keep track the time of being inactive.  
        //  Units: None
        //  Range: n/a
        USHORT InactiveTimer;
};      


/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessHighMAP_2AlarmTrigger() operation of 
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
inline void HighMAP_2AlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the High MAP_2 trigger in the current mode.
    mode->ProcessHighMAP_2AlarmTrigger();

}   // end ProcessTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the HighMAP_2AlarmTrigger object, it invokes 
//      HighMAP_2AlarmTrigger::S_GetInstance(), which returns a pointer 
//      to the HighMAP_2AlarmTrigger object.
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
//      HighMAP_2AlarmTrigger* - pointer to the instance of this class
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
inline HighMAP_2AlarmTrigger* HighMAP_2AlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new HighMAP_2AlarmTrigger(); 
        ASSERTION(S_Instance != NULL);
    }
   
    return(S_Instance);

}   // end HighMAP_2AlarmTrigger()

#endif // HIGHMAP_2ALARMTRIGGER_H
#endif // HFO_SYSTEM
