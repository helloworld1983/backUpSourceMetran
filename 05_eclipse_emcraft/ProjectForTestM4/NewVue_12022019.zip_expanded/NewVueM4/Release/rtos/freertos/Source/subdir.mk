################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rtos/freertos/Source/croutine.c \
../rtos/freertos/Source/event_groups.c \
../rtos/freertos/Source/list.c \
../rtos/freertos/Source/queue.c \
../rtos/freertos/Source/tasks.c \
../rtos/freertos/Source/timers.c 

OBJS += \
./rtos/freertos/Source/croutine.o \
./rtos/freertos/Source/event_groups.o \
./rtos/freertos/Source/list.o \
./rtos/freertos/Source/queue.o \
./rtos/freertos/Source/tasks.o \
./rtos/freertos/Source/timers.o 

C_DEPS += \
./rtos/freertos/Source/croutine.d \
./rtos/freertos/Source/event_groups.d \
./rtos/freertos/Source/list.d \
./rtos/freertos/Source/queue.d \
./rtos/freertos/Source/tasks.d \
./rtos/freertos/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
rtos/freertos/Source/%.o: ../rtos/freertos/Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


