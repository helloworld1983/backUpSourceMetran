/******************************************************************************/
//$COMMON.H$
//   File Name:  AdcDacTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The AdcDacTest class defines the A/D - D/A Wraparound test 
//      performed during CBIT and is derived from the Test base class.
/******************************************************************************/
#ifndef ADCDACTEST_H
#define ADCDACTEST_H

#include "Test.h"

#include "DebugTrace.h"

static const SHORT NUM_ADC_DAC_TEST_POINTS = 6;

// E_OpReqState is used to define the current state of an operator request.
enum E_AdcDacIx
{
    eDacOutput,
    eMinAdcInput,
    eMaxAdcInput,
    eNumAdcDacTableIx
};

class AdcDacTest : public Test
{
    public:
        static inline AdcDacTest* S_GetInstance(void);

        virtual E_TestStatus Run(void);

    protected:


    private:
        AdcDacTest(void);
        AdcDacTest(const AdcDacTest&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static AdcDacTest* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name:   Index
        //  Description: Current offset into the test data array 
        //  Units: None
        //  Range: n/a
        SHORT Index;

        //$COMMON.ATTRIBUTE$
        //  Name:   AdcDacData[]
        //  Description: Data table containing output test values
        //      and acceptable input limits.
        //  Units: None
        //  Range: n/a
        static SHORT AdcDacData[NUM_ADC_DAC_TEST_POINTS][eNumAdcDacTableIx];

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
//      access to the AdcDacTest object, it invokes AdcDacTest::S_GetInstance(),
//      which returns a pointer to the AdcDacTest object.
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
//      AdcDacTest* - pointer to the object instantiated from this class
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
inline AdcDacTest* AdcDacTest::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new AdcDacTest();
        ASSERTION( S_Instance != NULL );
    }
   
    return(S_Instance);

}   // end S_GetInstance()


#endif // ADCDACTEST_H
