################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/device_task.c \
D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/ipc_msg.c \
D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/ipc_receive_msg.c \
D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/ipc_send_msg.c \
D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/m0_img_ldr.c 

OBJS += \
./Device_Task/device_task.o \
./Device_Task/ipc_msg.o \
./Device_Task/ipc_receive_msg.o \
./Device_Task/ipc_send_msg.o \
./Device_Task/m0_img_ldr.o 

C_DEPS += \
./Device_Task/device_task.d \
./Device_Task/ipc_msg.d \
./Device_Task/ipc_receive_msg.d \
./Device_Task/ipc_send_msg.d \
./Device_Task/m0_img_ldr.d 


# Each subdirectory must supply rules for building sources it contributes
Device_Task/device_task.o: D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/device_task.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\FH310\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Device_Task/ipc_msg.o: D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/ipc_msg.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\FH310\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Device_Task/ipc_receive_msg.o: D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/ipc_receive_msg.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\FH310\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Device_Task/ipc_send_msg.o: D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/ipc_send_msg.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\FH310\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Device_Task/m0_img_ldr.o: D:/Metran_Project/FH-310/workspace/FH310_m0/FH310/src/m0_img_ldr.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\Metran_Project\FH-310\workspace\lpc_board_ea_oem_4357_m0\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\freertos\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\FH310\inc" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\src" -I"D:\Metran_Project\FH-310\workspace\FH310_m0\unitest\ceedling\extras\fixture\src" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc" -I"D:\Metran_Project\FH-310\workspace\lpc_chip_43xx_m0\inc\usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


