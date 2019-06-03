#include "HFOTask.h"
#include "HfoServo.h"

#define HFO_UART_CHANEL              LPC_USART2
#define UART_SRB_SIZE               128 /* Send */
#define UART_RRB_SIZE               32  /* Receive */
static RINGBUFF_T hfo_txringFc, hfo_rxringFc;
static uint8_t hfo_rxbuff[UART_RRB_SIZE], hfo_txbuff[UART_SRB_SIZE];

#define HFO_TARGET_ADDRESS  0x01
#define EXH_VALVE_TARGET_ADDRESS  0x02
#define INSTRUCTION_NUMBER  0x04
#define TYPE  0x00
#define MOTOR_BANK  0x00


extern "C" void UART2_IRQHandler(void)
{
    /* Want to handle any errors? Do it here. */
    /* Use default ring buffer handler. Override this with your own
       code if you need more capability. */
    Chip_UART_IRQRBHandler(HFO_UART_CHANEL, &hfo_rxringFc, &hfo_txringFc);
}


HFOTask *HFOTask::S_Instance = NULL;

HFOTask* HFOTask::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new HFOTask();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void HFOTask::Init (void)
{
    Board_UART_Init(HFO_UART_CHANEL);

    /* Setup UART for 115.2K8N1 */
    Chip_UART_DeInit(HFO_UART_CHANEL);
    Chip_UART_Init(HFO_UART_CHANEL);
    Chip_UART_SetBaud(HFO_UART_CHANEL, 230400);
    Chip_UART_ConfigData(HFO_UART_CHANEL, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT);
    Chip_UART_SetupFIFOS(HFO_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(HFO_UART_CHANEL);

    /* Before using the ring buffers, initialize them using the ring
                       buffer init function */
    RingBuffer_Init(&hfo_rxringFc, hfo_rxbuff, 1, UART_RRB_SIZE);
    RingBuffer_Init(&hfo_txringFc, hfo_txbuff, 1, UART_SRB_SIZE);

    /* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
    Chip_UART_SetupFIFOS(HFO_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
            UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

    /* Enable receive data and line status interrupt */
    Chip_UART_IntEnable(HFO_UART_CHANEL, (UART_IER_RBRINT | UART_IER_RLSINT));

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(USART2_IRQn, 2);
    NVIC_EnableIRQ(USART2_IRQn);

    Chip_SCU_PinMuxSet(0xB, 0, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 20);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 20,true);

}

void HFOTask::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    while(FOREVER)
    {
        uint8_t cmd[9];
        cmd[0] = 0x01;
        cmd[1] = INSTRUCTION_NUMBER;
        cmd[2] = TYPE;
        cmd[3] = MOTOR_BANK;

        int iFrequence = 17;

        int32_t ReturnValue = HfoServo::S_GetInstance()->CreateSinPosition(iFrequence);
        ReturnValue = ReturnValue/9;


        cmd[7] = (uint8_t)ReturnValue;
        cmd[6] = (uint8_t)(ReturnValue >> 8);
        cmd[5] = (uint8_t)(ReturnValue >> 16);
        cmd[4] = (uint8_t)(ReturnValue >> 24);

        cmd[8] = HFOTask::S_Instance->CalCheckSum(cmd);

        //initialize time delay, it is calculated by DELAY = 1/baud rate
        //DELAY = Time delay need to transfer 1 byte.
        const uint8_t DELAY = 50;   //Us
        //Set pin hight to send data
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 20,true);

        vTaskSuspendAll();
        Chip_UART_SendRB(HFO_UART_CHANEL, &hfo_txringFc, cmd, 9);
        xTaskResumeAll ();

        //Delay time to send data
        Board_DelayUs(DELAY * 8);

        //set pin low to receive data
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 20,false);

        //calculate delay time >>> IF 2ms - 2ms: can not run 2 motor
        vTaskDelay(2/portTICK_PERIOD_MS);
    }
}

void HFOTask::HFOCreateTask (void)
{
    xTaskCreate(Entry, "HFOTask", 5*configMINIMAL_STACK_SIZE,
                NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);
}

HFOTask::HFOTask (void)
{
}

uint8_t HFOTask::CalCheckSum (uint8_t Cmd[8])
{
    uint16_t uTempCheckSum = 0;
    for(int i = 0; i < 8; ++i)
    {
        uTempCheckSum += Cmd[i];
    }
    return (uint8_t)uTempCheckSum;
}
