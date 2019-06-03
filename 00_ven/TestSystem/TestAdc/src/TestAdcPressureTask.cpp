#include "TestAdcPressureTask.h"

#define ADC_10BIT_MASK 1023
#define ADC_MAX_VOLTAGE 3.3
#define ADC_BIT_RATE    400000//500000
#define ADC_ID  LPC_ADC0

#define PRESSURE1   ADC_CH3
#define PRESSURE2   ADC_CH6
#define TEMP   ADC_CH2

TestAdcPressureTask *TestAdcPressureTask::S_Instance = NULL;

TestAdcPressureTask* TestAdcPressureTask::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new TestAdcPressureTask();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void TestAdcPressureTask::Init (void)
{
    ADC_CLOCK_SETUP_T ADCSetup;
    uint32_t bitRate = ADC_BIT_RATE;
    ADC_RESOLUTION_T bitAccuracy = ADC_10BITS;
    Board_ADC_Init();
    /*ADC Init */
    Chip_ADC_Init(ADC_ID, &ADCSetup);
    /* Disable Burst mode */
    Chip_ADC_SetBurstCmd(ADC_ID, DISABLE);
    /*Set sample rate */
    Chip_ADC_SetSampleRate(ADC_ID, &ADCSetup, bitRate);
    /* Set resolution */
    Chip_ADC_SetResolution(ADC_ID, &ADCSetup, bitAccuracy);
    return;
}

void TestAdcPressureTask::TestAdcPressureCreateTask (void)
{
    xTaskCreate(Entry, "TestAdcPressureTask", 5*configMINIMAL_STACK_SIZE,NULL,
                (tskIDLE_PRIORITY + 2UL), (TaskHandle_t *) NULL);
}

void TestAdcPressureTask::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    float value = 0;
    int PressValue = 0;
    int tempvalue = 0;

//    Calculate P: MPXV7025GC6U
//    Vout = VS*(0.018*P+0.5)
//    VS = 5.0 Vdc


    //calculate T: MCP9700AT-E/TT
    //Vout = Tc x Ta + Vo;
    //Tc = 10 mV/C
    //Vo = 500 mV



    while(FOREVER)
    {

        value = TestAdcPressureTask::S_GetInstance()->ReadADC(PRESSURE1);

        PressValue = 100 * (((2 * value) / 5) - 0.5) / 0.018;

        DEBUG_ADC("Presor 1(x100) = %d\n", PressValue);


        value = TestAdcPressureTask::S_GetInstance()->ReadADC(PRESSURE2);

        PressValue = 100 * (((2 * value) / 5) - 0.5) / 0.018;

        DEBUG_ADC("Presor 2(x100) = %d\n", PressValue);

        value = TestAdcPressureTask::S_GetInstance()->ReadADC(TEMP);

        tempvalue = ((value) - 0.5) / 0.01;

        DEBUG_ADC("ADC value of TEMP = %d \n", tempvalue);




        vTaskDelay(2/portTICK_PERIOD_MS);
    }

}

float TestAdcPressureTask::ReadADC (ADC_CHANNEL_T channel)
{
    float voltVal;
    uint16_t dataADC;
    TestAdcPressureTask::S_GetInstance()->Init();
    Chip_ADC_EnableChannel(ADC_ID, channel, ENABLE);
    /* Start A/D conversion if not using burst mode */
    Chip_ADC_SetStartMode(ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
    /* Waiting for A/D conversion complete */
    while (Chip_ADC_ReadStatus(ADC_ID, channel, ADC_DR_DONE_STAT) != SET) {}
    /* Read ADC value */
    Chip_ADC_ReadValue(ADC_ID, channel, &dataADC);

//    DEBUG_ADC("adc(true value = adc/1023) = %d \n", (uint16_t)(dataADC * 100 * ADC_MAX_VOLTAGE / 1023));
    // convert binary val to real voltage val
    voltVal = dataADC * ADC_MAX_VOLTAGE / ADC_10BIT_MASK;
    return voltVal;
}

TestAdcPressureTask::TestAdcPressureTask (void)
{
}
