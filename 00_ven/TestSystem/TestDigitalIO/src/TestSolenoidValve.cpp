#include "TestSolenoidValve.h"
#include "timers.h"

#define SOLENOID_1_PORT             (0xB)             //PORT to control send data
#define SOLENOID_1_PIN              (1)               //PIN to control send data
#define SOLENOID_1_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_1_GPIO_PIN         (21)              //GPIO PIN to control send data

#define SOLENOID_2_PORT             (0xB)             //PORT to control send data
#define SOLENOID_2_PIN              (2)               //PIN to control send data
#define SOLENOID_2_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_2_GPIO_PIN         (22)              //GPIO PIN to control send data

#define SOLENOID_3_PORT             (0xB)             //PORT to control send data
#define SOLENOID_3_PIN              (3)               //PIN to control send data
#define SOLENOID_3_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_3_GPIO_PIN         (23)              //GPIO PIN to control send data

#define SOLENOID_4_PORT             (0xB)             //PORT to control send data
#define SOLENOID_4_PIN              (4)               //PIN to control send data
#define SOLENOID_4_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_4_GPIO_PIN         (24)              //GPIO PIN to control send data

#define SOLENOID_5_PORT             (0xB)             //PORT to control send data
#define SOLENOID_5_PIN              (5)               //PIN to control send data
#define SOLENOID_5_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_5_GPIO_PIN         (25)              //GPIO PIN to control send data

#define SOLENOID_6_PORT             (0xB)             //PORT to control send data
#define SOLENOID_6_PIN              (6)               //PIN to control send data
#define SOLENOID_6_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_6_GPIO_PIN         (26)              //GPIO PIN to control send data

#define RED_LED_LPC_PORT             (0x8)             //PORT to control send data
#define RED_LED_LPC_PIN              (7)               //PIN to control send data
#define RED_LED_LPC_GPIO_PORT        (4)               //GPIO PORT to control send data
#define RED_LED_LPC_GPIO_PIN         (7)              //GPIO PIN to control send data

#define ELLOW_LED_LPC_PORT             (0xC)             //PORT to control send data
#define ELLOW_LED_LPC_PIN              (9)               //PIN to control send data
#define ELLOW_LED_LPC_GPIO_PORT        (6)               //GPIO PORT to control send data
#define ELLOW_LED_LPC_GPIO_PIN         (8)              //GPIO PIN to control send data

#define BUZZER_PWM    LPC_SCT
#define BUZZER_HZ                       (2400)          //set frequency for buzzer

#define BUZZER_LPC_PORT             (0x0)             //PORT to control send data
#define BUZZER_LED_LPC_PIN              (1)               //PIN to control send data
#define BUZZER_LED_LPC_GPIO_PORT        (0)               //GPIO PORT to control send data
#define BUZZER_LED_LPC_GPIO_PIN         (1)              //GPIO PIN to control send data

#define BUZZER_2_4KHz_TICKS_TIME        (1)

#ifndef pMS_TO_TICKS
#define pMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif



TestSolenoidValve *TestSolenoidValve::S_Instance = NULL;

extern "C" void TIMER1_IRQHandler(void)
{
    static bool On = false;

    if (Chip_TIMER_MatchPending(LPC_TIMER1, 1)) {
        Chip_TIMER_ClearMatch(LPC_TIMER1, 1);
        On = (bool) !On;
//        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BUZZER_LED_LPC_GPIO_PORT, BUZZER_LED_LPC_GPIO_PIN,On);
    }
}

static TimerHandle_t BuzzerTimer2_4KHz;
static bool StatusPIN = true;

static void TimerCallback( TimerHandle_t xTimer )
{
    if (xTimerStart(BuzzerTimer2_4KHz,10) != pdPASS)
    {
        Board_UARTDebug("ERR: Failed to start 2_4 KHz Timer Buzzer \n");
    }
    StatusPIN = !StatusPIN;

    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BUZZER_LED_LPC_GPIO_PORT, BUZZER_LED_LPC_GPIO_PIN,StatusPIN);

}

static void CreateBuzzerTimer2_4KHz(void)
{

    BuzzerTimer2_4KHz = xTimerCreate
            ( /* Just a text name, not used by the RTOS
                                  kernel. */
                    "2_4 KHz Timer Buzzer",
                    /* The timer period in ticks, must be
                                  greater than 0. */
                    pMS_TO_TICKS(BUZZER_2_4KHz_TICKS_TIME),
                    /* The timers will auto-reload themselves
                                  when they expire. */
                    pdFALSE,
                    /* The ID is used to store a count of the
                                  number of times the timer has expired, which
                                  is initialised to 0. */
                    ( void * ) 0,
                    /* Each timer calls the same callback when
                                  it expires. */
                    TimerCallback
            );
}

TestSolenoidValve* TestSolenoidValve::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new TestSolenoidValve();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void TestSolenoidValve::Init (void)
{
    //initialize GPIO for Solenoid 1
    Chip_SCU_PinMuxSet(SOLENOID_1_PORT, SOLENOID_1_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SOLENOID_1_GPIO_PORT, SOLENOID_1_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_1_GPIO_PORT, SOLENOID_1_GPIO_PIN,false);

    //initialize GPIO for Solenoid 2
    Chip_SCU_PinMuxSet(SOLENOID_2_PORT, SOLENOID_2_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SOLENOID_2_GPIO_PORT, SOLENOID_2_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_2_GPIO_PORT, SOLENOID_2_GPIO_PIN,false);

    //initialize GPIO for Solenoid 3
    Chip_SCU_PinMuxSet(SOLENOID_3_PORT, SOLENOID_3_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SOLENOID_3_GPIO_PORT, SOLENOID_3_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_3_GPIO_PORT, SOLENOID_3_GPIO_PIN,false);

    //initialize GPIO for Solenoid 4
    Chip_SCU_PinMuxSet(SOLENOID_4_PORT, SOLENOID_4_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SOLENOID_4_GPIO_PORT, SOLENOID_4_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_4_GPIO_PORT, SOLENOID_4_GPIO_PIN,false);

    //initialize GPIO for Solenoid 5
    Chip_SCU_PinMuxSet(SOLENOID_5_PORT, SOLENOID_5_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SOLENOID_5_GPIO_PORT, SOLENOID_5_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_5_GPIO_PORT, SOLENOID_5_GPIO_PIN,false);

    //initialize GPIO for Solenoid 6
    Chip_SCU_PinMuxSet(SOLENOID_6_PORT, SOLENOID_6_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SOLENOID_6_GPIO_PORT, SOLENOID_6_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_6_GPIO_PIN,false);

    //initialize GPIO for RED_LED_LPC
    Chip_SCU_PinMuxSet(RED_LED_LPC_PORT, RED_LED_LPC_PIN, (SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, RED_LED_LPC_GPIO_PORT, RED_LED_LPC_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RED_LED_LPC_GPIO_PORT, RED_LED_LPC_GPIO_PIN,false);

    //initialize GPIO for RED_YELLOW_LPC_PORT
    Chip_SCU_PinMuxSet(ELLOW_LED_LPC_PORT, ELLOW_LED_LPC_PIN, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, ELLOW_LED_LPC_GPIO_PORT, ELLOW_LED_LPC_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,ELLOW_LED_LPC_GPIO_PORT, ELLOW_LED_LPC_GPIO_PIN,false);

    //initialize GPIO for RED_YELLOW_LPC_PORT
    Chip_SCU_PinMuxSet(BUZZER_LPC_PORT, BUZZER_LED_LPC_PIN, (SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, BUZZER_LED_LPC_GPIO_PORT, BUZZER_LED_LPC_GPIO_PIN);
    //Set PIN to LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BUZZER_LED_LPC_GPIO_PORT, BUZZER_LED_LPC_GPIO_PIN,false);

//    CreateBuzzerTimer2_4KHz();

//    if (xTimerStart(BuzzerTimer2_4KHz,10) != pdPASS)
//    {
//        Board_UARTDebug("ERR: Failed to start LED start 250MS timer \n");
//    }

    uint32_t timerFreq;
    /* Enable timer 1 clock and reset it */
    Chip_TIMER_Init(LPC_TIMER1);
    Chip_RGU_TriggerReset(RGU_TIMER1_RST);
    while (Chip_RGU_InReset(RGU_TIMER1_RST)) {}

    /* Get timer 1 peripheral clock rate */
    timerFreq = Chip_Clock_GetRate(CLK_MX_TIMER1);

    /* Timer setup for match and interrupt at TICKRATE_HZ */
    Chip_TIMER_Reset(LPC_TIMER1);
    Chip_TIMER_MatchEnableInt(LPC_TIMER1, 1);
    Chip_TIMER_SetMatch(LPC_TIMER1, 1, (timerFreq / 4800));
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 1);
    Chip_TIMER_Enable(LPC_TIMER1);

    /* Enable timer interrupt */
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
//    NVIC_SetPriority(TIMER1_IRQn, 0);


//    //Initialize the SCT as PWM and set frequency
//    Chip_SCTPWM_Init(BUZZER_PWM);
//    Chip_SCTPWM_SetRate(BUZZER_PWM, BUZZER_HZ);

//    // Setup Board specific output pin
//
//    // Use SCT_OUT12 pin
//    Chip_SCTPWM_SetOutPin(BUZZER_PWM, 2, 12); //CHAN KHONG HO TRO
//
//
//    // Start with 50% duty cycle
//    Chip_SCTPWM_SetDutyCycle(BUZZER_PWM, 2,
//            Chip_SCTPWM_PercentageToTicks(BUZZER_PWM, 50));
//
//    Chip_SCTPWM_Start(BUZZER_PWM);

}

void TestSolenoidValve::SolenoidValveCreateTask (void)
{
    xTaskCreate(Entry, "TestSolenoidValve", 5*configMINIMAL_STACK_SIZE,NULL,
                (tskIDLE_PRIORITY + 2UL), (TaskHandle_t *) NULL);
}

void TestSolenoidValve::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    static bool bStatus = true;

    while(FOREVER)
    {
//        Board_UARTDebug("Task GPIO\n");
        bStatus = !bStatus;

        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_1_GPIO_PIN,bStatus);
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_2_GPIO_PIN,bStatus);
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_3_GPIO_PIN,bStatus);
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_4_GPIO_PIN,bStatus);
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_5_GPIO_PIN,bStatus);
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,SOLENOID_6_GPIO_PORT, SOLENOID_6_GPIO_PIN,bStatus);

        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RED_LED_LPC_GPIO_PORT, RED_LED_LPC_GPIO_PIN,bStatus);
        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,ELLOW_LED_LPC_GPIO_PORT, ELLOW_LED_LPC_GPIO_PIN,bStatus);

        vTaskDelay(2/portTICK_PERIOD_MS);
    }


}

TestSolenoidValve::TestSolenoidValve (void)
{
}
