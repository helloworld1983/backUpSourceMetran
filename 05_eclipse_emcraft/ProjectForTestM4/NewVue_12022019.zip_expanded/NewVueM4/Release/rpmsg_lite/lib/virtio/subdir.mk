################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rpmsg_lite/lib/virtio/virtqueue.c 

OBJS += \
./rpmsg_lite/lib/virtio/virtqueue.o 

C_DEPS += \
./rpmsg_lite/lib/virtio/virtqueue.d 


# Each subdirectory must supply rules for building sources it contributes
rpmsg_lite/lib/virtio/%.o: ../rpmsg_lite/lib/virtio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


