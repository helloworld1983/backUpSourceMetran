//$COMMON.H$
//    File Name: EventGroup.h
//
//    Class: EventGroup - Event group for PD task
//
/******************************************************************************/
#ifndef EVENTGROUP_H_
#define EVENTGROUP_H_

//Util includes
#include "stdint.h"

//Event Group class
class EventGroup
{
    public:
        // static instance of EventGroup class
        static EventGroup *S_GetInstance (void);

        //Set event Flag of EventGroup
        uint32_t S_SetEventFlag (unsigned long eventFlags);

        //Get event flag of EventGroup
        uint32_t S_GetEventFlag ();

        //Clear event flag of EventGroup
        uint32_t S_ClearEventFlag (unsigned long eventFlags);

        //wait for event
        uint32_t S_WaitEventFlag (unsigned long eventFlags);

    private:
        //Constructor
        EventGroup ();

        // Entry Point into EventGriup
        static EventGroup *S_Instance;
};

#endif /* EVENTGROUP_H_ */
