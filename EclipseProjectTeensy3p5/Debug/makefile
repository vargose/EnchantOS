################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Source/LiteFXOS/Examples/subdir.mk
-include Source/LiteFXOS/subdir.mk
-include Source/subdir.mk
-include Libraries/teensy3/subdir.mk
-include Libraries/minIni-master/dev/subdir.mk
-include Libraries/Wire/utility/subdir.mk
-include Libraries/Wire/subdir.mk
-include Libraries/SparkFun_LSM6DS3_Arduino_Library-master/src/subdir.mk
-include Libraries/Snooze/utility/subdir.mk
-include Libraries/Snooze/subdir.mk
-include Libraries/SerialFlash/subdir.mk
-include Libraries/SPI/subdir.mk
-include Libraries/SD/utility/subdir.mk
-include Libraries/SD/subdir.mk
-include Libraries/NXPMotionSense/subdir.mk
-include Libraries/FastLED/subdir.mk
-include Libraries/EEPROM/subdir.mk
-include Libraries/Bounce2/subdir.mk
-include Libraries/Audio/utility/subdir.mk
-include Libraries/Audio/subdir.mk
-include Libraries/Adafruit_BluefruitLE_nRF51-master/utility/subdir.mk
-include Libraries/Adafruit_BluefruitLE_nRF51-master/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
EnchantOSTeensy3p5.hex \

SECONDARY_SIZE += \
EnchantOSTeensy3p5.siz \


# All Target
all: EnchantOSTeensy3p5.elf secondary-outputs

# Tool invocations
EnchantOSTeensy3p5.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM GNU C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -T "C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\mk64fx512.ld" -Xlinker --gc-sections -Xlinker --defsym=__rtc_localtime=1499566176 -Wl,-Map,"EnchantOSTeensy3p5.map" --specs=nano.specs -o "EnchantOSTeensy3p5.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

EnchantOSTeensy3p5.hex: EnchantOSTeensy3p5.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "EnchantOSTeensy3p5.elf"  "EnchantOSTeensy3p5.hex"
	@echo 'Finished building: $@'
	@echo ' '

EnchantOSTeensy3p5.siz: EnchantOSTeensy3p5.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "EnchantOSTeensy3p5.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(SECONDARY_SIZE)$(PDE_DEPS)$(C_DEPS)$(CC_DEPS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(SECONDARY_FLASH)$(ASM_DEPS)$(INO_DEPS)$(S_UPPER_DEPS)$(CPP_DEPS) EnchantOSTeensy3p5.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
