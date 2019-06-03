################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../devices/MIMX8MQ6/gcc/startup_MIMX8MQ6_cm4.S 

OBJS += \
./devices/MIMX8MQ6/gcc/startup_MIMX8MQ6_cm4.o 

S_UPPER_DEPS += \
./devices/MIMX8MQ6/gcc/startup_MIMX8MQ6_cm4.d 


# Each subdirectory must supply rules for building sources it contributes
devices/MIMX8MQ6/gcc/%.o: ../devices/MIMX8MQ6/gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


