################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vue/ipc/IpcInterface.c \
../vue/ipc/ipc_task.c 

OBJS += \
./vue/ipc/IpcInterface.o \
./vue/ipc/ipc_task.o 

C_DEPS += \
./vue/ipc/IpcInterface.d \
./vue/ipc/ipc_task.d 


# Each subdirectory must supply rules for building sources it contributes
vue/ipc/%.o: ../vue/ipc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DCPU_MIMX8MQ6DVAJZ -D__DEBUG -D__FPU_PRESENT -DARM_MATH_CM4 -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue\ipc" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\CMSIS\Include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\drivers" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\io" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\log" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\str" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rtos\freertos\Source\include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rtos\freertos\Source\portable\GCC\ARM_CM4F" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\gcc" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rpmsg_lite\lib\include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rpmsg_lite\lib\include\platform\imx8mq_m4" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue\util" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

