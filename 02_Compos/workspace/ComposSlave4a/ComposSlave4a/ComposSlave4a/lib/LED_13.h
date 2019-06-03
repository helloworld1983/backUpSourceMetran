//*****************************************************************************
//*****************************************************************************
//  FILENAME: LED_13.h
//   Version: 1.40, Updated on 2011/6/28 at 6:9:35                                          
//  Generated by PSoC Designer 5.1.2306
//
//  DESCRIPTION: LED_13 User Module C Language interface file
//-----------------------------------------------------------------------------
//  Copyright (c) Cypress Semiconductor 2011. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************
#ifndef LED_13_INCLUDE
#define LED_13_INCLUDE

#include <m8c.h>


/* Create pragmas to support proper argument and return value passing */
#pragma fastcall16  LED_13_Stop
#pragma fastcall16  LED_13_Start
#pragma fastcall16  LED_13_On
#pragma fastcall16  LED_13_Off
#pragma fastcall16  LED_13_Switch
#pragma fastcall16  LED_13_Invert
#pragma fastcall16  LED_13_GetState


//-------------------------------------------------
// Constants for LED_13 API's.
//-------------------------------------------------
//
#define  LED_13_ON   1
#define  LED_13_OFF  0

//-------------------------------------------------
// Prototypes of the LED_13 API.
//-------------------------------------------------
extern void  LED_13_Start(void);                                     
extern void  LED_13_Stop(void);                                      
extern void  LED_13_On(void);                                      
extern void  LED_13_Off(void);                                      
extern void  LED_13_Switch(BYTE bSwitch);
extern void  LED_13_Invert(void);                                         
extern BYTE  LED_13_GetState(void);                                         

//-------------------------------------------------
// Define global variables.                 
//-------------------------------------------------



#endif
