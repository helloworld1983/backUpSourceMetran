/******************************************************************************/
//$COMMON.H$
//
//    File Name: IPCTask.h
//
//    Class: IPCTask
//
/******************************************************************************/
#ifndef INC_IPCTASK_H_
#define INC_IPCTASK_H_

#include "FreeRTOS.h"
#include "queue.h"

#include "IpcInterface.h"

class IpcTask
{
    public:
        // static instance of IPC task
        static IpcTask *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init(void);

        //Create IPC Task
        void IPCCreateTask(void);

        //send data to IPC Queue
        bool SendToIPCQueue(const void *pvItemToQueue, TickType_t xTicksToWait);

    protected:


    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of IPC task
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static IpcTask *S_Instance;

        //$COMMON.ATTRIBUTE$
        //    Name: IPCQueue
        //
        //    Description: Queue for IPC class
        //
        //    Units: None
        //
        //    Range: N/A
        //
        QueueHandle_t IPCQueue;

        // Entry Point into Alarms Task
        static void Entry (void* pvParameters);

        // Alarms Task constructor
        IpcTask (void);

        //convert data from struct to array buffer
        static uint8_t *ConvertDataToSend(Message *eMessage);

        //Convert data from array buffer to struct
        static Message *ReConvertData(uint8_t *pBuf, uint32_t Size);

};

#endif /* INC_IPCTASK_H_ */
