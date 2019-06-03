//*****************************************************************************/
//$COMMON.H$
//   File Name:  CrossoverCircuitTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: CrossoverCircuitTest
//      Tests the crossover circuit to ensure it can toggle appropriately
//      between using the air or O2 sources to keep the safety valve closed. 
//*****************************************************************************/

#ifndef CROSSOVERCIRCUITTEST_H
#define CROSSOVERCIRCUITTEST_H

#include "ChildTest.h"

class CrossoverCircuitTest : public ChildTest 
{
public:
    CrossoverCircuitTest(SelfTest*);
    ~CrossoverCircuitTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    CrossoverCircuitTest(const CrossoverCircuitTest&);
    void operator=(CrossoverCircuitTest&);

    // state machine states
    void ST_CrossoverCircuitBlower(void);
    void ST_ConnectO2(void);
    void ST_VerifyO2Connected(ResponseData*);
    void ST_CrossoverCircuitOxygen(void);
    void ST_CheckValve2(void);

    void DoStateAction(EventData* pData);

    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
		eST_CrossoverCircuitBlower = ChildTest::eST_MaxStates,
		eST_ConnectO2,
		eST_VerifyO2Connected,
		eST_CrossoverCircuitOxygen,
		eST_CheckValve2,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    static const LONG BLOWER_SPINDOWN_FLOW;
};
#endif //CROSSOVERCIRCUITTEST_H

