;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME: EzI2Cs_1INT.asm
;;  Version: 1.30, Updated on 2011/9/2 at 9:39:55
;;  Generated by PSoC Designer 5.1.2309
;;
;;  DESCRIPTION: I2CFXM (Slave) Interrupt Service Routine
;;  
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress Semiconductor 2011. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************
include "EzI2Cs_1.inc"
include "m8c.inc"
include "memory.inc"




;-----------------------------------------------
;  Global Symbols
;-----------------------------------------------

export    EzI2Cs_1_varPage      
export   _EzI2Cs_1_varPage      

export    EzI2Cs_1_bState  
export   _EzI2Cs_1_bState

export    EzI2Cs_1_bRAM_RWoffset
export   _EzI2Cs_1_bRAM_RWoffset

export    EzI2Cs_1_bRAM_RWcntr
export   _EzI2Cs_1_bRAM_RWcntr

export   _EzI2Cs_1_pRAM_Buf_Addr_LSB
export    EzI2Cs_1_pRAM_Buf_Addr_LSB

IF (SYSTEM_LARGE_MEMORY_MODEL)
export   _EzI2Cs_1_pRAM_Buf_Addr_MSB
export    EzI2Cs_1_pRAM_Buf_Addr_MSB
ENDIF

export   _EzI2Cs_1_bRAM_Buf_Size                                
export    EzI2Cs_1_bRAM_Buf_Size    

export   _EzI2Cs_1_bRAM_Buf_WSize               
export    EzI2Cs_1_bRAM_Buf_WSize  

IF (EzI2Cs_1_ROM_ENABLE)
export    EzI2Cs_1_bROM_RWoffset
export   _EzI2Cs_1_bROM_RWoffset

export    EzI2Cs_1_bROM_RWcntr   
export   _EzI2Cs_1_bROM_RWcntr   

export   _EzI2Cs_1_pROM_Buf_Addr_LSB
export    EzI2Cs_1_pROM_Buf_Addr_LSB
export   _EzI2Cs_1_pROM_Buf_Addr_MSB
export    EzI2Cs_1_pROM_Buf_Addr_MSB

export   _EzI2Cs_1_bROM_Buf_Size                           
export    EzI2Cs_1_bROM_Buf_Size

ENDIF

export    EzI2Cs_1_bBusy_Flag  
export   _EzI2Cs_1_bBusy_Flag

AREA InterruptRAM (RAM, REL, CON)

;-----------------------------------------------
; Variable Allocation
;-----------------------------------------------


;; Exported variables
 _EzI2Cs_1_varPage:                                ; This points to the variable page
  EzI2Cs_1_varPage:          

 _EzI2Cs_1_bState:
  EzI2Cs_1_bState:                           blk 1

;; RAM space variables
 _EzI2Cs_1_bRAM_RWoffset:                          ; RAM address counter.  This is reset each time
  EzI2Cs_1_bRAM_RWoffset:                    blk 1 ; a read or write is initiated.

 _EzI2Cs_1_bRAM_RWcntr:                            ; RAM Read/Write counter.  Keeps track of offset 
  EzI2Cs_1_bRAM_RWcntr:                      blk 1 ; during a read or write operation.  Reset to
                                                      ; _bRAM_RWoffset at start of R/W command.

IF (SYSTEM_LARGE_MEMORY_MODEL)
 _EzI2Cs_1_pRAM_Buf_Addr_MSB:                      ; Base address (MSB) to RAM buffer.  
  EzI2Cs_1_pRAM_Buf_Addr_MSB:                blk 1 ; 
ENDIF

 _EzI2Cs_1_pRAM_Buf_Addr_LSB:                      ; Base address (LSB) to RAM buffer.  
  EzI2Cs_1_pRAM_Buf_Addr_LSB:                blk 1 ; 

 _EzI2Cs_1_bRAM_Buf_Size:                          ; Size of RAM buffer.   
  EzI2Cs_1_bRAM_Buf_Size:                    blk 1 ; 

 _EzI2Cs_1_bRAM_Buf_WSize:                         ; Portion of the RAM buffer size that is writable.
  EzI2Cs_1_bRAM_Buf_WSize:                   blk 1 ; 

;; ROM space variables
IF (EzI2Cs_1_ROM_ENABLE)
 _EzI2Cs_1_bROM_RWoffset:                          ; ROM address counter.  This is reset each time
  EzI2Cs_1_bROM_RWoffset:                    blk 1 ; a read is initiated

 _EzI2Cs_1_bROM_RWcntr:                            ; ROM read counter. Keeps track of offset 
  EzI2Cs_1_bROM_RWcntr:                      blk 1 ; during a read operation.  Reset to
                                                      ; _bRAM_RWoffset at start of command.

 _EzI2Cs_1_pROM_Buf_Addr_MSB:                      ; ROM address (MSB) counter. (Relative to buffer)  This
  EzI2Cs_1_pROM_Buf_Addr_MSB:                blk 1 ; counter is reset each time a read is initiated.

 _EzI2Cs_1_pROM_Buf_Addr_LSB:                      ; ROM address (MSB) counter. (Relative to buffer)  This
  EzI2Cs_1_pROM_Buf_Addr_LSB:                blk 1 ; counter is reset each time a read is initiated.

 _EzI2Cs_1_bROM_Buf_Size:                          ; Size of RAM buffer.                            
  EzI2Cs_1_bROM_Buf_Size:                    blk 1 ; 

ENDIF

IF (EzI2Cs_1_AUTO_ADDR_CHECK^1)
IF (EzI2Cs_1_DYNAMIC_ADDR) 

export    EzI2Cs_1_bAddr
export   _EzI2Cs_1_bAddr 

 _EzI2Cs_1_bAddr:
  EzI2Cs_1_bAddr:                            blk 1
ENDIF
ENDIF

 _EzI2Cs_1_bBusy_Flag:
  EzI2Cs_1_bBusy_Flag:                       blk 1

;-----------------------------------------------
;  EQUATES and TABLES
;-----------------------------------------------

;; Bit definitions for EzI2Cs_1_bState
STATE_IDLE:         equ  0x00      ; Wait for Correct Address
STATE_WR_RAM_ADDR:  equ  0x02      ; Wait for Secondary address on write
STATE_WR_RAM:       equ  0x04      ; Write RAM Data
STATE_RD_RAM:       equ  0x06      ; Read RAM Data

STATE_WR_ROM_ADDR:  equ  0x08      ; Wait for Secondary address on write
STATE_RD_ROM:       equ  0x0A      ; Read ROM Data

STATE_WR_ROM:       equ  0x0C      ; Write ROM (Not supported at this time)
STATE_RESET:        equ  0x0E      ; Reset state machine

STATE_MASK:         equ  0x0E
STATE_MASK2:        equ  0x0F      ; State Mask

ALT_MODE_FLAG:      equ  0x40      ; Reserved


;@PSoC_UserCode_INIT@ (Do not change this line.)
;---------------------------------------------------
; Insert your custom declarations below this banner
;---------------------------------------------------

;------------------------
;  Constant Definitions
;------------------------


;------------------------
; Variable Allocation
;------------------------


;---------------------------------------------------
; Insert your custom declarations above this banner
;---------------------------------------------------
;@PSoC_UserCode_END@ (Do not change this line.)




AREA UserModules (ROM, REL, CON)

export _EzI2Cs_1_ISR
;;****************************************************
;; I2C_ISR  main entry point from vector 60h
;;
;;****************************************************

 EzI2Cs_1_ISR:
_EzI2Cs_1_ISR:

    push A
    push X
    
    ;@PSoC_UserCode_ISR_START@ (Do not change this line.)
    ;---------------------------------------------------
    ; Insert your custom code below this banner
    ;---------------------------------------------------

    ;---------------------------------------------------
    ; Insert your custom code above this banner
    ;---------------------------------------------------
    ;@PSoC_UserCode_END@ (Do not change this line.)

;; The folling conditional code is only valid when using the
;; large memory model.
IF (SYSTEM_LARGE_MEMORY_MODEL)
   RAM_CHANGE_PAGE_MODE FLAG_PGMODE_2              ; Set Page Mode
   REG_PRESERVE IDX_PP                             ; Save Index Page Pointer
   REG_PRESERVE CUR_PP                             ; Save Current Page Pointer
   RAM_SETPAGE_CUR  >EzI2Cs_1_varPage      ; Set the current page mode Pointer
   mov   A, [EzI2Cs_1_pRAM_Buf_Addr_MSB]   ; Set Index page mode pointer
   RAM_SETPAGE_IDX A
ENDIF

    or   [EzI2Cs_1_bState],EzI2Cs_1_ANY_ACTIVITY                     ; Set Activity flag
    tst  reg[EzI2Cs_1_SCR_REG],EzI2Cs_1_SCR_ADDRESS                  ; Check for address
    jz   .I2C_CHECK_STOP                                             ; Go to check for Stop condition if no Address
    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bits.                       
    or   [EzI2Cs_1_bState],STATE_IDLE                                ; Address flag set, change to IDLE state
    jmp  .I2C_DO_STATE_MACHINE
.I2C_CHECK_STOP:
    ; Check for Stop condition here.  If a stop condition
    ; exists, reset state machine to idle.
    tst reg[EzI2Cs_1_SCR_REG],EzI2Cs_1_SCR_STOP_STATUS               ; Check for Stop condition
    jz .I2C_DO_STATE_MACHINE                                            ; No Stop condition - do I2C state mashine
    ; Following line was commented due to CDT#60202.
    ; The stop bit is cleared by SetEzI2Cs_1_SCR macro, so the functionality related to EzI2Cs_1_bBusy_Flag is preserved.
    ; and reg[EzI2Cs_1_SCR_REG],~EzI2Cs_1_SCR_STOP_STATUS            ; Clear Stop bit
    jmp  I2C_STATE_RESET
.I2C_DO_STATE_MACHINE:
    mov  A,[EzI2Cs_1_bState]    ; Get State
    and  A,STATE_MASK                   ; Mask off invalid states
    jacc I2C_STATE_JUMP_TABLE

I2C_STATE_JUMP_TABLE:
    jmp  I2C_STATE_IDLE                 ; Idle state
    jmp  I2C_STATE_WR_RAM_ADDR              ; Wait for Address write state
    jmp  I2C_STATE_WR_RAM               ; 
    jmp  I2C_STATE_RD_RAM
IF (EzI2Cs_1_ROM_ENABLE)        ; Only valid if ROM enabled
    jmp  I2C_STATE_WR_ROM_ADDR
    jmp  I2C_STATE_RD_ROM
    jmp  I2C_STATE_WR_ROM
ELSE
    jmp  I2C_STATE_RESET
    jmp  I2C_STATE_RESET
    jmp  I2C_STATE_RESET
ENDIF
    jmp  I2C_STATE_RESET

    jmp  EzI2Cs_1_ISR_END


    ;            *** I2C Idle state ***
    ;
    ;   Sit idle until a start with address is issued.
    ;   Check to see if there is an address match
    ;     If address match, ACK the bus and determine next state
    ;     Else NAK the transfer and return to idle state.
    ;   Also check stop for condition.IF (EzI2Cs_1_ROM_ENABLE)  ;; Enable only if alternate ROM Address is Enabled
    ;
I2C_STATE_IDLE:                 ; Idle state

IF (EzI2Cs_1_AUTO_ADDR_CHECK^1)   ;; for CY8C28X45 chip: skip address comparison and NACK sending stage-hardware will do this for us  if AutoAddressCompare feature is enabled.
                                          ;; The code in this pre-compiler directive will be executed for all chips except CY8C28X45.
IF (EzI2Cs_1_DYNAMIC_ADDR)  ;; DYNAMIC ADDRESS
    mov  A,reg[EzI2Cs_1_DR_REG]                                      ; Get transmitted address
    and  A,EzI2Cs_1_ADDR_MASK                                        ; Mask off alt address bit and R/W bit
    cmp  A,[EzI2Cs_1_bAddr]                                          ; Check for proper Address
    jz   .CHK_ADDR_MODE 
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_NAK )                             ; NAK Address 
    jmp  EzI2Cs_1_ISR_END                                            ; Not valid Address, leave

ELSE    ;; STATIC ADDRESS
    mov  A,reg[EzI2Cs_1_DR_REG]                                      ; Get transmitted address
    and  A,EzI2Cs_1_ADDR_MASK                                        ; Mask off alt address bit and R/W bit
    cmp  A,EzI2Cs_1_SLAVE_ADDR                                       ; Check for proper Address
    jz   .CHK_ADDR_MODE 
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_NAK )                             ; NAK Address 
    jmp  EzI2Cs_1_ISR_END                                            ; Not valid Address, leave
ENDIF
ENDIF

.CHK_ADDR_MODE:   ; A proper address has been detected, now determine what mode, R/W alt_addr?? 
IF (EzI2Cs_1_ROM_ENABLE)  ;; Enable only if alternate ROM Address is Enabled
    tst  reg[EzI2Cs_1_DR_REG],EzI2Cs_1_ALT_ADDR_BIT                  ; Check for Alt address
    jnz  SERVICE_ROM_ADDR
ENDIF

.STANDARD_ADDR:
    tst  reg[EzI2Cs_1_DR_REG],EzI2Cs_1_RD_FLAG                       ; Check for a Read operation
    jnz  .PREPARE_FOR_RAM_READ

    mov [EzI2Cs_1_bBusy_Flag], EzI2Cs_1_I2C_BUSY_RAM_WRITE           ; Write transaction in process - set Busy flag to WRITE

    ; Prepare for RAM Write Address operation
    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState],STATE_WR_RAM_ADDR                         ; Set state machine to do RAM Write
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_ACK )                             ; ACK Address 
    jmp  EzI2Cs_1_ISR_END                                     ; Base address to RAM buffer.  

.PREPARE_FOR_RAM_READ:
    mov [EzI2Cs_1_bBusy_Flag], EzI2Cs_1_I2C_BUSY_RAM_READ            ; Possible read transaction in process - set Busy flag to READ

    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState],STATE_RD_RAM                              ; Set state machine to do RAM Read 
    mov  [EzI2Cs_1_bRAM_RWcntr],[EzI2Cs_1_bRAM_RWoffset]             ; Reset address counter to start of Offset
    mov  A,[EzI2Cs_1_pRAM_Buf_Addr_LSB]                              ; Get base address
    add  A,[EzI2Cs_1_bRAM_RWcntr]                                    ; Set Offset and add to base address      
    mov  X,A                                                         ; Put offset in X
    mov  A,[X]                                                       ; Get first byte to transmit
    mov  reg[EzI2Cs_1_DR_REG],A                               ; Base address to RAM buffer.  
    inc  [EzI2Cs_1_bRAM_RWcntr]                                      ; Increment RAM buffer counter to next location.

                                                                     ; ACK command and transmit first byte.
    SetEzI2Cs_1_SCR (EzI2Cs_1_SCR_ACK|EzI2Cs_1_SCR_TRANSMIT)   
    jmp  EzI2Cs_1_ISR_END

    ;            *** I2C Read RAM state ***
    ;
I2C_STATE_RD_RAM:
    ;@PSoC_UserCode_RAM_RD@ (Do not change this line.)
    ;---------------------------------------------------
    ; Insert your custom code below this banner
    ;---------------------------------------------------
    
    ;---------------------------------------------------
    ; Insert your custom code above this banner
    ;---------------------------------------------------
    ;@PSoC_UserCode_END@ (Do not change this line.)
    
    mov  A,[EzI2Cs_1_bRAM_Buf_Size]
    dec  A
    cmp  A,[EzI2Cs_1_bRAM_RWcntr]                                    ; Check to see if out of range.
    jc   .I2C_TRANSMIT_DATA  ; WARNING!! Bogas data will be transmitted if out of range.   

    mov  A,[EzI2Cs_1_pRAM_Buf_Addr_LSB]                              ; Get base address
    add  A,[EzI2Cs_1_bRAM_RWcntr]                                    ; Set Offset and add to base address      
    mov  X,A                                                         ; Put offset in X
    mov  A,[X]                                                       ; Get first byte to transmit
    mov  reg[EzI2Cs_1_DR_REG],A                                      ; Write data to transmit register
    inc  [EzI2Cs_1_bRAM_RWcntr]                                      ; Increment RAM buffer counter to next location.
    or   [EzI2Cs_1_bState],EzI2Cs_1_READ_ACTIVITY                    ; Set Read Activity flag


.I2C_TRANSMIT_DATA:     
    mov  reg[EzI2Cs_1_DR_REG],A                                      ; Write data to transmit register
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_TRANSMIT )                        ; ACK command and transmit first byte. 
    jmp  EzI2Cs_1_ISR_END


    ;            *** I2C Write RAM Address state ***
    ;
    ;  During this state, the RAM address offset is set.
I2C_STATE_WR_RAM_ADDR:              ; Wait for Address write state
    mov  A,reg[EzI2Cs_1_DR_REG]                                      ; Get transmitted Address offset
    cmp  A,[EzI2Cs_1_bRAM_Buf_Size]                                  ; Check if out of range.
    jnc  I2C_NAK_DATA                                                ; If out of range NAK address
    jz   I2C_NAK_DATA

    ; Address in range
    mov  [EzI2Cs_1_bRAM_RWcntr],A                                    ; Reset address counter with new value
    mov  [EzI2Cs_1_bRAM_RWoffset],A                                  ; Set offset with new value.
    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState],STATE_WR_RAM                              ; Set state machine to do RAM Write
    jmp  I2C_ACK_DATA                         ; ACK the data


    ;            *** I2C Write RAM state
I2C_STATE_WR_RAM:  
    ;@PSoC_UserCode_RAM_WR@ (Do not change this line.)
    ;---------------------------------------------------
    ; Insert your custom code below this banner
    ;---------------------------------------------------
    
    ;---------------------------------------------------
    ; Insert your custom code above this banner
    ;---------------------------------------------------
    ;@PSoC_UserCode_END@ (Do not change this line.)
    
    mov  A,[EzI2Cs_1_bRAM_Buf_WSize]                                 ; Get buffer size to make sure we
    jz   I2C_NAK_DATA                                                ; If RAM WSize is zero, do not allow write.
    dec  A                                                           ; are in a valid area.
    cmp  A,[EzI2Cs_1_bRAM_RWcntr]                                    ; Check to see if out of range.
    jc   I2C_NAK_DATA                                                ; If out of range NAK address

    mov  A,[EzI2Cs_1_pRAM_Buf_Addr_LSB]                              ; Get base address
    add  A,[EzI2Cs_1_bRAM_RWcntr]                                    ; Set Offset and add to base address      
    mov  X,A                                                         ; Put offset in X

    mov  A,reg[EzI2Cs_1_DR_REG]                                      ; Read data to be written
    mov  [X],A                                                       ; Store data in Buffer
    or   [EzI2Cs_1_bState],EzI2Cs_1_WRITE_ACTIVITY                   ; Set Write Activity flag
    inc  [EzI2Cs_1_bRAM_RWcntr]                                      ; Advance pointer to next location
    jmp  I2C_ACK_DATA                         ; ACK the data

    
    
IF (EzI2Cs_1_ROM_ENABLE)  ;; Enable only if alternate ROM Address is Enabled

SERVICE_ROM_ADDR:  ; At this time only ROM Read is supported.  
    tst  reg[EzI2Cs_1_DR_REG],EzI2Cs_1_RD_FLAG                       ; Check for a Read operation
    jnz  PREPARE_FOR_ROM_READ

    mov [EzI2Cs_1_bBusy_Flag], EzI2Cs_1_I2C_BUSY_ROM_WRITE           ; Write transaction in process - set Busy flag to WRITE
    
    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState],STATE_WR_ROM_ADDR                         ; Set state machine to do ROM ADDR Write
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_ACK )                             ; ACK Address 
    jmp  EzI2Cs_1_ISR_END                                    ; Base address to RAM buffer.  

    ; Prepare for Write ROM Address.              
I2C_STATE_WR_ROM_ADDR:
    mov  A,reg[EzI2Cs_1_DR_REG]                                      ; Get transmitted Address offset
    cmp  A,[EzI2Cs_1_bROM_Buf_Size]                                  ; Check if out of range.
    jnc  I2C_NAK_DATA                                                ; If out of range NAK address
    jz   I2C_NAK_DATA

    mov  [EzI2Cs_1_bROM_RWcntr],A                                    ; Reset address counter with new value
    mov  [EzI2Cs_1_bROM_RWoffset],A                                  ; Set offset with new value.
    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState],STATE_WR_ROM                              ; Set state machine to do ROM Write
    jmp  I2C_ACK_DATA

I2C_STATE_WR_ROM:  // Flash command interpreter
   ;@PSoC_UserCode_ROM_WR@ (Do not change this line.)
   ;---------------------------------------------------
   ; Insert your custom code below this banner
   ;---------------------------------------------------

   ;---------------------------------------------------
   ; Insert your custom code above this banner
   ;---------------------------------------------------
   ;@PSoC_UserCode_END@ (Do not change this line.)

    jnc  I2C_NAK_DATA                                                ; Write to ROM not supported.

PREPARE_FOR_ROM_READ:
    mov [EzI2Cs_1_bBusy_Flag], EzI2Cs_1_I2C_BUSY_ROM_READ            ; Possible read transaction in process - set Busy flag to READ
    
    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState],STATE_RD_ROM                              ; Set state machine to do ROM Read 
    mov  [EzI2Cs_1_bROM_RWcntr],[EzI2Cs_1_bROM_RWoffset]             ; Reset address counter to start of Offset
    mov  X,[EzI2Cs_1_pROM_Buf_Addr_MSB]                              ; Get MSB of ROM address in X
    mov  A,[EzI2Cs_1_pROM_Buf_Addr_LSB]                              ; Get LSB of ROM base address
    add  A,[EzI2Cs_1_bROM_RWcntr]                                    ; Set Offset and add to base address      
    jnc  .GET_ROM_VALUE
    inc  X    ; Inc the MSB
.GET_ROM_VALUE:
    swap A,X  ; Place MSB of ROM address in A, and LSB in X for ROMX
    romx      ; Get Rom value in A

    mov  reg[EzI2Cs_1_DR_REG],A                              ; Base address to RAM buffer.  
    inc  [EzI2Cs_1_bROM_RWcntr]                                      ; Increment RAM buffer counter to next location.

                                                                     ; ACK command and transmit first byte.
    SetEzI2Cs_1_SCR  (EzI2Cs_1_SCR_ACK|EzI2Cs_1_SCR_TRANSMIT)   
    jmp  EzI2Cs_1_ISR_END


I2C_STATE_RD_ROM:
    ;@PSoC_UserCode_ROM_RD@ (Do not change this line.)
    ;---------------------------------------------------
    ; Insert your custom code below this banner
    ;---------------------------------------------------

    ;---------------------------------------------------
    ; Insert your custom code above this banner
    ;---------------------------------------------------
    ;@PSoC_UserCode_END@ (Do not change this line.)

    mov  A,[EzI2Cs_1_bROM_Buf_Size]
    dec  A
    cmp  A,[EzI2Cs_1_bROM_RWcntr]                                    ; Check to see if out of range.
    jc   .I2C_TRANSMIT_ROM_DATA  ; WARNING!! Bogas data will be transmitted if out of range.   

    mov  X,[EzI2Cs_1_pROM_Buf_Addr_MSB]                              ; Get MSB of ROM address in X
    mov  A,[EzI2Cs_1_pROM_Buf_Addr_LSB]                              ; Get LSB of ROM base address
    add  A,[EzI2Cs_1_bROM_RWcntr]                                    ; Set Offset and add to base address      
    jnc  .GET_ROM_VALUE
    inc  X    ; Inc the MSB
.GET_ROM_VALUE:
    swap A,X  ; Place MSB of ROM address in A, and LSB in X for ROMX
    romx      ; Get Rom value in A
    mov  reg[EzI2Cs_1_DR_REG],A                              ; Base address to RAM buffer.  
    inc  [EzI2Cs_1_bROM_RWcntr]                                      ; Increment RAM buffer counter to next location.

.I2C_TRANSMIT_ROM_DATA:     
    mov  reg[EzI2Cs_1_DR_REG],A                                      ; Write data to transmit register
    SetEzI2Cs_1_SCR  (EzI2Cs_1_SCR_TRANSMIT)                         ; ACK command and transmit first byte. 
    jmp  EzI2Cs_1_ISR_END

ENDIF

;; Generic handlers

I2C_ACK_DATA:
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_ACK )                             ; ACK Data
    jmp  EzI2Cs_1_ISR_END

I2C_NAK_DATA:   ;; NAK data and return  !!WARNING, NOT SURE IF THIS WILL WORK
    SetEzI2Cs_1_SCR ( EzI2Cs_1_SCR_NAK )                             ;  NAK Data
    jmp  EzI2Cs_1_ISR_END

I2C_STATE_RESET:
    ;@PSoC_UserCode_I2C_RST_Start@ (Do not change this line.)
    ;---------------------------------------------------
    ; Insert your custom code below this banner
    ;---------------------------------------------------

    ;---------------------------------------------------
    ; Insert your custom code above this banner
    ;---------------------------------------------------
    ;@PSoC_UserCode_END@ (Do not change this line.)

    and  [EzI2Cs_1_bState],~STATE_MASK2                              ; Clear State bit.                       
    or   [EzI2Cs_1_bState], STATE_IDLE    ; Reset State
    mov [EzI2Cs_1_bBusy_Flag],EzI2Cs_1_I2C_FREE                      ; Clear Busy flag
    ; Reset pointer buffers as well
    ;@PSoC_UserCode_I2C_RST_End@ (Do not change this line.)
    ;---------------------------------------------------
    ; Insert your custom code below this banner
    ;---------------------------------------------------

    ;---------------------------------------------------
    ; Insert your custom code above this banner
    ;---------------------------------------------------
    ;@PSoC_UserCode_END@ (Do not change this line.)

EzI2Cs_1_ISR_END:

; This conditional code is only used when using the large memory model.
IF (SYSTEM_LARGE_MEMORY_MODEL)
   REG_RESTORE CUR_PP           ; Restore Current Page Pointer
   REG_RESTORE IDX_PP           ; Restore Index Page Pointer
ENDIF
    pop  X
    pop  A

    reti
; end of file EzI2Cs_1INT.asm
