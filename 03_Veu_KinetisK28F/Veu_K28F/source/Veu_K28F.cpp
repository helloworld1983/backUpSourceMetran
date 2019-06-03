/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    Veu_K28F.cpp
 * @brief   Application entry point.
 */


//Utils includes
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK28F15.h"
#include "fsl_debug_console.h"
#include "DebugTrace.h"

//freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

//Alarm includes
#include "AlarmTask.h"

//PD includes
#include "PdTask.h"

//ipc includes
#include "IpcTask.h"

//bd includes
#include "BdTask.h"

//Cbit includes
#include "CBitTask.h"

/****************************************************************************************/
//
// Function name:   prvSetupHardware
//
// Details:  Init all neccesary hardware for each Task
//
// Arguments:   None
//
// ReturnValue : void
//
/****************************************************************************************/
static void prvSetupHardware (void)
{
    AlarmTask::S_GetInstance ()->Init();
    IpcTask::S_GetInstance()->Init();
    BdTask::S_GetInstance()->Init();
}

/****************************************************************************************/
//
// Function name:   main_tasks
//
// Details:  Create and run different tasks for controlling Portable system
//
// Arguments:   None
//
// ReturnValue : void
//
/****************************************************************************************/
static void main_tasks (void)
{
    const int FOREVER = 1;

    AlarmTask::S_GetInstance ()->AlarmCreateTask ();
    PdTask::S_GetInstance ()->PDCreateTask ();
    IpcTask::S_GetInstance()->IPCCreateTask();
    BdTask::S_GetInstance()->BDCreateTask();
    CbitTask::S_GetInstance()->CBitCreateTask();

    vTaskStartScheduler ();
    //Control should never come here
    DEBUG_MAIN("Schedule Failure\r\n");

    while (FOREVER)
    {
    }
}

int main(void) {

  	// Init board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	// Init FSL debug console.
    BOARD_InitDebugConsole();

    //initialize all task
    prvSetupHardware ();

    //go to boot loader

    DEBUG_MAIN("System Start %d \n", SystemCoreClock);
    //run all task
    main_tasks ();

    return 0;
}
