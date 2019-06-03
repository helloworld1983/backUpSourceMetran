################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vue/util/Delay.c \
../vue/util/kfc.c \
../vue/util/kiss_fastfir.c \
../vue/util/kiss_fft.c \
../vue/util/kiss_fftnd.c \
../vue/util/kiss_fftndr.c \
../vue/util/kiss_fftr.c 

OBJS += \
./vue/util/Delay.o \
./vue/util/kfc.o \
./vue/util/kiss_fastfir.o \
./vue/util/kiss_fft.o \
./vue/util/kiss_fftnd.o \
./vue/util/kiss_fftndr.o \
./vue/util/kiss_fftr.o 

C_DEPS += \
./vue/util/Delay.d \
./vue/util/kfc.d \
./vue/util/kiss_fastfir.d \
./vue/util/kiss_fft.d \
./vue/util/kiss_fftnd.d \
./vue/util/kiss_fftndr.d \
./vue/util/kiss_fftr.d 


# Each subdirectory must supply rules for building sources it contributes
vue/util/%.o: ../vue/util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue\ipc" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\CMSIS\Include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\drivers" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\io" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\log" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\utilities\str" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rtos\freertos\Source\include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rtos\freertos\Source\portable\GCC\ARM_CM4F" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\devices\MIMX8MQ6\gcc" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rpmsg_lite\lib\include" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\rpmsg_lite\lib\include\platform\imx8mq_m4" -I"E:\OneDrive - CONG TY TNHH METRAN VIET NAM\NewVue\NewVueM4\vue\util" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


