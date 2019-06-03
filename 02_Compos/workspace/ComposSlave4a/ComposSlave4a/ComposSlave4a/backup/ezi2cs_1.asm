;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME: EzI2Cs_1.asm
;;  Version: 1.30, Updated on 2011/9/2 at 9:39:55
;;  Generated by PSoC Designer 5.1.2309
;;
;;  DESCRIPTION: EzI2Cs User Module software implementation file
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

include "m8c.inc"
include "memory.inc"
include "EzI2Cs_1.inc"
include "PSoCGPIOINT.inc"

;-----------------------------------------------
; include instance specific register definitions
;-----------------------------------------------

;-----------------------------------------------
;  Global Symbols
;-----------------------------------------------
;-------------------------------------------------------------------
;  Declare the functions global for both assembler and C compiler.
;
;  Note that there are two names for each API. First name is
;  assembler reference. Name with underscore is name refence for
;  C compiler.  Calling function in C source code does not require
;  the underscore.
;-------------------------------------------------------------------

export    EzI2Cs_1_EnableInt
export   _EzI2Cs_1_EnableInt
export    EzI2Cs_1_ResumeInt
export   _EzI2Cs_1_ResumeInt
export    EzI2Cs_1_Start
export   _EzI2Cs_1_Start

export    EzI2Cs_1_DisableInt
export   _EzI2Cs_1_DisableInt
export    EzI2Cs_1_Stop
export   _EzI2Cs_1_Stop
export    EzI2Cs_1_DisableSlave
export   _EzI2Cs_1_DisableSlave
export    EzI2Cs_1_SetRamBuffer
export   _EzI2Cs_1_SetRamBuffer
export    EzI2Cs_1_GetAddr
export   _EzI2Cs_1_GetAddr
export    EzI2Cs_1_GetActivity
export   _EzI2Cs_1_GetActivity


IF (EzI2Cs_1_DYNAMIC_ADDR | EzI2Cs_1_AUTO_ADDR_CHECK) ;; Enable this function if Address is Dynamic or the AUTO_ADDR_CHECK is enabled
export    EzI2Cs_1_SetAddr
export   _EzI2Cs_1_SetAddr
ENDIF

IF (EzI2Cs_1_ROM_ENABLE)  ;; Enable only if alternate ROM Address is Enabled
export    EzI2Cs_1_SetRomBuffer
export   _EzI2Cs_1_SetRomBuffer
ENDIF


AREA UserModules (ROM, REL, CON)

.SECTION

;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_Start
;
;  DESCRIPTION:
;   Initialize the EzI2Cs_1 I2C bus interface.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;
;  RETURNS: none
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;    IN THE LARGE MEMORY MODEL CURRENTLY ONLY THE PAGE POINTER 
;    REGISTERS LISTED BELOW ARE MODIFIED.  THIS DOES NOT GUARANTEE 
;    THAT IN FUTURE IMPLEMENTATIONS OF THIS FUNCTION OTHER PAGE POINTER 
;    REGISTERS WILL NOT BE MODIFIED.
;          
;    Page Pointer Registers Modified: 
;          CUR_PP
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_Start:
_EzI2Cs_1_Start:

   RAM_PROLOGUE RAM_USE_CLASS_4
   RAM_SETPAGE_CUR >EzI2Cs_1_varPage
   
IF (EzI2Cs_1_DYNAMIC_ADDR)  ;; DYNAMIC ADDRESS
IF (EzI2Cs_1_AUTO_ADDR_CHECK^1) ;; for CY8C28X45 chip: do not touch the EzI2Cs_1_bAddr variable if AutoAddressCompare feature is enabled.
   mov  [EzI2Cs_1_bAddr],EzI2Cs_1_SLAVE_ADDR
ENDIF   
ENDIF

   M8C_SetBank1 ;The SDA and SCL pins are setting to Hi-z drive mode
   and reg[EzI2Cs_1SDA_DriveMode_0_ADDR],~(EzI2Cs_1SDA_MASK|EzI2Cs_1SCL_MASK)
   or  reg[EzI2Cs_1SDA_DriveMode_1_ADDR], (EzI2Cs_1SDA_MASK|EzI2Cs_1SCL_MASK)
   M8C_SetBank0
   or  reg[EzI2Cs_1SDA_DriveMode_2_ADDR], (EzI2Cs_1SDA_MASK|EzI2Cs_1SCL_MASK)

   mov  [EzI2Cs_1_bState],0x00    ;; Make sure state machine is initialized
   mov [EzI2Cs_1_bBusy_Flag],EzI2Cs_1_I2C_FREE ;; Clear Busy flag

   call EzI2Cs_1_EnableInt
   call EzI2Cs_1_EnableSlave

   nop
   nop
   nop
   nop
   nop
   
   mov A, 0
   mov [EzI2Cs_1_bRAM_RWoffset], A
IF (EzI2Cs_1_ROM_ENABLE)
   mov [EzI2Cs_1_bROM_RWoffset], A
ENDIF
   
   M8C_SetBank1 ;The SDA and SCL pins are restored to Open Drain Low drive mode
   or reg[EzI2Cs_1SDA_DriveMode_0_ADDR], (EzI2Cs_1SDA_MASK|EzI2Cs_1SCL_MASK)
   or reg[EzI2Cs_1SDA_DriveMode_1_ADDR], (EzI2Cs_1SDA_MASK|EzI2Cs_1SCL_MASK)
   M8C_SetBank0
   or reg[EzI2Cs_1SDA_DriveMode_2_ADDR], (EzI2Cs_1SDA_MASK|EzI2Cs_1SCL_MASK)

   RAM_EPILOGUE RAM_USE_CLASS_4
   ret

.ENDSECTION

IF (EzI2Cs_1_DYNAMIC_ADDR | EzI2Cs_1_AUTO_ADDR_CHECK) ;; Enable this function if Address is Dynamic or the AUTO_ADDR_CHECK is enabled
.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_SetAddr(BYTE bAddr)
;
;  DESCRIPTION:
;   Set the I2C slave address for the EzI2Cs_1 I2C bus interface.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;      A =>  Slave address
;
;  RETURNS: none
;
;  SIDE EFFECTS;    
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;    IN THE LARGE MEMORY MODEL CURRENTLY ONLY THE PAGE POINTER 
;    REGISTERS LISTED BELOW ARE MODIFIED.  THIS DOES NOT GUARANTEE 
;    THAT IN FUTURE IMPLEMENTATIONS OF THIS FUNCTION OTHER PAGE POINTER 
;    REGISTERS WILL NOT BE MODIFIED.
;          
;    Page Pointer Registers Modified: 
;          CUR_PP
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_SetAddr:
_EzI2Cs_1_SetAddr:
   RAM_PROLOGUE RAM_USE_CLASS_4
IF (EzI2Cs_1_AUTO_ADDR_CHECK^1) ;; for CY8C28X45 chip: do not touch the EzI2Cs_1_bAddr variable if AutoAddressCompare feature is enabled.
   RAM_SETPAGE_CUR >EzI2Cs_1_bAddr
   asl   A
   mov   [EzI2Cs_1_bAddr],A
ELSE                          ;; write to the ADDR register instead
   RAM_PROLOGUE RAM_USE_CLASS_2
   and  A, ~EzI2Cs_1_HW_ADDR_MASK	; verify address value
   mov  X, SP
   push A                                   ; store address value
   M8C_SetBank1               ;; Set Bank 1
   mov  A, reg[EzI2Cs_1_ADDR_REG]   ; get value from address register
   and  A, EzI2Cs_1_HW_ADDR_MASK    ; define highest bit
   or   A, [X]                              ; form address value 	
   mov  reg[EzI2Cs_1_ADDR_REG], A   ; set new address value to register
   M8C_SetBank0               ;; Set Bank 0
   pop  A
   RAM_EPILOGUE RAM_USE_CLASS_2
ENDIF
   RAM_EPILOGUE RAM_USE_CLASS_4
   ret

.ENDSECTION
ENDIF

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME:BYTE EzI2Cs_1_GetActivity(void)
;
;  DESCRIPTION:
;    Return a non-zero value if the I2C hardware has seen activity on the bus.
;    The activity flag will be cleared if set when calling this function.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:  none
;
;  RETURNS: 
;    BYTE  non-zero = Activity
;          zero     = No Activity
;
;  SIDE EFFECTS;    
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;    IN THE LARGE MEMORY MODEL CURRENTLY ONLY THE PAGE POINTER 
;    REGISTERS LISTED BELOW ARE MODIFIED.  THIS DOES NOT GUARANTEE 
;    THAT IN FUTURE IMPLEMENTATIONS OF THIS FUNCTION OTHER PAGE POINTER 
;    REGISTERS WILL NOT BE MODIFIED.
;          
;    Page Pointer Registers Modified: 
;          CUR_PP
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_GetActivity:
_EzI2Cs_1_GetActivity:
   RAM_PROLOGUE RAM_USE_CLASS_4
   RAM_SETPAGE_CUR >EzI2Cs_1_bState
   mov   A,[EzI2Cs_1_bState]
   and   A,EzI2Cs_1_ACTIVITY_MASK         ; Mask off activity bits
   and   [EzI2Cs_1_bState],~EzI2Cs_1_ACTIVITY_MASK ; Clear system activity bits

EzI2Cs_1_GetActivity_End:
   RAM_EPILOGUE RAM_USE_CLASS_4
   ret

.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: BYTE EzI2Cs_1_GetAddr(Void)
;
;  DESCRIPTION:
;   Get the I2C slave address for the EzI2Cs_1 I2C bus interface.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS: none
;
;  SIDE EFFECTS;    
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;    IN THE LARGE MEMORY MODEL CURRENTLY ONLY THE PAGE POINTER 
;    REGISTERS LISTED BELOW ARE MODIFIED.  THIS DOES NOT GUARANTEE 
;    THAT IN FUTURE IMPLEMENTATIONS OF THIS FUNCTION OTHER PAGE POINTER 
;    REGISTERS WILL NOT BE MODIFIED.
;          
;    Page Pointer Registers Modified: 
;          CUR_PP
;
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_GetAddr:
_EzI2Cs_1_GetAddr:

IF (EzI2Cs_1_DYNAMIC_ADDR | EzI2Cs_1_AUTO_ADDR_CHECK) ;; if Address is Dynamic or the AUTO_ADDR_CHECK is enabled
   RAM_PROLOGUE RAM_USE_CLASS_4
IF (EzI2Cs_1_AUTO_ADDR_CHECK^1) ;; for CY8C28X45 chip: do not touch the EzI2Cs_1_bAddr variable if AutoAddressCompare feature is enabled.
   RAM_SETPAGE_CUR >EzI2Cs_1_bAddr
   mov   A,[EzI2Cs_1_bAddr]
   asr   A                          ; Shift Addr to right to drop RW bit.
ELSE                          ;; read the address from ADDR register instead
   M8C_SetBank1               ;; Set Bank 1 
   mov A, reg[EzI2Cs_1_ADDR_REG]
   M8C_SetBank0               ;; Set Bank 0
ENDIF
   and   A, 0x7f              ; Mask off bogus MSb
   RAM_EPILOGUE RAM_USE_CLASS_4
ELSE
   mov   A,0xb            
ENDIF
   ret

.ENDSECTION



.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_EnableInt
;  FUNCTION NAME: EzI2Cs_1_ResumeInt
;  DESCRIPTION:
;     Enables SDA interrupt allowing start condition detection. Remember to call the
;     global interrupt enable function by using the macro: M8C_EnableGInt.
;	  EzI2Cs_1_ResumeInt performs the enable int function without fist clearing
;     pending interrupts.
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS: none
;
;  SIDE EFFECTS: REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;
 EzI2Cs_1_ResumeInt:
_EzI2Cs_1_ResumeInt:
   RAM_PROLOGUE RAM_USE_CLASS_1
   jmp   ResumeEntry

 EzI2Cs_1_EnableInt:
_EzI2Cs_1_EnableInt:
   RAM_PROLOGUE RAM_USE_CLASS_1
   ;first clear any pending interrupts
   and   reg[INT_CLR3], ~EzI2Cs_1_INT_MASK   
ResumeEntry:
   M8C_EnableIntMask EzI2Cs_1_INT_REG, EzI2Cs_1_INT_MASK
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_EnableSlave
;
;  DESCRIPTION:
;     Enables SDA interrupt allowing start condition detection. Remember to call the
;     global interrupt enable function by using the macro: M8C_EnableGInt.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS: none
;
;  SIDE EFFECTS: REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_EnableSlave:
_EzI2Cs_1_EnableSlave:

    RAM_PROLOGUE RAM_USE_CLASS_1
    
    IF (EzI2Cs_1_CY8C27XXXA_ID) ;; Enable this code if we have CY8C27XXXA chip ID
    ; Save original CPU clock speed
    M8C_SetBank1          ; Set Bank 1 
    mov  A,reg[OSC_CR0]   ; Get current configuration of OSC_CR0 (Bank 1)
    push A                ; Save OSC_CR0 configuration
    and  A,0xF8           ; Mask off CPU speed
    or   A,0x05           ; Set clock to 750KHz
    mov  reg[OSC_CR0],A   ; Write new value to OSC_CR0 (Bank 1)
    M8C_SetBank0          ; Back to Bank 0
    ENDIF 
    ; Enable I2C Slave
    IF(EzI2Cs_1_USED_I2C_BLOCK)
    M8C_SetBank1
    or   reg[EzI2Cs_1_CFG_REG],(EzI2Cs_1_CFG_Slave_EN | EzI2Cs_1_CFG_BUS_ERROR_IE | EzI2Cs_1_CFG_STOP_IE)
    M8C_SetBank0
    ELSE
    or   reg[EzI2Cs_1_CFG_REG],(EzI2Cs_1_CFG_Slave_EN | EzI2Cs_1_CFG_BUS_ERROR_IE | EzI2Cs_1_CFG_STOP_IE)
    ENDIF
    IF (EzI2Cs_1_CY8C27XXXA_ID) ;; Enable this code if we have CY8C27XXXA chip ID    
    ; Restore original CPU clock speed
    pop  A
    M8C_SetBank1          ; Set Bank 1
    mov  reg[OSC_CR0],A   ; Restore
    M8C_SetBank0          ; Back to Bank 0
    ENDIF    
    RAM_EPILOGUE RAM_USE_CLASS_1
    ret

.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_DisableInt
;  FUNCTION NAME: EzI2Cs_1_Stop
;
;  DESCRIPTION:
;     Disables EzI2Cs_1 slave by disabling SDA interrupt
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS: none
;
;  SIDE EFFECTS: REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_Stop:
_EzI2Cs_1_Stop:
   RAM_PROLOGUE RAM_USE_CLASS_1

   M8C_DisableIntMask EzI2Cs_1_INT_REG, EzI2Cs_1_INT_MASK
   IF(EzI2Cs_1_USED_I2C_BLOCK)
   M8C_SetBank1
   and  reg[EzI2Cs_1_CFG_REG],~EzI2Cs_1_CFG_Slave_EN
   M8C_SetBank0
   ELSE
   and  reg[EzI2Cs_1_CFG_REG],~EzI2Cs_1_CFG_Slave_EN
   ENDIF
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION



.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_DisableInt
;  FUNCTION NAME: EzI2Cs_1_Stop
;
;  DESCRIPTION:
;     Disables EzI2Cs_1 slave by disabling SDA interrupt
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS: none
;
;  SIDE EFFECTS: REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_DisableInt:
_EzI2Cs_1_DisableInt:
   RAM_PROLOGUE RAM_USE_CLASS_1
   M8C_DisableIntMask EzI2Cs_1_INT_REG, EzI2Cs_1_INT_MASK
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: EzI2Cs_1_DisableSlave
;
;  DESCRIPTION:
;     Disables EzI2Cs_1 slave by disabling SDA interrupt
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: none
;
;  RETURNS: none
;
;  SIDE EFFECTS: REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;

 EzI2Cs_1_DisableSlave:
_EzI2Cs_1_DisableSlave:
   RAM_PROLOGUE RAM_USE_CLASS_1
   IF(EzI2Cs_1_USED_I2C_BLOCK)
   M8C_SetBank1
   and  reg[EzI2Cs_1_CFG_REG],~EzI2Cs_1_CFG_Slave_EN
   M8C_SetBank0
   ELSE
   and  reg[EzI2Cs_1_CFG_REG],~EzI2Cs_1_CFG_Slave_EN
   ENDIF
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: 
;          void EzI2Cs_1_SetRamBuffer(BYTE bSize, BYTE bRWboundry, BYTE * pAddr)
;
;  DESCRIPTION:
;     Sets the location and size of the I2C RAM buffer.          
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: 
;     [SP-3] =>  Size of data structure
;     [SP-4] =>  R/W boundary of (Must be less than or equal to size.)
;     [SP-5] =>  LSB of data pointer
;     [SP-6] =>  MSB of data pointer (Only used for large memory model)
;
;  RETURNS: none
;
;  SIDE EFFECTS;    
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;    IN THE LARGE MEMORY MODEL CURRENTLY ONLY THE PAGE POINTER 
;    REGISTERS LISTED BELOW ARE MODIFIED.  THIS DOES NOT GUARANTEE 
;    THAT IN FUTURE IMPLEMENTATIONS OF THIS FUNCTION OTHER PAGE POINTER 
;    REGISTERS WILL NOT BE MODIFIED.
;          
;    Page Pointer Registers Modified: 
;          CUR_PP
;
;  THEORY of OPERATION or PROCEDURE:
;

; Stack offset constants
RAMBUF_SIZE:   equ  -3   ; Stack position for data structure size.
RW_SIZE:       equ  -4   ; Stack position for R/W area size.       
RAMPTR_LSB:    equ  -5   ; Stack position for RAM pointer LSB.   
RAMPTR_MSB:    equ  -6   ; Stack position for RAM pointer MSB.   

 EzI2Cs_1_SetRamBuffer:
_EzI2Cs_1_SetRamBuffer:

    RAM_PROLOGUE RAM_USE_CLASS_4
    RAM_PROLOGUE RAM_USE_CLASS_2
    RAM_SETPAGE_CUR >EzI2Cs_1_bRAM_Buf_Size     ; Set page to global var page.
                                                        ; All these globals should be
                                                        ; on the same page.          
    mov   X,SP
    mov   A,[X+RAMBUF_SIZE]
    mov   [EzI2Cs_1_bRAM_Buf_Size],A         ; Store the buffer size

    mov   A,[X+RW_SIZE]                            ; Store R/W boundary             
    mov   [EzI2Cs_1_bRAM_Buf_WSize],A        ; 
    
    mov   A,[X+RAMPTR_LSB]                         ; Store only LSB of data pointer
    mov   [EzI2Cs_1_pRAM_Buf_Addr_LSB],A     ; 

IF (SYSTEM_LARGE_MEMORY_MODEL)                             ; Only worry about the address MSB
                                                           ; if in the large memory Model
    mov   A,[X+RAMPTR_MSB]                         ; Store only MSB of data pointer
    mov   [EzI2Cs_1_pRAM_Buf_Addr_MSB],A     ; 
ENDIF

    RAM_EPILOGUE RAM_USE_CLASS_2
    RAM_EPILOGUE RAM_USE_CLASS_4
    ret

.ENDSECTION

IF (EzI2Cs_1_ROM_ENABLE)  ;; Enable only if alternate ROM Address is Enabled
.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: 
;          void EzI2Cs_1_SetRomBuffer(BYTE bSize, BYTE * pAddr)
;
;  DESCRIPTION:
;     Sets the location and size of the I2C ROM buffer.          
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: 
;     [SP-3] =>  Size of data const data structure
;     [SP-4] =>  LSB of data pointer
;     [SP-5] =>  MSB of data pointer (Only used for large memory model)
;
;  RETURNS: none
;
;  SIDE EFFECTS;    
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;    IN THE LARGE MEMORY MODEL CURRENTLY ONLY THE PAGE POINTER 
;    REGISTERS LISTED BELOW ARE MODIFIED.  THIS DOES NOT GUARANTEE 
;    THAT IN FUTURE IMPLEMENTATIONS OF THIS FUNCTION OTHER PAGE POINTER 
;    REGISTERS WILL NOT BE MODIFIED.
;          
;    Page Pointer Registers Modified: 
;          CUR_PP
;
;  THEORY of OPERATION or PROCEDURE:
;

; Stack offset constants
ROMBUF_SIZE:   equ  -3   ; Stack position for data structure size.
ROMPTR_LSB:    equ  -4   ; Stack position for ROM pointer LSB.   
ROMPTR_MSB:    equ  -5   ; Stack position for ROM pointer MSB.   

 EzI2Cs_1_SetRomBuffer:
_EzI2Cs_1_SetRomBuffer:

    RAM_PROLOGUE RAM_USE_CLASS_4
    RAM_PROLOGUE RAM_USE_CLASS_2
    RAM_SETPAGE_CUR >EzI2Cs_1_bROM_Buf_Size     ; Set page to global var page.
                                                        ; All these globals should be
                                                        ; on the same page.          
    mov   X,SP
    mov   A,[X+ROMBUF_SIZE]
    mov   [EzI2Cs_1_bROM_Buf_Size],A         ; Store the buffer size

    mov   A,[X+ROMPTR_LSB]                         ; Store LSB of data pointer
    mov   [EzI2Cs_1_pROM_Buf_Addr_LSB],A     ; 
    mov   A,[X+ROMPTR_MSB]                         ; Store MSB of data pointer
    mov   [EzI2Cs_1_pROM_Buf_Addr_MSB],A     ; 
    RAM_EPILOGUE RAM_USE_CLASS_2
    RAM_EPILOGUE RAM_USE_CLASS_4
    ret

.ENDSECTION
ENDIF

; End of File EzI2Cs_1.asm
