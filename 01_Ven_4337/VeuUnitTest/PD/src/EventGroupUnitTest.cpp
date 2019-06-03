#include "EnableTestCase.h"
#ifdef PD_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "PDFixture.h"
#include "EventGroup.h"

#include "DebugTrace.h"

using namespace ::testing;


static EventGroupHandle_t xCreatedEventGroup = NULL;

#define configTICK_RATE_HZ          ( ( TickType_t ) 1000 )
#define portTICK_PERIOD_MS          ( ( TickType_t ) 1000 / configTICK_RATE_HZ )

EventGroup* S_Instance = NULL;

EventGroup* S_GetInstance (void)
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

uint32_t S_SetEventFlag (unsigned long eventFlags)
{
    uint32_t uxBits;
    //set event flag use function of FreeRTOS
    uxBits = xEventGroupSetBits (xCreatedEventGroup, eventFlags);

    DEBUG_EVENT_GROUP("Set EventFlag %x \n", uxBits);
    return uxBits;
}

uint32_t S_GetEventFlag ()
{
    uint32_t uxBits;
    //get the current value of the bits in an event group
    uxBits = xEventGroupGetBits(xCreatedEventGroup);

    DEBUG_EVENT_GROUP("Get event Flag  %x \n", uxBits);
    return uxBits;
}

uint32_t S_ClearEventFlag (unsigned long eventFlags)
{
    uint32_t uxBits;
    DEBUG_EVENT_GROUP("Clear event flag %x \n", eventFlags);

    //clear event flag
    uxBits = xEventGroupClearBits (xCreatedEventGroup, eventFlags);

    return uxBits;
}

uint32_t S_WaitEventFlag (unsigned long eventFlags)
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

class EventGroupUnitMock : public PDTestFixture
{
public:
    EventGroupUnitMock(void) : PDTestFixture(new BaseModuleMock)
{
}
    virtual ~EventGroupUnitMock(void){}
    virtual void SetUp(void)
    {
        MyEventGroupUnitTest = new EventGroup();
    }
    virtual void TearDown(void)
    {
        delete MyEventGroupUnitTest;
    }
    EventGroup *MyEventGroupUnitTest;
};

class EventGroupTest : public ::testing::Test
{
public:
    EventGroupTest(void)
    {
    }
    virtual ~EventGroupTest(void){}
    virtual void SetUp(void)
    {
        MyEventGroup = new EventGroup();
    }
    virtual void TearDown(void)
    {
        delete MyEventGroup;
    }
    EventGroup *MyEventGroup;
};


//test for EventGroup* EventGroup::S_GetInstance (void)
TEST_F(EventGroupTest, EventGroupTest_S_GetInstance)
{
    S_GetInstance();
    EXPECT_NE((void*)0, S_Instance);
}

//test for uint32_t EventGroup::S_SetEventFlag (unsigned long eventFlags)
TEST_F(EventGroupUnitMock, EventGroupUnitMock_S_SetEventFlag)
{
    EXPECT_CALL(*_EventGroupMock, xEventGroupSetBits(xCreatedEventGroup, 0)).Times(1).WillOnce(Return(1));

    EXPECT_EQ(1, S_SetEventFlag(0));
}

//test for uint32_t EventGroup::S_GetEventFlag ()
TEST_F(EventGroupUnitMock, EventGroupUnitMock_S_GetEventFlag)
{
    EXPECT_CALL(*_EventGroupMock, xEventGroupGetBits(xCreatedEventGroup)).Times(1).WillOnce(Return(1));

    EXPECT_EQ(1, S_GetEventFlag());
}

//Test for uint32_t EventGroup::S_ClearEventFlag (unsigned long eventFlags)
TEST_F(EventGroupUnitMock, EventGroupUnitMock_S_ClearEventFlag)
{
    EXPECT_CALL(*_EventGroupMock, xEventGroupClearBits(xCreatedEventGroup, 1)).Times(1).WillOnce(Return(2));

    EXPECT_EQ(2, S_ClearEventFlag(1));
}

//test for uint32_t EventGroup::S_WaitEventFlag (unsigned long eventFlags)
TEST_F(EventGroupUnitMock, EventGroupUnitMock_S_WaitEventFlag)
{
    EXPECT_CALL(*_EventGroupMock, xEventGroupWaitBits(xCreatedEventGroup, 1, pdTRUE, pdFALSE, 100 / portTICK_PERIOD_MS)).Times(1).WillOnce(Return(2));
    EXPECT_EQ(2, S_WaitEventFlag(1));
}

#endif  //end define PD_UNIT_TEST
