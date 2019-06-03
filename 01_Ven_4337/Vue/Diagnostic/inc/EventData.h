//*****************************************************************************/
//$COMMON.H$
//   File Name:  EventData.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: EventData
//      A base class for handling new and deletes of event data.
//*****************************************************************************/

#ifndef EventData_H
#define EventData_H

//#inclubde "Allocator.h"

class EventData 
{
    // EventData can have max of 20 instances and a max size of 64 bytes
//    ALLOCATOR_H_INTERFACE_OVERRIDE(EventData, 20, 64)
public:
    EventData() {}
    virtual ~EventData() {}
};
#endif //EventData_H

