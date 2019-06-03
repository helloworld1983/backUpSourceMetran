/******************************************************************************/
//$COMMON.H$
//   File Name:  HighTempTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: The HighTempTest class is derived from the Test class.
//      It serves the specific purpose of testing the internal temperature
//      sensor of the ventilator.
/******************************************************************************/
#ifndef HIGHTEMPTEST_H
#define HIGHTEMPTEST_H

#include "Test.h"

#include "DebugTrace.h"

class HighTempTest : public Test
{
    public:
        static inline HighTempTest* S_GetInstance(void);
  
        virtual E_TestStatus Run(void);

    protected:

    private:
        HighTempTest(const HighTempTest&);
        HighTempTest(void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static HighTempTest* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: NumHighAlarmReadings
        //  Description:  Number of readings that exceed the high alarm limit.
        //  Units: None
        //  Range: n/a
        SHORT NumHighAlarmReadings;

        void CheckAlarmCondition(SHORT tempReading);
          
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the HighTempTest object, it invokes 
//      HighTempTest::S_GetInstance(), which returns a pointer to the 
//      HighTempTest object.
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
//      HighTempTest* - pointer to the object instantiated from this class
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
inline HighTempTest* HighTempTest::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new HighTempTest();
        ASSERTION( S_Instance != NULL );
    }
   
    return(S_Instance);

}   // end S_GetInstance()




#endif // HIGHTEMPTEST_H
