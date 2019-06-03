#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#include "FreeRTOS.h"
#include "task.h"

#include "TestSolenoidValve.h"
#include "TestAdcPressureTask.h"
#include "TestFlowSensorTask.h"
#include "ProxyFlowTask.h"
#include "TestMotorSPI.h"
#include "TestUSB.h"



static void prvSetupHardware(void)
{
    TestSolenoidValve::S_GetInstance()->Init();
    TestAdcPressureTask::S_GetInstance()->Init();
    TestFlowSensorTask::S_GetInstance()->Init();
    ProxyFlowTask::S_GetInstance()->Init();
    TestMotorSPI::S_GetInstance()->Init();
    TestUSB::S_GetInstance()->Init();
}

static void main_tasks (void)
{
    const int FOREVER = 1;
    TestSolenoidValve::S_GetInstance()->SolenoidValveCreateTask();
    TestAdcPressureTask::S_GetInstance()->TestAdcPressureCreateTask();
    TestFlowSensorTask::S_GetInstance()->TestFlowSensorCreateTask();
    ProxyFlowTask::S_GetInstance()->ProxyFlowCreateTask();
    TestMotorSPI::S_GetInstance()->TestMotorSPICreateTask();
//    ProxyFlowTask::S_GetInstance()->ProxyFlowReadTask();
    TestUSB::S_GetInstance()->TestUSBCreateTask();
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
