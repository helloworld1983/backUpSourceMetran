#include "TestUSB.h"
#include "SystemConfigure.h"
#include <string.h>
#include "DebugTrace.h"
#include "libusbdev.h"

/* The size of the packet buffer. */
#define PACKET_BUFFER_SIZE        4096

/* Application defined LUSB interrupt status  */
#define LUSB_DATA_PENDING       _BIT(0)

/* Packet buffer for processing */
static uint8_t g_rxBuff[PACKET_BUFFER_SIZE];


TestUSB *TestUSB::S_Instance = NULL;

TestUSB* TestUSB::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new TestUSB();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void TestUSB::Init (void)
{
    libusbdev_init(USB_STACK_MEM_BASE, USB_STACK_MEM_SIZE);
}

void TestUSB::Entry (void* pvParameters)
{
    const int FOREVER = 1;


    while(FOREVER)
    {
        DEBUG_USB("usb task\n");

//        while (libusbdev_Connected() == 0) {
//            /* Sleep until next IRQ happens */
//            __WFI();
//        }
//
//        while (libusbdev_Connected()) {
//
//            if (libusbdev_QueueReadDone() != -1) {
//
//                /* Dummy process read data ......*/
//                /* requeue read request */
//                libusbdev_QueueReadReq(g_rxBuff, PACKET_BUFFER_SIZE);
//            }
//        }







        vTaskDelay(2/portTICK_PERIOD_MS);
    }
}

TestUSB::TestUSB (void)
{
}

void TestUSB::TestUSBCreateTask (void)
{
    xTaskCreate(Entry, "TestUSB", USB_TASK_SIZE, NULL,
                USB_TASK_PRIORITY, (TaskHandle_t *) NULL);
}
