################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Device_Task_Test.c \
../src/Device_Task_Test_Runner.c \
../src/FH310_m0_unitest.c \
../src/cr_startup_lpc43xx-m0app.c \
../src/sysinit.c 

OBJS += \
./src/Device_Task_Test.o \
./src/Device_Task_Test_Runner.o \
./src/FH310_m0_unitest.o \
./src/cr_startup_lpc43xx-m0app.o \
./src/sysinit.o 

C_DEPS += \
./src/Device_Task_Test.d \
./src/Device_Task_Test_Runner.d \
./src/FH310_m0_unitest.d \
./src/cr_startup_lpc43xx-m0app.d \
./src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\device\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


