//$COMMON.CPP$
//   File Name:  EventGroup.cpp
//
//   Purpose: Create an event group using in PD task
//              when we have a event from any of module, this events call PD task
//              to process handle for this event
//
//   Interfaces:
//
//   Restrictions: One instance of this class exists
/******************************************************************************/
#include "EventGroup.h"

//freeRTOS includes
#include "FreeRTOS.h"
#include "event_groups.h"

//Utils includes
#include "DebugTrace.h"

EventGroup* EventGroup::S_Instance = NULL;
static EventGroupHandle_t xCreatedEventGroup = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: Returns the pointer to the EventGroup
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: EventGroup pointer
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
EventGroup* EventGroup::S_GetInstance (void)
{
    //if pointer to the EventGroup does not exit, create it
    if(S_Instance == NULL)
    {
        S_Instance = new EventGroup ();
    }
    else
    {
        //warning in terminal
        DEBUG_EVENT_GROUP("EventGroup::S_GetInstance exits\n");
    }
    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EventGroup
//
//    Processing: constructor for EventGroup class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
EventGroup::EventGroup ()
{
    //FreeRTOS create a event Group
    xCreatedEventGroup = xEventGroupCreate ();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_SetEventFlag
//
//    Processing: Set event flag in eventgroup
//
//    Input Parameters: UNSIGNED eventFlags
//
//    Output Parameters: None
//
//    Return Values: uxBits - The value of the event group at the time the call to
//                                  xEventGroupSetBits() returns
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: This function cannot be called from an interrupt
//
/******************************************************************************/
uint32_t EventGroup::S_SetEventFlag (unsigned long eventFlags)
{
    uint32_t uxBits;
    //set event flag use function of FreeRTOS
    uxBits = xEventGroupSetBits (xCreatedEventGroup, eventFlags);

    DEBUG_EVENT_GROUP("Set EventFlag %x \n", uxBits);
    return uxBits;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetEventFlag
//
//    Processing: Get event flag in eventgroup
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: uxBits - current value of the bits in an event group
//                          at the time the call to xEventGroupGetBits
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: This function cannot be called from an interrupt
//
/******************************************************************************/
uint32_t EventGroup::S_GetEventFlag ()
{
    uint32_t uxBits;
    //get the current value of the bits in an event group
    uxBits = xEventGroupGetBits(xCreatedEventGroup);

    DEBUG_EVENT_GROUP("Get event Flag  %x \n", uxBits);
    return uxBits;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_ClearEventFlag
//
//    Processing: Clear bits within an event group
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: The value of the event group
//                          before the specified bits were cleared
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: This function cannot be called from an interrupt
//
/******************************************************************************/
uint32_t EventGroup::S_ClearEventFlag (unsigned long eventFlags)
{
    uint32_t uxBits;
    DEBUG_EVENT_GROUP("Clear event flag %x \n", eventFlags);

    //clear event flag
    uxBits = xEventGroupClearBits (xCreatedEventGroup, eventFlags);

    return uxBits;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_WaitEventFlag
//
//    Processing: block to wait for one or more bits to be set within a
//                      previously created event group
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: The value of the event group at the time either the bits being waited
//                          for became set, or the block time expired
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: This function cannot be called from an interrupt
//
/******************************************************************************/
uint32_t EventGroup::S_WaitEventFlag (unsigned long eventFlags)
{
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    uint32_t uxBits;

    uxBits = xEventGroupWaitBits (xCreatedEventGroup,        //The event Group
            eventFlags,            //The bits within the event group to wait for
            pdTRUE,                 //The bit should be cleared before returning
            pdFALSE,               //Don't wait for all bits, either bit will do
            xTicksToWait);    //Wait a maximum of 100ms for either bit to be set
    return uxBits;
}
