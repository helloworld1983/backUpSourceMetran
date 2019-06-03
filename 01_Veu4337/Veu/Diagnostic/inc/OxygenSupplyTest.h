//*****************************************************************************/
//$COMMON.H$
//   File Name:  OxygenSupplyTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: OxygenSupplyTest
//      To ensure the oxygen supply sensor correctly detects the existence
//      of an O2 source connected to the instrument. When this test completes
//      successfully the O2 source should be left disconnected to the 
//      instrument. 
//*****************************************************************************/

#ifndef OXYGENSUPPLYTEST_H
#define OXYGENSUPPLYTEST_H

#include "ChildTest.h"

class OxygenSupplyTest : public ChildTest
{
public:
    OxygenSupplyTest(SelfTest*);
    ~OxygenSupplyTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    OxygenSupplyTest(const OxygenSupplyTest&);
    void operator=(OxygenSupplyTest&);

    // state machine states
	void ST_GetO2State(void);
    void ST_DisconnectO2(void);
    void ST_VerifyO2Disconnected(ResponseData* pData);
    void ST_ConnectO2(void);
    void ST_VerifyO2Connected(ResponseData* pData);

    void DoStateAction(EventData* pData);

    void BleedExcessO2(void);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_GetO2State = ChildTest::eST_MaxStates,
		eST_DisconnectO2,
		eST_VerifyO2Disconnected,
		eST_ConnectO2,
		eST_VerifyO2Connected,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};
#endif //OXYGENSUPPLYTEST_H

