#ifndef INC_EVENTGROUP_H_
#define INC_EVENTGROUP_H_

//Util includes
#include "stdint.h"

typedef void * EventGroupHandle_t;
typedef uint32_t EventBits_t;
typedef uint32_t TickType_t;
typedef long BaseType_t;

typedef long BaseType_t;
#define pdFALSE         ( ( BaseType_t ) 0 )
#define pdTRUE          ( ( BaseType_t ) 1 )

//define function to mock
EventGroupHandle_t xEventGroupCreate( void );
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet );
EventBits_t xEventGroupGetBits( EventGroupHandle_t xEventGroup);
EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear );
EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait );

//Event Group class
class EventGroup
{
public:
    EventGroup(void){};
};


// static instance of EventGroup class
EventGroup *S_GetInstance (void);

//Set event Flag of EventGroup
uint32_t S_SetEventFlag (unsigned long eventFlags);

//Get event flag of EventGroup
uint32_t S_GetEventFlag ();

//Clear event flag of EventGroup
uint32_t S_ClearEventFlag (unsigned long eventFlags);

//wait for event
uint32_t S_WaitEventFlag (unsigned long eventFlags);

#endif /* INC_EVENTGROUP_H_ */
