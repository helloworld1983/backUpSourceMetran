################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../devices/MIMX8MQ6/drivers/fsl_clock.c \
../devices/MIMX8MQ6/drivers/fsl_common.c \
../devices/MIMX8MQ6/drivers/fsl_ecspi.c \
../devices/MIMX8MQ6/drivers/fsl_ecspi_freertos.c \
../devices/MIMX8MQ6/drivers/fsl_gpio.c \
../devices/MIMX8MQ6/drivers/fsl_gpt.c \
../devices/MIMX8MQ6/drivers/fsl_i2c.c \
../devices/MIMX8MQ6/drivers/fsl_i2c_freertos.c \
../devices/MIMX8MQ6/drivers/fsl_mu.c \
../devices/MIMX8MQ6/drivers/fsl_pwm.c \
../devices/MIMX8MQ6/drivers/fsl_qspi.c \
../devices/MIMX8MQ6/drivers/fsl_rdc.c \
../devices/MIMX8MQ6/drivers/fsl_rdc_sema42.c \
../devices/MIMX8MQ6/drivers/fsl_sai.c \
../devices/MIMX8MQ6/drivers/fsl_sema4.c \
../devices/MIMX8MQ6/drivers/fsl_tmu.c \
../devices/MIMX8MQ6/drivers/fsl_uart.c \
../devices/MIMX8MQ6/drivers/fsl_uart_freertos.c \
../devices/MIMX8MQ6/drivers/fsl_wdog.c 

OBJS += \
./devices/MIMX8MQ6/drivers/fsl_clock.o \
./devices/MIMX8MQ6/drivers/fsl_common.o \
./devices/MIMX8MQ6/drivers/fsl_ecspi.o \
./devices/MIMX8MQ6/drivers/fsl_ecspi_freertos.o \
./devices/MIMX8MQ6/drivers/fsl_gpio.o \
./devices/MIMX8MQ6/drivers/fsl_gpt.o \
./devices/MIMX8MQ6/drivers/fsl_i2c.o \
./devices/MIMX8MQ6/drivers/fsl_i2c_freertos.o \
./devices/MIMX8MQ6/drivers/fsl_mu.o \
./devices/MIMX8MQ6/drivers/fsl_pwm.o \
./devices/MIMX8MQ6/drivers/fsl_qspi.o \
./devices/MIMX8MQ6/drivers/fsl_rdc.o \
./devices/MIMX8MQ6/drivers/fsl_rdc_sema42.o \
./devices/MIMX8MQ6/drivers/fsl_sai.o \
./devices/MIMX8MQ6/drivers/fsl_sema4.o \
./devices/MIMX8MQ6/drivers/fsl_tmu.o \
./devices/MIMX8MQ6/drivers/fsl_uart.o \
./devices/MIMX8MQ6/drivers/fsl_uart_freertos.o \
./devices/MIMX8MQ6/drivers/fsl_wdog.o 

C_DEPS += \
./devices/MIMX8MQ6/drivers/fsl_clock.d \
./devices/MIMX8MQ6/drivers/fsl_common.d \
./devices/MIMX8MQ6/drivers/fsl_ecspi.d \
./devices/MIMX8MQ6/drivers/fsl_ecspi_freertos.d \
./devices/MIMX8MQ6/drivers/fsl_gpio.d \
./devices/MIMX8MQ6/drivers/fsl_gpt.d \
./devices/MIMX8MQ6/drivers/fsl_i2c.d \
./devices/MIMX8MQ6/drivers/fsl_i2c_freertos.d \
./devices/MIMX8MQ6/drivers/fsl_mu.d \
./devices/MIMX8MQ6/drivers/fsl_pwm.d \
./devices/MIMX8MQ6/drivers/fsl_qspi.d \
./devices/MIMX8MQ6/drivers/fsl_rdc.d \
./devices/MIMX8MQ6/drivers/fsl_rdc_sema42.d \
./devices/MIMX8MQ6/drivers/fsl_sai.d \
./devices/MIMX8MQ6/drivers/fsl_sema4.d \
./devices/MIMX8MQ6/drivers/fsl_tmu.d \
./devices/MIMX8MQ6/drivers/fsl_uart.d \
./devices/MIMX8MQ6/drivers/fsl_uart_freertos.d \
./devices/MIMX8MQ6/drivers/fsl_wdog.d 


# Each subdirectory must supply rules for building sources it contributes
devices/MIMX8MQ6/drivers/%.o: ../devices/MIMX8MQ6/drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DCPU_MIMX8MQ6DVAJZ -D__DEBUG -D__FPU_PRESENT -DARM_MATH_CM4 -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\vue" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\CMSIS\Include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\drivers" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\io" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\log" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\str" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rtos\freertos\Source\include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rtos\freertos\Source\portable\GCC\ARM_CM4F" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\gcc" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rpmsg_lite\lib\include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rpmsg_lite\lib\include\platform\imx8mq_m4" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\vue\util" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


