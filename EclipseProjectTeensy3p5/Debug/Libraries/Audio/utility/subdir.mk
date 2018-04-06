################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Audio/utility/sqrt_integer.c 

C_DEPS += \
./Libraries/Audio/utility/sqrt_integer.d 

OBJS += \
./Libraries/Audio/utility/sqrt_integer.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Audio/utility/sqrt_integer.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Audio/utility/sqrt_integer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DTEENSYDUINO=137 -DARDUINO=10803 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -std=gnu11 -nostdlib -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


