#ifndef INC_MYQUEUE_H_
#define INC_MYQUEUE_H_

#include "standard.h"
#include "CommonTypes.h"

#include "FreeRTOS.h"
#include "queue.h"

class Q
{
    public:
        Q(UNSIGNED QueueLength, UNSIGNED ItemSize);
        virtual ~Q(void);
        STATUS Send(const void * pvItemToQueue, uint32_t xTicksToWait = 0);
        STATUS SendToFront(const void * pvItemToQueue, uint32_t xTicksToWait = 0);
        STATUS SendToBack(const void * pvItemToQueue, uint32_t xTicksToWait = 0);
        STATUS Retrieve(void *pvBuffer, uint32_t xTicksToWait = 0);

    private:
        QueueHandle_t xQueue;
};

#endif /* INC_MYQUEUE_H_ */
