#include "IpcTask.h"
#include "IpcInterface.h"

#include "DebugTrace.h"
#include "Standard.h"

//#include "UsbVcom.h"
//#include "cdc_vcom.h"

#include "FreeRTOS.h"
#include "task.h"

#include "TaskCommon.h"

#define IPC_QUEUE_LENGTH    (10)

IpcTask *IpcTask::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      * - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
IpcTask* IpcTask::S_GetInstance (void)
{
    //Create object if it is NULL
    if(NULL == S_Instance)
    {
        S_Instance = new IpcTask();
    }

    ASSERTION(S_Instance != NULL);

    return S_Instance;
}

void IpcTask::Init (void)
{
    //Initialize USBVcom hardware
//    USB_init();

    //Initialize ipc Queue
    IPCQueue = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(Message));

}

void IpcTask::IPCCreateTask (void)
{
    //Create IPC task
    xTaskCreate(Entry, "IPCTask", IPC_TASK_SIZE,
                NULL, TASK_PRIO_IPC_TASK, (TaskHandle_t *) NULL);
}

bool IpcTask::SendToIPCQueue (const void* pvItemToQueue,
                              TickType_t xTicksToWait)
{

}

void IpcTask::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    volatile UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Board_UARTDebug("StackSize ENTER IPC: %d \n", uxHighWaterMark);

    while(FOREVER)
    {
        DEBUG_IPC("IPC Task\n");


        vTaskDelay(IPC_TASK_DELAY_TIME);  //delay 20 ms
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
        DEBUG_IPC("StackSize REMAIN IPC: (%d)  USE (%d) \n", uxHighWaterMark, (IPC_TASK_SIZE - uxHighWaterMark)*4);

    } // while (FOREVER) ....

}

IpcTask::IpcTask (void)
{

}

uint8_t* IpcTask::ConvertDataToSend (Message* eMessage)
{

}

Message* IpcTask::ReConvertData (uint8_t* pBuf, uint32_t Size)
{

}
