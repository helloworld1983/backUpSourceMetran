################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rtos/freertos/Source/portable/low_power_tickless/fsl_tickless_gpt.c 

OBJS += \
./rtos/freertos/Source/portable/low_power_tickless/fsl_tickless_gpt.o 

C_DEPS += \
./rtos/freertos/Source/portable/low_power_tickless/fsl_tickless_gpt.d 


# Each subdirectory must supply rules for building sources it contributes
rtos/freertos/Source/portable/low_power_tickless/%.o: ../rtos/freertos/Source/portable/low_power_tickless/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


