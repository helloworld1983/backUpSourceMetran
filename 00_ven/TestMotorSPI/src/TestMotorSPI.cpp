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

//P3.3 SCK(1)   => (2) SSP0_SCK - R39. xanh la
//P3.6 MISO(1)  => (0) GPIO 0[6] - R41 tim
//P3.7 MOSI(1)  => (2) SSPO_MISO  - R40 trang
//P3.8 CS(4) - GPIO 5(11) => (2)SSPO -MOSI

#define SCK_PORT                    (0x3)
#define SCK_PIN                     (3)

#define MISO_PORT                   (0x3)
#define MISO_PIN                    (7)

#define MOSI_PORT                   (0x3)
#define MOSI_PIN                    (8)

#define CS_PORT                     (0x3)
#define CS_PIN                      (6)

#define CS_GPIO_PORT                (0)
#define CS_GPIO_PIN                 (6)

#define LPC_SSP           LPC_SSP0
#define SSP_IRQ           SSP0_IRQn

#define LPC_GPDMA_SSP_TX  GPDMA_CONN_SSP0_Tx
#define LPC_GPDMA_SSP_RX  GPDMA_CONN_SSP0_Rx

#define BUFFER_SIZE                         (0x100)
#define SSP_DATA_BITS                       (SSP_BITS_8)
#define SSP_DATA_BIT_NUM(databits)          (databits+1)
#define SSP_DATA_BYTES(databits)            (((databits) > SSP_BITS_8) ? 2:1)

#define SSP_LO_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? 0xFF:(0xFF>>(8-SSP_DATA_BIT_NUM(databits))))
#define SSP_HI_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? (0xFF>>(16-SSP_DATA_BIT_NUM(databits))):0)

#define BUFFER_SIZE                         (0x100)

static SSP_ConfigFormat ssp_format;
static Chip_SSP_DATA_SETUP_T xf_setup;

static volatile uint8_t  isXferCompleted = 0;
static uint8_t dmaChSSPTx, dmaChSSPRx;

/* Tx buffer */
static uint8_t Tx_Buf[BUFFER_SIZE];

/* Rx buffer */
static uint8_t Rx_Buf[BUFFER_SIZE];

static volatile uint8_t isDmaTxfCompleted = 0;
static volatile uint8_t isDmaRxfCompleted = 0;

static void prvSetupHardware(void)
{
    //Initialize for SPI
    //PIN SCK
    Chip_SCU_PinMuxSet(SCK_PORT, SCK_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC2));
    //PIN MISO
    Chip_SCU_PinMuxSet(MISO_PORT, MISO_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC2));
    //PIN MOSI
    Chip_SCU_PinMuxSet(MOSI_PORT, MOSI_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC2));
    //PIN CS
    Chip_SCU_PinMuxSet(CS_PORT, CS_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CS_GPIO_PORT, CS_GPIO_PIN);
    //Set PIN to LOW - default
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS_GPIO_PORT, CS_GPIO_PIN,false);


    // initialize SSP
    Chip_SSP_Init(LPC_SSP);
    Chip_SSP_SetMaster(LPC_SSP, true);
    Chip_SSP_SetBitRate(LPC_SSP, 1500000);
    Chip_SSP_SetFormat(LPC_SSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_MODE3);
    Chip_SSP_Enable(LPC_SSP);
    Chip_SSP_Int_Enable(LPC_SSP);   /* enable interrupt */


    //set up clock out put = CLKIN_MAINPLL/ 12
    Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, 12);
    Chip_Clock_SetBaseClock(CLK_BASE_OUT, CLKIN_IDIVE, true, false);



}
static void TaskMotor(void*p)
{
    Board_UARTDebug("Small Motor\n");

    uint8_t Tx_Buf[20];
    //set Amax
    Tx_Buf[0] = 0xA8;
    Tx_Buf[1] = 0x00;
    Tx_Buf[2] = 0x0F;
    Tx_Buf[3] = 0x42;
    Tx_Buf[4] = 0x40;
    //set Dmax
    Tx_Buf[5] = 0xA9;
    Tx_Buf[6] = 0x00;
    Tx_Buf[7] = 0x0F;
    Tx_Buf[8] = 0x42;
    Tx_Buf[9] = 0x40;
    //set position 1
    Tx_Buf[10] = 0xB7;
    Tx_Buf[11] = 0x00;
    Tx_Buf[12] = 0x01;
    Tx_Buf[13] = 0x86;
    Tx_Buf[14] = 0xA0;
    //set position 2
    Tx_Buf[15] = 0xB7;
    Tx_Buf[16] = 0x00;
    Tx_Buf[17] = 0x03;
    Tx_Buf[18] = 0x0D;
    Tx_Buf[19] = 0x40;

    Chip_SSP_WriteFrames_Blocking(LPC_SSP, &Tx_Buf[0], 10);


    static bool status = true;

    const int DATA = 2;

    uint8_t test[DATA];
    for(int i = 0; i < DATA; ++i)
    {
        test[i] = 0xAA;
    }


    while(1)
    {
        Board_UARTDebug("Send...\n");

        for(int i = 0; i < DATA; ++i)
        {
            Chip_SSP_WriteFrames_Blocking(LPC_SSP, &test[i], 1);
        }



        if(status)
        {
            for(int i = 10; i < 15; i++)
            {
                Chip_SSP_WriteFrames_Blocking(LPC_SSP, &Tx_Buf[i], 1);
            }
//            Chip_SSP_WriteFrames_Blocking(LPC_SSP, &Tx_Buf[10], 5);
        }
        else
        {
            for(int i = 15; i < 20; i++)
            {
                Chip_SSP_WriteFrames_Blocking(LPC_SSP, &Tx_Buf[i], 1);
            }
//            Chip_SSP_WriteFrames_Blocking(LPC_SSP, &Tx_Buf[15], 5);
        }

        status = !status;

        vTaskDelay(100/portTICK_PERIOD_MS);

    }
}


void TestTaskMotor(void)
{
    xTaskCreate(TaskMotor, "TestTaskMotor", 5*configMINIMAL_STACK_SIZE,NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);

}


static void main_tasks (void)
{
    const int FOREVER = 1;
    TestTaskMotor();
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

    Board_UARTDebug("main program\n");
    prvSetupHardware();

    main_tasks ();
    return 0 ;
}
