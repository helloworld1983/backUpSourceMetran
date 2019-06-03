//*****************************************************************************/
//$COMMON.H$
//   File Name:  HeatedFilterTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: HeatedFilterTest
//      Test the heated filter backpressure and verify the pressure is
//      within acceptable range.
//*****************************************************************************/

#ifndef HEATEDFILTERTEST_H
#define HEATEDFILTERTEST_H

#include "ChildTest.h"

class HeatedFilterTest : public ChildTest 
{
public:
    HeatedFilterTest(SelfTest*);
    ~HeatedFilterTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    HeatedFilterTest(const HeatedFilterTest&);
    void operator=(HeatedFilterTest&);

    // state machine states
    void ST_MeasureFilterBackpressure(void);
    void ST_DisconnectCircuitFromFilter(void);
    void ST_MeasureNoFilterBackpressure(ResponseData* pData);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_MeasureFilterBackpressure = ChildTest::eST_MaxStates,
        eST_DisconnectCircuitFromFilter,
        eST_MeasureNoFilterBackpressure,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
    
    //$COMMON.ATTRIBUTE$
    //  Name: PressureConn
    //  Description: Measured backpressure with the filter connected.
    //  Units: cmH20 * SCALE
    //  Range: 
    SHORT PressureConn;
    
    //$COMMON.ATTRIBUTE$
    //  Name: PressureDisc
    //  Description: Measured backpressure with the filter disconnected.
    //  Units: cmH20 * SCALE
    //  Range: 
    SHORT PressureDisc;
    
    //$COMMON.ATTRIBUTE$
    //  Name: PressureHF
    //  Description: The calculated filter backpressure.
    //  Units: cmH20 * SCALE
    //  Range: 
    SHORT PressureHF;
    
    static const SHORT PRESSURE_SYS; 
};
#endif //HEATEDFILTERTEST_H

