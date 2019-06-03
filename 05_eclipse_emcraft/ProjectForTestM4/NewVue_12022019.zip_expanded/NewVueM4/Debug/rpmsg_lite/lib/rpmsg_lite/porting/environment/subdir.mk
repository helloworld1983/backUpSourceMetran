################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rpmsg_lite/lib/rpmsg_lite/porting/environment/rpmsg_env_freertos.c 

OBJS += \
./rpmsg_lite/lib/rpmsg_lite/porting/environment/rpmsg_env_freertos.o 

C_DEPS += \
./rpmsg_lite/lib/rpmsg_lite/porting/environment/rpmsg_env_freertos.d 


# Each subdirectory must supply rules for building sources it contributes
rpmsg_lite/lib/rpmsg_lite/porting/environment/%.o: ../rpmsg_lite/lib/rpmsg_lite/porting/environment/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DCPU_MIMX8MQ6DVAJZ -D__DEBUG -D__FPU_PRESENT -DARM_MATH_CM4 -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\vue" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\CMSIS\Include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\drivers" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\io" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\log" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\str" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rtos\freertos\Source\include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rtos\freertos\Source\portable\GCC\ARM_CM4F" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\gcc" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rpmsg_lite\lib\include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rpmsg_lite\lib\include\platform\imx8mq_m4" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\vue\util" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


