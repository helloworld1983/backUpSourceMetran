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

#include "HfoServo.h"

#include "FreeRTOS.h"
#include "task.h"

//#include "uart3.h"

#define HFO_UART_CHANEL              LPC_USART2
#define AIR_UART_CHANEL              LPC_USART0
#define UART_SRB_SIZE               128 /* Send */
#define UART_RRB_SIZE               32  /* Receive */
static RINGBUFF_T hfo_txringFc, hfo_rxringFc;
static uint8_t hfo_rxbuff[UART_RRB_SIZE], hfo_txbuff[UART_SRB_SIZE];

static RINGBUFF_T air_txringFc, air_rxringFc;
static uint8_t air_rxbuff[UART_RRB_SIZE], air_txbuff[UART_SRB_SIZE];

#define HFO_TARGET_ADDRESS  0x01
#define EXH_VALVE_TARGET_ADDRESS  0x02
#define INSTRUCTION_NUMBER  0x04
#define TYPE  0x00
#define MOTOR_BANK  0x00

static void UART2_init()
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
}

static void UART0_init()
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
}

extern "C" void UART0_IRQHandler(void)
{
    /* Want to handle any errors? Do it here. */
    /* Use default ring buffer handler. Override this with your own
       code if you need more capability. */
    Chip_UART_IRQRBHandler(AIR_UART_CHANEL, &air_rxringFc, &air_txringFc);
}

extern "C" void UART2_IRQHandler(void)
{
    /* Want to handle any errors? Do it here. */
    /* Use default ring buffer handler. Override this with your own
       code if you need more capability. */
    Chip_UART_IRQRBHandler(HFO_UART_CHANEL, &hfo_rxringFc, &hfo_txringFc);
}

uint8_t CalCheckSum(uint8_t Cmd[8])
{
    uint16_t uTempCheckSum = 0;
    for(int i = 0; i < 8; ++i)
    {
        uTempCheckSum += Cmd[i];
    }
    return (uint8_t)uTempCheckSum;
}

static void BigMotor(void*p)
{
    Board_UARTDebug("BigMotor\n");
    static bool status = true;

    //DO NOT PUT ANY DEBUG IN WHILE ---> DELAY EXECUTION TIME---> FREQUENCY IS NOT GUARANTE
    while(1)
    {
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 20,status);
        status = !status;

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

        cmd[8] = CalCheckSum(cmd);

        vTaskSuspendAll();
        Chip_UART_SendRB(HFO_UART_CHANEL, &hfo_txringFc, cmd, 9);
        xTaskResumeAll ();
        Board_UARTDebug("HFO[%d] \n", ReturnValue);


        //calculate delay time >>> IF 2ms - 2ms: can not run 2 motor
        vTaskDelay(2/portTICK_PERIOD_MS);
    }

}

static void SmallMotor(void*p)
{
    Board_UARTDebug("Small Motor\n");
    static bool status = true;

    while(1)
    {
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 20,status);
        status = !status;

        uint8_t cmd[9];
        cmd[0] = 0x02;
        cmd[1] = INSTRUCTION_NUMBER;
        cmd[2] = TYPE;
        cmd[3] = MOTOR_BANK;

//        int iFrequence = 17;

//        int32_t ReturnValue = HfoServo::S_GetInstance()->CreateSinPositionForSmallMotor(iFrequence);
//        ReturnValue = ReturnValue/10;
        int32_t ReturnValue;
        if(status)
        {
            ReturnValue = 1000;
        }
        else
        {
            ReturnValue = 0;
        }


        cmd[7] = (uint8_t)ReturnValue;
        cmd[6] = (uint8_t)(ReturnValue >> 8);
        cmd[5] = (uint8_t)(ReturnValue >> 16);
        cmd[4] = (uint8_t)(ReturnValue >> 24);

        cmd[8] = CalCheckSum(cmd);

        vTaskSuspendAll();
        Chip_UART_SendRB(AIR_UART_CHANEL, &air_txringFc, cmd, 9);
        xTaskResumeAll();
        Board_UARTDebug("\tAIR[%d]\n",ReturnValue);

        vTaskDelay(50/portTICK_PERIOD_MS);

    }
}

void TestTaskBigMotor(void)
{
    xTaskCreate(BigMotor, "BigMotor", 5*configMINIMAL_STACK_SIZE,NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);

}
void TestTaskSmallMotor(void)
{
    xTaskCreate(SmallMotor, "SmallMotor", 5*configMINIMAL_STACK_SIZE,NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);

}


static void main_tasks (void)
{
    const int FOREVER = 1;
    TestTaskBigMotor();
    TestTaskSmallMotor();
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

//    //init 2 pin to debug
//    Chip_GPIO_Init(LPC_GPIO_PORT);
//
//    Chip_SCU_PinMuxSet(0xF, 8, (SCU_MODE_FUNC4));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 22);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 22,true);
//
//
//    Chip_SCU_PinMuxSet(0xF, 9, (SCU_MODE_FUNC4));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 23);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 23,true);

    //init uart
    UART0_init();
    UART2_init();

    Chip_SCU_PinMuxSet(0x6, 6, (SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 5);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 5,true);

    Chip_SCU_PinMuxSet(0xB, 0, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 20);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 20,true);

    Board_UARTDebug("main program\n");

    main_tasks ();
    return 0 ;
}
