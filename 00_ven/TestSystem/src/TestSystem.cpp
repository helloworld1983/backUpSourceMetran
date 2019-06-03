#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "FreeRTOS.h"
#include "task.h"

#include "HFOTask.h"
#include "AIRMotorTask.h"
#include "ProxyFlowTask.h"
#include "ProxyFlowSensor.h"
#include "TestFlowSensorTask.h"
#include "TestFlowSenI2C1.h"
#include "TestAdcPressureTask.h"
#include "TestIPCTask.h"
#include "TestSolenoidValve.h"

#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0APP) | defined (__MULTICORE_MASTER_SLAVE_M0SUB)
#include "cr_start_m0.h"
#endif

static void prvSetupHardware(void)
{
    HFOTask::S_GetInstance()->Init();
    AirTask::S_GetInstance()->Init();
    ProxyFlowTask::S_GetInstance()->Init();
    TestFlowSensorTask::S_GetInstance()->Init();
    TestFlowSenI2C1::S_GetInstance()->Init();
    TestAdcPressureTask::S_GetInstance()->Init();
    TestIPCTask::S_GetInstance()->Init();
    TestSolenoidValve::S_GetInstance()->Init();

}


static void main_tasks (void)
{
    const int FOREVER = 1;
    HFOTask::S_GetInstance()->HFOCreateTask();
    AirTask::S_GetInstance()->AirCreateTask();
    ProxyFlowTask::S_GetInstance()->ProxyFlowCreateTask();
    TestFlowSensorTask::S_GetInstance()->TestFlowSensorCreateTask();
    TestFlowSenI2C1::S_GetInstance()->TestFlowSenI2C1CreateTask();
    TestAdcPressureTask::S_GetInstance()->TestAdcPressureCreateTask();
    TestIPCTask::S_GetInstance()->TestIPCCreateTask();
    TestSolenoidValve::S_GetInstance()->SolenoidValveCreateTask();

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
#endif
#endif

    //Initialize hardware
    prvSetupHardware();

    Board_UARTDebug("Main Program\n");
    main_tasks ();


    return 0 ;
}
