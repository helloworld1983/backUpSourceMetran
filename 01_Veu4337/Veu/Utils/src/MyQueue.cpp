#include "MyQueue.h"
#include "DebugTrace.h"

Q::Q (UNSIGNED QueueLength, UNSIGNED ItemSize)
{
    xQueue = xQueueCreate(QueueLength, ItemSize);
    ASSERTION(NULL != xQueue);
}

Q::~Q (void)
{
    vQueueDelete(xQueue);
}

STATUS Q::Send (const void* pvItemToQueue, uint32_t xTicksToWait)
{
    STATUS status = 0;
    if(pdPASS == xQueueSend(xQueue, pvItemToQueue, xTicksToWait))
    {
        status = Q_SUCCESS;
    }
    else
    {
        status = Q_FAIL;
    }
    return status;
}

STATUS Q::SendToFront (const void* pvItemToQueue, uint32_t xTicksToWait)
{
    STATUS status = 0;
    if(pdPASS == xQueueSendToFront(xQueue, pvItemToQueue, xTicksToWait))
    {
        status = Q_SUCCESS;
    }
    else
    {
        status = Q_FAIL;
    }
    return status;
}

STATUS Q::SendToBack (const void* pvItemToQueue, uint32_t xTicksToWait)
{
    STATUS status = 0;
    if(pdPASS == xQueueSendToBack(xQueue, pvItemToQueue, xTicksToWait))
    {
        status = Q_SUCCESS;
    }
    else
    {
        status = Q_FAIL;
    }
    return status;
}

STATUS Q::Retrieve (void* pvBuffer, uint32_t xTicksToWait)
{
    STATUS status = 0;
    if(pdPASS == xQueueReceive(xQueue, pvBuffer, xTicksToWait))
    {
        status = Q_SUCCESS;
    }
    else
    {
        status = Q_FAIL;
    }
    return status;
}
