//*****************************************************************************
//*****************************************************************************
//  FILENAME: LED_9.h
//   Version: 1.40, Updated on 2011/9/2 at 9:40:1                                          
//  Generated by PSoC Designer 5.1.2309
//
//  DESCRIPTION: LED_9 User Module C Language interface file
//-----------------------------------------------------------------------------
//  Copyright (c) Cypress Semiconductor 2011. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************
#ifndef LED_9_INCLUDE
#define LED_9_INCLUDE

#include <m8c.h>


/* Create pragmas to support proper argument and return value passing */
#pragma fastcall16  LED_9_Stop
#pragma fastcall16  LED_9_Start
#pragma fastcall16  LED_9_On
#pragma fastcall16  LED_9_Off
#pragma fastcall16  LED_9_Switch
#pragma fastcall16  LED_9_Invert
#pragma fastcall16  LED_9_GetState


//-------------------------------------------------
// Constants for LED_9 API's.
//-------------------------------------------------
//
#define  LED_9_ON   1
#define  LED_9_OFF  0

//-------------------------------------------------
// Prototypes of the LED_9 API.
//-------------------------------------------------
extern void  LED_9_Start(void);                                     
extern void  LED_9_Stop(void);                                      
extern void  LED_9_On(void);                                      
extern void  LED_9_Off(void);                                      
extern void  LED_9_Switch(BYTE bSwitch);
extern void  LED_9_Invert(void);                                         
extern BYTE  LED_9_GetState(void);                                         

//-------------------------------------------------
// Define global variables.                 
//-------------------------------------------------



#endif
