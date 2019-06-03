//*****************************************************************************/
//$COMMON.H$
//   File Name:  StateMachine.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: StateMachine
//      Provides state machine transition handling.
//*****************************************************************************/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "CommonTypes.h"

#include "EventData.h"

class StateMachine 
{
public:
    StateMachine();
    virtual ~StateMachine() { }

protected:
    void NextState(UCHAR newState, EventData* pData = NULL);
    void StateEngine(void);
    virtual void DoStateAction(EventData* pData) = 0;

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentState
    //  Description: Holds the current state of the derived class's state
    //      machine. 
    //  Units: 
    //  Range: 
    UCHAR CurrentState;

    //$COMMON.ATTRIBUTE$
    //  Name: LastState
    //  Description: Holds the previous state of the derived class's state
    //      machine. 
    //  Units: 
    //  Range: 
    UCHAR LastState;
private:
    // prevent copying
    StateMachine(const StateMachine&);
    void operator=(StateMachine&);

    //$COMMON.ATTRIBUTE$
    //  Name: pEventData
    //  Description: A pointer to pass event data between states.
    //  Units: 
    //  Range: 
    EventData* pEventData;

    //$COMMON.ATTRIBUTE$
    //  Name: EventGenerated
    //  Description: If an event is generated, will be set true.
    //  Units: 
    //  Range: 
    E_Bool EventGenerated;
};

#endif //STATEMACHINE_H

