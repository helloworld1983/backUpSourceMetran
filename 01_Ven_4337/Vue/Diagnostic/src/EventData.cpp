//$COMMON.CPP$
//   File Name:  EventData.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: A base class for handling new and deletes of event data.
//      Structures or classes can inherit from EventData to provide a common
//      means for event data to be passed around. Once the event data is used
//      up it can be deleted. 
//
//      Many different structures/classes will inherit from EventData. Each 
//      derived class may differ in size. The new/delete services are provided
//      by the Allocator class. To have the base class handle any size dynamic 
//      memory allocation (via Allocator), we set up a maximum size that the
//      event data must fit within. This gives all event data types the same
//      size memory block so Allocator can provide memory management services.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "EventData.h"

//ALLOCATOR_CPP_INTERFACE(EventData)



