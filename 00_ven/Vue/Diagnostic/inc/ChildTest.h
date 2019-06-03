//*****************************************************************************/
//$COMMON.H$
//   File Name:  ChildTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: ChildTest
//      Provide a common base for all child tests. 
//*****************************************************************************/

#ifndef CHILDTEST_H
#define CHILDTEST_H

#include "SelfTest.h"
#include "CommonConstants.h"  // Need UNSIGNED definition

class ChildTest : public SelfTest
{
public:
    ChildTest(SelfTest*, E_StringID = eNoString);
    ~ChildTest() { }

protected:
    // state machine states
    virtual void ST_Idle(EventData *);
    virtual void ST_Complete(EventData *);
    virtual void ST_Cancel(EventData *);
    
    enum E_States 
    {
        eST_Idle = 0,
        eST_Complete,
        eST_Cancel,
        eST_MaxStates
    };
private:
    // prevent copying
    ChildTest(const ChildTest&);
    void operator=(ChildTest&);
};
#endif //CHILDTEST_H

