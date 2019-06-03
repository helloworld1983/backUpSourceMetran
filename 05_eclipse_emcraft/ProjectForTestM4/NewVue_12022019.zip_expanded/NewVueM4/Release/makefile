################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include vue/util/subdir.mk
-include vue/ipc/subdir.mk
-include vue/device/subdir.mk
-include vue/board/subdir.mk
-include vue/subdir.mk
-include rtos/freertos/Source/portable/low_power_tickless/subdir.mk
-include rtos/freertos/Source/portable/GCC/ARM_CM4F/subdir.mk
-include rtos/freertos/Source/subdir.mk
-include rpmsg_lite/lib/virtio/subdir.mk
-include rpmsg_lite/lib/rpmsg_lite/porting/platform/imx8mq_m4/subdir.mk
-include rpmsg_lite/lib/rpmsg_lite/porting/environment/subdir.mk
-include rpmsg_lite/lib/rpmsg_lite/subdir.mk
-include rpmsg_lite/lib/common/subdir.mk
-include devices/MIMX8MQ6/utilities/str/subdir.mk
-include devices/MIMX8MQ6/utilities/log/subdir.mk
-include devices/MIMX8MQ6/utilities/io/subdir.mk
-include devices/MIMX8MQ6/utilities/subdir.mk
-include devices/MIMX8MQ6/drivers/subdir.mk
-include devices/MIMX8MQ6/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
NewVueM4.hex \

SECONDARY_SIZE += \
NewVueM4.siz \


# All Target
all: NewVueM4.elf secondary-outputs

# Tool invocations
NewVueM4.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Xlinker --gc-sections -Wl,-Map,"NewVueM4.map" -o "NewVueM4.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

NewVueM4.hex: NewVueM4.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "NewVueM4.elf"  "NewVueM4.hex"
	@echo 'Finished building: $@'
	@echo ' '

NewVueM4.siz: NewVueM4.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=berkeley "NewVueM4.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) NewVueM4.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
