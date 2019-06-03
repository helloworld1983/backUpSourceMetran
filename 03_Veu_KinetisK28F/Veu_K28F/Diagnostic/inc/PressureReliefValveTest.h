//*****************************************************************************/
//$COMMON.H$
//   File Name:  PressureReliefValveTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: PressureReliefValveTest
//      Check the pressure relief valve to ensure it opens within the 
//      correct pressure range.
//*****************************************************************************/

#ifndef PRESSURERELIEFVALVETEST_H
#define PRESSURERELIEFVALVETEST_H

#include "ChildTest.h"

class PressureReliefValveTest : public ChildTest
{
public:
    PressureReliefValveTest(SelfTest*);
    ~PressureReliefValveTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    
protected:
private:
    // prevent copying
    PressureReliefValveTest(const PressureReliefValveTest&);
    void operator=(PressureReliefValveTest&);

    // state machine states
	void ST_CrackFlowPressure(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
		eST_CrackFlowPressure = ChildTest::eST_MaxStates,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    E_Bool GetStabilizedPressure(LONG, SHORT, UNSIGNED, UNSIGNED, SHORT*, 
        E_Bool*, E_Bool*);
};
#endif //PRESSURERELIEFVALVETEST_H

