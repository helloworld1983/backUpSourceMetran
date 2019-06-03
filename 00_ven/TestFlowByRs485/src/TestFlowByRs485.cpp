#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0APP) | defined (__MULTICORE_MASTER_SLAVE_M0SUB)
#include "cr_start_m0.h"
#endif

#include "FreeRTOS.h"
#include "task.h"

#include "ProxyFlowSensor.h"

//#include "uart3.h"

#define FC_UART_CHANEL              LPC_USART3
#define UART_SRB_SIZE               128 /* Send */
#define UART_RRB_SIZE               32  /* Receive */
static RINGBUFF_T gs_txringFc, gs_rxringFc;
static uint8_t gs_rxbuff[UART_RRB_SIZE], gs_txbuff[UART_SRB_SIZE];


void UART_init()
{
    Board_UART_Init(FC_UART_CHANEL);

    /* Setup UART for 115.2K8N1 */
    Chip_UART_DeInit(FC_UART_CHANEL);
    Chip_UART_Init(FC_UART_CHANEL);
    Chip_UART_SetBaud(FC_UART_CHANEL, 115200);
    Chip_UART_ConfigData(FC_UART_CHANEL, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT);
    Chip_UART_SetupFIFOS(FC_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(FC_UART_CHANEL);

    /* Before using the ring buffers, initialize them using the ring
                       buffer init function */
    RingBuffer_Init(&gs_rxringFc, gs_rxbuff, 1, UART_RRB_SIZE);
    RingBuffer_Init(&gs_txringFc, gs_txbuff, 1, UART_SRB_SIZE);

    /* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
    Chip_UART_SetupFIFOS(FC_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
            UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

    /* Enable receive data and line status interrupt */
    Chip_UART_IntEnable(FC_UART_CHANEL, (UART_IER_RBRINT | UART_IER_RLSINT));

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(USART3_IRQn, 3);
    NVIC_EnableIRQ(USART3_IRQn);

    //set this pin high before send to RS485

    Chip_SCU_PinMuxSet(0x9, 2, (SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 4, 14);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,4, 14,false);


}



extern "C" void UART3_IRQHandler(void)
{
    /* Want to handle any errors? Do it here. */
    /* Use default ring buffer handler. Override this with your own
       code if you need more capability. */
    Chip_UART_IRQRBHandler(FC_UART_CHANEL, &gs_rxringFc, &gs_txringFc);
}

static void Commend(void*p)
{

    //DO NOT PUT ANY DEBUG IN WHILE ---> DELAY EXECUTION TIME---> FREQUENCY IS NOT GUARANTE
    while(1)
    {
        const uint BYTE_TO_SEND = 7;

//        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 20,true);

        uint8_t cmd[7] = {0x7E, 0x00, 0xD0, 0x01, 0x01, 0x2D, 0x7e}; //Get device name
//        uint8_t cmd[BYTE_TO_SEND] = {0x7E, 0x00, 0x33, 0x02, 0x00, 0xFA, 0xD0, 0x7E}; //Get single measurement

//        uint8_t cmd[7] = {0x31, 0x30, 0x31, 0x30, 0x31, 0x30, 13};

        Board_UARTDebug("send data \n");
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,4, 14,true);
        vTaskSuspendAll();
//        ProxyFlowSensor::S_GetInstance()->SendDataToUART(cmd, BYTE_TO_SEND);
        Chip_UART_SendRB(FC_UART_CHANEL, &gs_txringFc, cmd, BYTE_TO_SEND);

//        Board_UARTDebug("Send %x \n", num);

        xTaskResumeAll ();
//        vTaskDelay(1/(portTICK_PERIOD_MS));
        //Board_DelayUs(700); //delay when get name device
        Board_DelayUs(87*BYTE_TO_SEND); //delay when get name device
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,4, 14,false);

//        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 20,false);



//        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        //calculate delay time >>> IF 2ms - 2ms: can not run 2 motor
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}

static void Read(void*p)
{
    static bool status = true;

    while(1)
    {
//        Board_UARTDebug("ReadData:\n");

//        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 20,false);

        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 23,status);
        status = !status;


        uint8_t cmd[32];
//        int num = ProxyFlowSensor::S_GetInstance()->ReadDataFromUART(cmd, 32);

        int num = Chip_UART_ReadRB(FC_UART_CHANEL, &gs_rxringFc, cmd, 32);

        if(num != 0)
        {
            for(int i = 0; i < 32; ++i)
            {
                Board_UARTDebug("%x ",cmd[i]);
            }
            Board_UARTDebug("\n");
        }




        vTaskDelay(2/portTICK_PERIOD_MS);

    }
}

void SendCommendToFlow(void)
{
    xTaskCreate(Commend, "SendCommendToFlow", 5*configMINIMAL_STACK_SIZE,NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);

}
void ReadData(void)
{
    xTaskCreate(Read, "ReadData", 5*configMINIMAL_STACK_SIZE,NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);

}


static void main_tasks (void)
{
    const int FOREVER = 1;
    SendCommendToFlow();
    ReadData();
    vTaskStartScheduler ();
    //Control should never come here
    while (FOREVER)
    {
    }
}


int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
#if defined (__MULTICORE_MASTER) || defined (__MULTICORE_NONE)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();

#endif
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    // Start M0APP slave processor
#if defined (__MULTICORE_MASTER_SLAVE_M0APP)
    cr_start_m0(SLAVE_M0APP,&__core_m0app_START__);
#endif

    // Start M0SUB slave processor
#if defined (__MULTICORE_MASTER_SLAVE_M0SUB)
    cr_start_m0(SLAVE_M0SUB,&__core_m0sub_START__);
#endif

    //init 2 pin to debug
    Chip_GPIO_Init(LPC_GPIO_PORT);

    Chip_SCU_PinMuxSet(0xF, 8, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 22);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 22,true);


    Chip_SCU_PinMuxSet(0xF, 9, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 23);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 23,true);

    //init uart
    UART_init();
//    ProxyFlowSensor::S_GetInstance()->Init();

    main_tasks ();
    return 0 ;
}
