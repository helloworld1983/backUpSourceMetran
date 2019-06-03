################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ceedling/src/unity.c 

OBJS += \
./ceedling/src/unity.o 

C_DEPS += \
./ceedling/src/unity.d 


# Each subdirectory must supply rules for building sources it contributes
ceedling/src/%.o: ../ceedling/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\device\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


