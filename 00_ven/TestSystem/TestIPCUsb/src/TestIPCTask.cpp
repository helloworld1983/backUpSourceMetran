#include "TestIPCTask.h"

#include "UsbVcom.h"
#include "cdc_vcom.h"

//#define TIME_QUEUE_AVAILABLE        (( TickType_t )0)

TestIPCTask *TestIPCTask::S_Instance = NULL;

TestIPCTask* TestIPCTask::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new TestIPCTask();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void TestIPCTask::Init (void)
{

//    Chip_SCU_PinMuxSet(0xC, 5, (SCU_MODE_FUNC4));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 6, 4);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,6, 4,true);

    USB_init();

//    //initialize queue to send and receive data from IMX
//    qTrasferDataToIMX = xQueueCreate(QUEUE_LENGTH_MESSAGE,sizeof(struct Message));
//    qReceiveFromIMX = xQueueCreate(QUEUE_LENGTH_MESSAGE,sizeof(struct Message));
//
//    //check queue is create successfully
//    ASSERTION(qTrasferDataToIMX != NULL);
//    ASSERTION(qReceiveFromIMX != NULL);
}

void TestIPCTask::TestIPCCreateTask (void)
{
    xTaskCreate(Entry, "TestIPCTask", 5*configMINIMAL_STACK_SIZE,NULL,
                (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);
}
#define MAX_BUFFER (32)
void TestIPCTask::Entry (void* pvParameters)
{
    uint8_t MessageOK[MAX_BUFFER];
    uint8_t Temp_Buff[MAX_BUFFER];

    for(int i = 0; i < MAX_BUFFER; ++i)
    {
       MessageOK[i] = 0x1A;
    }


    const int FOREVER = 1;
    while(FOREVER)
    {
        bool EstablishFlag = false;
         bool ConnectedFlag = false;
         int count = 0;
         for(int i = 0; i < MAX_BUFFER; ++i){
            MessageOK[i] = 0x1A;
         }


         while (1) {
            DEBUG_IPC("USB Task %d \n", vcom_connected());
            /* Check if host has connected and opened the VCOM port */
            if ((vcom_connected() != 0) && (vcom_connected() != 257) && (!EstablishFlag)) {

               DEBUG_IPC("Establish connection \n");
               EstablishFlag = true;
            }else{
               DEBUG_IPC("Can not establish connection\n");
               EstablishFlag = false;
            }

            if(EstablishFlag){
               DEBUG_IPC("Established connection \n");

               if(vcom_bread(Temp_Buff, MAX_BUFFER) != 0)
               {
                   DEBUG_IPC("data = ");
                   for(int i = 0; i < MAX_BUFFER; ++i){
                       DEBUG_IPC("%x ", Temp_Buff[i]);
                       Temp_Buff[i] = 0;
                   }
                   DEBUG_IPC("\n");
               }


               count = MAX_BUFFER;

               if(MAX_BUFFER == count){
                  ConnectedFlag = true;
                  vcom_write(MessageOK, MAX_BUFFER);  //send message OK
                  count = 0;
               }else{
                  ConnectedFlag = false;
                  count = 0;
               }



      //         if(ConnectedFlag){
      //
      //
      //         }else{
      //            DEBUG_IPC("failed to connected\n");
      //
      //         }




































            }


            DEBUG_IPC("\n");
            vTaskDelay(20/portTICK_PERIOD_MS);
         }

    }
}

TestIPCTask::TestIPCTask (void)
{
}

//bool TestIPCTask::SendData (Message* eMessage)
//{
//    //initialize status of sending data
//    bool bStatus = false;
//
//    //check eMessage is NULL or not
//    ASSERTION(eMessage != NULL);
//
//    //send data if sending queue is not NULL
//    if(NULL != qTrasferDataToIMX)
//    {
//        if(xQueueSend(qTrasferDataToIMX, &eMessage, TIME_QUEUE_AVAILABLE))
//        {
//            bStatus = true;
//
//        }
//        else
//        {
//            bStatus = false;
//        }
//    }
//    else
//    {
//        bStatus = false;
//    }
//    return bStatus;
//
//}
//
//bool TestIPCTask::GetData (Message* eMessage)
//{
//    //initialize status of sending data
//    bool bStatus = false;
//
//    //check eMessage is NULL or not
//    ASSERTION(eMessage != NULL);
//
//    //Receive Data if receiving queue is not NULL
//    if(NULL != qReceiveFromIMX)
//    {
//        if(xQueueReceive(qReceiveFromIMX, &eMessage, TIME_QUEUE_AVAILABLE))
//        {
//            DEBUG_IPC("Rec data from IMX \n");
//            DEBUG_IPC(" MessID = %d , isRes = %d, data  = %x %x \n", eMessage->MessageID, eMessage->IsResponse,
//                      eMessage->CommandID, eMessage->Length);
//
//            for(int i = 0; i < eMessage->Length; ++i)
//            {
//                DEBUG_IPC("%x ", eMessage->Data[i]);
//            }
//            bStatus = true;
//
//        }
//        else
//        {
//            bStatus = false;
//        }
//
//    }
//    else
//    {
//        bStatus = false;
//    }
//
//    return bStatus;
//}
