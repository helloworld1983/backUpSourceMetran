################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../devices/MIMX8MQ6/system_MIMX8MQ6_cm4.c 

OBJS += \
./devices/MIMX8MQ6/system_MIMX8MQ6_cm4.o 

C_DEPS += \
./devices/MIMX8MQ6/system_MIMX8MQ6_cm4.d 


# Each subdirectory must supply rules for building sources it contributes
devices/MIMX8MQ6/%.o: ../devices/MIMX8MQ6/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


