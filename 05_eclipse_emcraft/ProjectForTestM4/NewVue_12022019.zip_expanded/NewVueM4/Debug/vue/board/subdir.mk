################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vue/board/board.c \
../vue/board/clock_config.c \
../vue/board/i2c.c \
../vue/board/pin_mux.c \
../vue/board/spi.c \
../vue/board/uart.c 

OBJS += \
./vue/board/board.o \
./vue/board/clock_config.o \
./vue/board/i2c.o \
./vue/board/pin_mux.o \
./vue/board/spi.o \
./vue/board/uart.o 

C_DEPS += \
./vue/board/board.d \
./vue/board/clock_config.d \
./vue/board/i2c.d \
./vue/board/pin_mux.d \
./vue/board/spi.d \
./vue/board/uart.d 


# Each subdirectory must supply rules for building sources it contributes
vue/board/%.o: ../vue/board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DCPU_MIMX8MQ6DVAJZ -D__DEBUG -D__FPU_PRESENT -DARM_MATH_CM4 -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\vue" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\CMSIS\Include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\drivers" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\io" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\log" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\utilities\str" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rtos\freertos\Source\include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rtos\freertos\Source\portable\GCC\ARM_CM4F" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\devices\MIMX8MQ6\gcc" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rpmsg_lite\lib\include" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\rpmsg_lite\lib\include\platform\imx8mq_m4" -I"E:\myDrive\00_source_workspace\05_eclipse_emcraft\ProjectForTestM4\NewVue_12022019.zip_expanded\NewVueM4\vue\util" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


