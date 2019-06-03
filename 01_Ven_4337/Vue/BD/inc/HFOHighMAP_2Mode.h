/******************************************************************************/
//$COMMON.H$
//   File Name:  HFOHighMAP_2Mode.h
//
//   Class: 
//      This file contains the class declaration for the HFOHighMAP_2 Mode
//      which is derived from the NonVentMode base class.
//
//   Revision History:
//
/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef HFOHIGHMAP_2MODE_H
#define HFOHIGHMAP_2MODE_H

#include "AlarmBreathingMode.h"

class HFOHighMAP_2Mode : public AlarmBreathingMode
{
    public:
        static inline HFOHighMAP_2Mode* S_GetInstance(void);
        virtual void Enter(E_VentilationMode fromModeId);
        virtual void Exit(void);


    protected:

    private:
        HFOHighMAP_2Mode(void);
        HFOHighMAP_2Mode(const HFOHighMAP_2Mode&);

        virtual void UpdateAlarmStatus(void);
		
        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static HFOHighMAP_2Mode* S_Instance;
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
//      to the object comes via this operation.  If any other object wants 
//      access to the HFOHighMAP_2Mode object, it invokes HFOHighMAP_2Mode::S_GetInstance(),
//      which returns a pointer to the HFOHighMAP_2Mode object.
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
//      HFOHighMAP_2Mode* - pointer to the object instantiated from this class
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
inline HFOHighMAP_2Mode* HFOHighMAP_2Mode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new HFOHighMAP_2Mode();
        ASSERTION(S_Instance != NULL);
    }
   
    return(S_Instance);

}   // end S_GetInstance()

#endif // HFOHIGHMAP_2MODE_H
#endif // HFO_SYSTEM
