/*
===============================================================================

 Name        : usbdevice.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#include "MassStorage.h"


USB_ClassInfo_MS_Device_t Disk_MS_Interface =
    {
        .Config =
            {
                .InterfaceNumber           = 0,

                .DataINEndpointNumber      = MASS_STORAGE_IN_EPNUM,
                .DataINEndpointSize        = MASS_STORAGE_IO_EPSIZE,
                .DataINEndpointDoubleBank  = false,

                .DataOUTEndpointNumber     = MASS_STORAGE_OUT_EPNUM,
                .DataOUTEndpointSize       = MASS_STORAGE_IO_EPSIZE,
                .DataOUTEndpointDoubleBank = false,

                .TotalLUNs                 = TOTAL_LUNS,
            },
    };


/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    ConfigSuccess &= MS_Device_ConfigureEndpoints(&Disk_MS_Interface);

//  LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    MS_Device_ProcessControlRequest(&Disk_MS_Interface);
}

/** Mass Storage class driver callback function the reception of SCSI commands from the host, which must be processed.
 *
 *  \param[in] MSInterfaceInfo  Pointer to the Mass Storage class interface configuration structure being referenced
 */
bool CALLBACK_MS_Device_SCSICommandReceived(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo)
{
    bool CommandSuccess;

    LEDs_SetAllLEDs(LEDMASK_USB_BUSY);
    CommandSuccess = SCSI_DecodeSCSICommand(MSInterfaceInfo);
    LEDs_SetAllLEDs(LEDMASK_USB_READY);

    return CommandSuccess;
}

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

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

    Chip_USB0_Init();
    Chip_USB1_Init();



    USB_Init(USB_MODE_Device);
    NVIC_SetPriority(USB1_IRQn, 1);//Q_add
    NVIC_SetPriority(USB0_IRQn, 1);//Q_add


    return 0 ;
}
