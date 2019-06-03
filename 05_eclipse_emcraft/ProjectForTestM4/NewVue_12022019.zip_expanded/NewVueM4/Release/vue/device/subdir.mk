################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vue/device/ADCSensor.c \
../vue/device/ActivePressureSensor.c \
../vue/device/CMVController.c \
../vue/device/DeviceInterface.c \
../vue/device/DeviceTask.c \
../vue/device/DigitalIO.c \
../vue/device/ExhController.c \
../vue/device/FlowController.c \
../vue/device/HFOController.c \
../vue/device/HFOServo.c \
../vue/device/I2CSensor.c \
../vue/device/MotorController.c \
../vue/device/NCPAPController.c \
../vue/device/RS485Sensor.c \
../vue/device/VCVController.c \
../vue/device/i2c.c \
../vue/device/spi.c \
../vue/device/uart.c 

OBJS += \
./vue/device/ADCSensor.o \
./vue/device/ActivePressureSensor.o \
./vue/device/CMVController.o \
./vue/device/DeviceInterface.o \
./vue/device/DeviceTask.o \
./vue/device/DigitalIO.o \
./vue/device/ExhController.o \
./vue/device/FlowController.o \
./vue/device/HFOController.o \
./vue/device/HFOServo.o \
./vue/device/I2CSensor.o \
./vue/device/MotorController.o \
./vue/device/NCPAPController.o \
./vue/device/RS485Sensor.o \
./vue/device/VCVController.o \
./vue/device/i2c.o \
./vue/device/spi.o \
./vue/device/uart.o 

C_DEPS += \
./vue/device/ADCSensor.d \
./vue/device/ActivePressureSensor.d \
./vue/device/CMVController.d \
./vue/device/DeviceInterface.d \
./vue/device/DeviceTask.d \
./vue/device/DigitalIO.d \
./vue/device/ExhController.d \
./vue/device/FlowController.d \
./vue/device/HFOController.d \
./vue/device/HFOServo.d \
./vue/device/I2CSensor.d \
./vue/device/MotorController.d \
./vue/device/NCPAPController.d \
./vue/device/RS485Sensor.d \
./vue/device/VCVController.d \
./vue/device/i2c.d \
./vue/device/spi.d \
./vue/device/uart.d 


# Each subdirectory must supply rules for building sources it contributes
vue/device/%.o: ../vue/device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue\ipc" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\CMSIS\Include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\drivers" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\io" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\log" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\str" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rtos\freertos\Source\include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rtos\freertos\Source\portable\GCC\ARM_CM4F" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\gcc" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rpmsg_lite\lib\include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rpmsg_lite\lib\include\platform\imx8mq_m4" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue\util" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


