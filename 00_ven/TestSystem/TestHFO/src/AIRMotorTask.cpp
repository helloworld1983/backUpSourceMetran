#include "AIRMotorTask.h"

#include "HfoServo.h"

#define AIR_UART_CHANEL              LPC_USART0
#define UART_SRB_SIZE               128 /* Send */
#define UART_RRB_SIZE               32  /* Receive */

static RINGBUFF_T air_txringFc, air_rxringFc;
static uint8_t air_rxbuff[UART_RRB_SIZE], air_txbuff[UART_SRB_SIZE];

#define HFO_TARGET_ADDRESS  0x01
#define EXH_VALVE_TARGET_ADDRESS  0x02
#define INSTRUCTION_NUMBER  0x04
#define TYPE  0x00
#define MOTOR_BANK  0x00

AirTask *AirTask::S_Instance = NULL;

AirTask* AirTask::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new AirTask();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void AirTask::Init (void)
{
    Board_UART_Init(AIR_UART_CHANEL);

    /* Setup UART for 115.2K8N1 */
    Chip_UART_DeInit(AIR_UART_CHANEL);
    Chip_UART_Init(AIR_UART_CHANEL);
    Chip_UART_SetBaud(AIR_UART_CHANEL, 230400);
    Chip_UART_ConfigData(AIR_UART_CHANEL, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT);
    Chip_UART_SetupFIFOS(AIR_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(AIR_UART_CHANEL);

    /* Before using the ring buffers, initialize them using the ring
                           buffer init function */
    RingBuffer_Init(&air_rxringFc, air_rxbuff, 1, UART_RRB_SIZE);
    RingBuffer_Init(&air_txringFc, air_txbuff, 1, UART_SRB_SIZE);

    /* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
    Chip_UART_SetupFIFOS(AIR_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
            UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

    /* Enable receive data and line status interrupt */
    Chip_UART_IntEnable(AIR_UART_CHANEL, (UART_IER_RBRINT | UART_IER_RLSINT));

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(USART0_IRQn, 2);
    NVIC_EnableIRQ(USART0_IRQn);

    Chip_SCU_PinMuxSet(0x6, 6, (SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 5);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,true);
}

extern "C" void UART0_IRQHandler(void)
{
    /* Want to handle any errors? Do it here. */
    /* Use default ring buffer handler. Override this with your own
       code if you need more capability. */
    Chip_UART_IRQRBHandler(AIR_UART_CHANEL, &air_rxringFc, &air_txringFc);
}

void AirTask::AirCreateTask (void)
{
    xTaskCreate(Entry, "AirTask", 5*configMINIMAL_STACK_SIZE,NULL,
                (tskIDLE_PRIORITY + 4UL), (TaskHandle_t *) NULL);
}

void AirTask::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    static bool status = true;

    while(FOREVER)
    {
        //send to motor 1
        uint8_t cmd[9];
        cmd[0] = 0x02;
        cmd[1] = INSTRUCTION_NUMBER;
        cmd[2] = TYPE;
        cmd[3] = MOTOR_BANK;

        int iFrequence = 17;

        int32_t ReturnValue = HfoServo::S_GetInstance()->CreateSinPositionForSmallMotor(iFrequence);
        ReturnValue = ReturnValue/10;
//        int32_t ReturnValue;
//        status = !status;
//        if(status)
//        {
//            ReturnValue = 2000;
//        }
//        else
//        {
//            ReturnValue = 0;
//        }

        cmd[7] = (uint8_t)ReturnValue;
        cmd[6] = (uint8_t)(ReturnValue >> 8);
        cmd[5] = (uint8_t)(ReturnValue >> 16);
        cmd[4] = (uint8_t)(ReturnValue >> 24);

        cmd[8] = AirTask::S_Instance->CalCheckSum(cmd);

        //initialize time delay, it is calculated by DELAY = 1/baud rate
        //DELAY = Time delay need to transfer 1 byte.
        const uint8_t DELAY = 50;   //Us
        //Set pin hight to send data
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,true);

        vTaskSuspendAll();
        Chip_UART_SendRB(AIR_UART_CHANEL, &air_txringFc, cmd, 9);
        xTaskResumeAll();

        //Delay time to send data
        Board_DelayUs(DELAY * 8);

        //set pin low to receive data
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,false);

        //Send to motor 2
        cmd[0] = 0x03;

        cmd[8] = AirTask::S_Instance->CalCheckSum(cmd);

        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,true);

        vTaskSuspendAll();
        Chip_UART_SendRB(AIR_UART_CHANEL, &air_txringFc, cmd, 9);
        xTaskResumeAll();

        //Delay time to send data
        Board_DelayUs(DELAY * 8);

        //set pin low to receive data
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,false);

        //send to motor 3
        cmd[0] = 0x04;

        cmd[8] = AirTask::S_Instance->CalCheckSum(cmd);

        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,true);

        vTaskSuspendAll();
        Chip_UART_SendRB(AIR_UART_CHANEL, &air_txringFc, cmd, 9);
        xTaskResumeAll();

        //Delay time to send data
        Board_DelayUs(DELAY * 8);

        //set pin low to receive data
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,false);



//        Board_UARTDebug("\tAIR[%d]\n",ReturnValue);
        vTaskDelay(5/portTICK_PERIOD_MS);

    }
}

AirTask::AirTask (void)
{
}

uint8_t AirTask::CalCheckSum (uint8_t Cmd[8])
{
    uint16_t uTempCheckSum = 0;
    for(int i = 0; i < 8; ++i)
    {
        uTempCheckSum += Cmd[i];
    }
    return (uint8_t)uTempCheckSum;
}
