;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME:   LED_1.asm
;;  Version: 1.40, Updated on 2011/9/2 at 9:40:1
;;  Generated by PSoC Designer 5.1.2309
;;
;;  DESCRIPTION: LED user module.
;;
;;
;;  NOTE: User Module APIs conform to the fastcall16 convention for marshalling
;;        arguments and observe the associated "Registers are volatile" policy.
;;        This means it is the caller's responsibility to preserve any values
;;        in the X and A registers that are still needed after the API functions
;;        returns. For Large Memory Model devices it is also the caller's 
;;        responsibility to perserve any value in the CUR_PP, IDX_PP, MVR_PP and 
;;        MVW_PP registers. Even though some of these registers may not be modified
;;        now, there is no guarantee that will remain the case in future releases.
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress Semiconductor 2011. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************

include "LED_1.inc"
include "memory.inc"

export _LED_1_Start
export  LED_1_Start

export _LED_1_Stop
export  LED_1_Stop

export _LED_1_On
export  LED_1_On

export _LED_1_Off
export  LED_1_Off

export _LED_1_Switch
export  LED_1_Switch

export _LED_1_Invert
export  LED_1_Invert

export _LED_1_GetState
export  LED_1_GetState


AREA UserModules (ROM, REL)


.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: LED_1_Start(void)
;  FUNCTION NAME: LED_1_Stop(void)
;
;  FUNCTION NAME: LED_1_Switch(void)
;
;  DESCRIPTION: ( Switch )
;     Turn LED on or off     
;
;  DESCRIPTION: ( Start, Stop )
;     Turn LED off                       
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:  ( Switch )
;     A => If 0, turn off LED, if > 0 turn on LED
;
;  ARGUMENTS:  ( Start, Stop )
;      None
;
;  RETURNS:  none
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;-----------------------------------------------------------------------------
_LED_1_On:
 LED_1_On:
   mov  A,0x01
   jmp  LED_1_Switch 

_LED_1_Start:
 LED_1_Start:
_LED_1_Stop:
 LED_1_Stop:
_LED_1_Off:
 LED_1_Off:
   mov  A,0x00

_LED_1_Switch:
 LED_1_Switch:
   RAM_PROLOGUE RAM_USE_CLASS_4
   RAM_PROLOGUE RAM_USE_CLASS_2
   RAM_SETPAGE_CUR >Port_2_Data_SHADE

   or   A,0x00                                   ; Check mode
   jz   .Turn_Off_LED

.Turn_On_LED:
IF(1)                                            ; Active High Digit Drive
   or   [Port_2_Data_SHADE],LED_1_PinMask
ELSE                                             ; Active Low Digit Drive
   and  [Port_2_Data_SHADE],~LED_1_PinMask
ENDIF
   jmp  .Switch_LED

.Turn_Off_LED:
IF(1)                      ; Active High Digit Drive
   and  [Port_2_Data_SHADE],~LED_1_PinMask
ELSE                              ; Active Low Digit Drive
   or   [Port_2_Data_SHADE],LED_1_PinMask
ENDIF

.Switch_LED:
   mov  A,[Port_2_Data_SHADE]
   mov  reg[LED_1_PortDR],A

   RAM_EPILOGUE RAM_USE_CLASS_2
   RAM_EPILOGUE RAM_USE_CLASS_4
   ret
.ENDSECTION



.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: LED_1_Invert(void)
;
;  DESCRIPTION:
;     Invert state of LED                               
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS:  none
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;-----------------------------------------------------------------------------
_LED_1_Invert:
 LED_1_Invert:
   RAM_PROLOGUE RAM_USE_CLASS_4
   RAM_PROLOGUE RAM_USE_CLASS_2
   RAM_SETPAGE_CUR >Port_2_Data_SHADE

   xor  [Port_2_Data_SHADE],LED_1_PinMask
   mov  A,[Port_2_Data_SHADE]
   mov  reg[LED_1_PortDR],A

   RAM_EPILOGUE RAM_USE_CLASS_2
   RAM_EPILOGUE RAM_USE_CLASS_4
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: LED_1_GetState(void)
;
;  DESCRIPTION:
;     Get state of LED
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS:  
;    State of LED   1 = ON,  0 = OFF
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;-----------------------------------------------------------------------------
_LED_1_GetState:
 LED_1_GetState:
   RAM_PROLOGUE RAM_USE_CLASS_4
   RAM_PROLOGUE RAM_USE_CLASS_2
   RAM_SETPAGE_CUR >Port_2_Data_SHADE

   mov   A,[Port_2_Data_SHADE]         ; Get shade value
IF(1)                                  ; Active High Digit Drive
   // Nothing for now
ELSE                                   ; Active Low Digit Drive
   cpl   A                             ; Invert bit if Active low
ENDIF
   and   A,LED_1_PinMask               ; Mask off the trash
   jz    .End_LED_GS                   ; If zero, we're done
   mov   A,0x01                        ; Return a 1 no mater what the mask is.

.End_LED_GS:
   RAM_EPILOGUE RAM_USE_CLASS_2
   RAM_EPILOGUE RAM_USE_CLASS_4
   ret
.ENDSECTION
