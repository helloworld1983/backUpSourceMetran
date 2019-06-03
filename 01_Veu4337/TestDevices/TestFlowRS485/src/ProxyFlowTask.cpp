#include "ProxyFlowTask.h"
#include "ProxyFlowSensor.h"
#include "SystemConfigure.h"
#include "Delay.h"

ProxyFlowTask *ProxyFlowTask::S_Instance = NULL;

ProxyFlowTask* ProxyFlowTask::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new ProxyFlowTask();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void ProxyFlowTask::Init (void)
{
    ProxyFlowSensor::S_GetInstance()->Init();
}

void ProxyFlowTask::ProxyFlowCreateTask (void)
{
    xTaskCreate(Entry, "ProxyFlowTask", FLOWRS485_TASK_SIZE ,NULL,
                FLOWRS485_TASK_PRIORITY, (TaskHandle_t *) NULL);
}



void GetDeviceName(void)
{
    ProxySensorSendFrameData GetDeviceName;

    GetDeviceName.Address = 0x00;
    GetDeviceName.CMD = 0xD0;
    GetDeviceName.LengTH = 0x01;
    GetDeviceName.Data[0] = 0x01;

    uint8_t DataToSen[6 + GetDeviceName.LengTH] = {};
    uint32_t SizeofData;

    ProxyFlowSensor::S_GetInstance()->GenerateFrameData(&GetDeviceName, DataToSen, SizeofData);

    for(uint32_t i = 0; i < SizeofData; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", DataToSen[i]);
    }
    DEBUG_PROXYFLOWTASK("\n");

    uint8_t Data[] = {};
    uint64_t DSize = 0;

    ProxyFlowSensor::S_GetInstance()->ConvertByteArrayToSend(DataToSen, SizeofData, Data, DSize);

    for(uint32_t i = 0; i < DSize; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", Data[i]);
    }
    DEBUG_PROXYFLOWTASK("\n");


    vTaskSuspendAll();
//    ProxyFlowSensor::S_GetInstance()->SendDataToUART(DataToSen, SizeofData);
    ProxyFlowSensor::S_GetInstance()->SendDataToUART(Data, DSize);
    xTaskResumeAll();

    uint8_t cmd[32]= {0};
    int num = ProxyFlowSensor::S_GetInstance()->ReadDataFromUART(cmd, 32);

    DEBUG_PROXYFLOWTASK("Read data(%d) \n", num);
    for(int i = 0; i < num; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", cmd[i]);
    }

    DEBUG_PROXYFLOWTASK("\n\n ");
    ProxySensorRecFrameData recData;

    if(ProxyFlowSensor::S_GetInstance()->ConvertReceiveDataToFrameData(cmd, num, &recData))
    {
        DEBUG_PROXYFLOWTASK("OK\n");
    }
    else
    {
        DEBUG_PROXYFLOWTASK("BAD\n");
    }
}

void StartContinuousMeasurement(void)
{
    int num = 0;
    uint8_t recData[32] = {0};

    ProxySensorSendFrameData StartContMeas;

    StartContMeas.Address = 0x00;
    StartContMeas.CMD = 0x33;
    StartContMeas.LengTH = 0x02;

    StartContMeas.Data[0] = 0x00;
    StartContMeas.Data[1] = 0x02;


    uint8_t StartContinuousMeasurement[6 + StartContMeas.LengTH] = {};
    uint32_t SizeofStartContinuousMeasurement = 0;

    uint8_t TempData[] = {};
    uint64_t TempSize = 0;

    ProxyFlowSensor::S_GetInstance()->GenerateFrameData(&StartContMeas, StartContinuousMeasurement, SizeofStartContinuousMeasurement);
    ProxyFlowSensor::S_GetInstance()->ConvertByteArrayToSend(StartContinuousMeasurement, SizeofStartContinuousMeasurement, TempData, TempSize);
    DEBUG_PROXYFLOWTASK("SEND = ");
    for(uint64_t i  = 0; i  < TempSize; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", TempData[i]);
    }
    DEBUG_PROXYFLOWTASK("\n");
    vTaskSuspendAll();
    ProxyFlowSensor::S_GetInstance()->SendDataToUART(TempData, TempSize);
    xTaskResumeAll();

    num = ProxyFlowSensor::S_GetInstance()->ReadDataFromUART(recData, 32);
    DEBUG_PROXYFLOWTASK("REC = ");
    for(int i  = 0; i  < num; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", recData[i]);
    }
    DEBUG_PROXYFLOWTASK("\n");
}

void GetLastMeasure(void)
{
    int num = 0;
    uint8_t recData[32] = {0};

    ProxySensorSendFrameData GetLastMeasure;

    GetLastMeasure.Address = 0x00;
    GetLastMeasure.CMD = 0x35;
    GetLastMeasure.LengTH = 0x00;



    uint8_t GetLastArr[6 + GetLastMeasure.LengTH] = {};
    uint32_t SizeofGetLastArr = 0;

    uint8_t TempData[] = {};
    uint64_t TempSize = 0;

    ProxyFlowSensor::S_GetInstance()->GenerateFrameData(&GetLastMeasure, GetLastArr, SizeofGetLastArr);
    ProxyFlowSensor::S_GetInstance()->ConvertByteArrayToSend(GetLastArr, SizeofGetLastArr, TempData, TempSize);
    DEBUG_PROXYFLOWTASK("SEND = ");
    for(uint64_t i  = 0; i  < TempSize; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", TempData[i]);
    }
    DEBUG_PROXYFLOWTASK("\n");

    vTaskSuspendAll();
    ProxyFlowSensor::S_GetInstance()->SendDataToUART(TempData, TempSize);
    xTaskResumeAll();

    num = ProxyFlowSensor::S_GetInstance()->ReadDataFromUART(recData, 32);
    DEBUG_PROXYFLOWTASK("REC = ");
    for(int i  = 0; i  < num; ++i)
    {
        DEBUG_PROXYFLOWTASK("%x ", recData[i]);
    }
    DEBUG_PROXYFLOWTASK("\n                   %d\n", (recData[5] << 8) + recData[6]);
}


void ProxyFlowTask::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    StartContinuousMeasurement();

    while(FOREVER)
    {

        DEBUG_PROXYFLOWTASK("ProxyFlowTask\n");

        GetLastMeasure();


        vTaskDelay(5/portTICK_PERIOD_MS);
    }
}

void ProxyFlowTask::ProxyFlowReadTask (void)
{
    xTaskCreate(Read, "ProxyFlowTask", 2*1024 ,NULL,
                (tskIDLE_PRIORITY + 4UL), (TaskHandle_t *) NULL);
}

void ProxyFlowTask::Read (void* P)
{
    const int FOREVER = 1;

    uint8_t cmd[32]= {0};

    while(FOREVER)
    {

        int num = ProxyFlowSensor::S_GetInstance()->ReadDataFromUART(cmd, 32);

        DEBUG_PROXYFLOWTASK("Read data(%d) \n", num);

//        ProxySensorRecFrameData recData;

//        if(ProxyFlowSensor::S_GetInstance()->ConvertReceiveDataToFrameData(cmd, num, &recData))
//        {
//            DEBUG_PROXYFLOWTASK("%x %x %x %x \n", recData.Address, recData.CMD, recData.State, recData.LengTH);
//            for(int i = 0; i < recData.LengTH; ++i)
//            {
//                DEBUG_PROXYFLOWTASK("%c ", recData.Data[i]);
//            }
//            DEBUG_PROXYFLOWTASK("\n");
//        }

        vTaskDelay(500/portTICK_PERIOD_MS);

    }
}

ProxyFlowTask::ProxyFlowTask (void)
{
}
