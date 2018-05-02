################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeAlarm.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeAudio.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeCompare.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeDigital.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeSPI.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeTimer.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeTouch.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeUSBSerial.cpp \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/Snoozelc5vBuffer.cpp 

C_SRCS += \
C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/wake.c 

C_DEPS += \
./Libraries/Snooze/utility/wake.d 

OBJS += \
./Libraries/Snooze/utility/SnoozeAlarm.o \
./Libraries/Snooze/utility/SnoozeAudio.o \
./Libraries/Snooze/utility/SnoozeCompare.o \
./Libraries/Snooze/utility/SnoozeDigital.o \
./Libraries/Snooze/utility/SnoozeSPI.o \
./Libraries/Snooze/utility/SnoozeTimer.o \
./Libraries/Snooze/utility/SnoozeTouch.o \
./Libraries/Snooze/utility/SnoozeUSBSerial.o \
./Libraries/Snooze/utility/Snoozelc5vBuffer.o \
./Libraries/Snooze/utility/wake.o 

CPP_DEPS += \
./Libraries/Snooze/utility/SnoozeAlarm.d \
./Libraries/Snooze/utility/SnoozeAudio.d \
./Libraries/Snooze/utility/SnoozeCompare.d \
./Libraries/Snooze/utility/SnoozeDigital.d \
./Libraries/Snooze/utility/SnoozeSPI.d \
./Libraries/Snooze/utility/SnoozeTimer.d \
./Libraries/Snooze/utility/SnoozeTouch.d \
./Libraries/Snooze/utility/SnoozeUSBSerial.d \
./Libraries/Snooze/utility/Snoozelc5vBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Snooze/utility/SnoozeAlarm.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeAlarm.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeAudio.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeAudio.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeCompare.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeCompare.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeDigital.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeDigital.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeSPI.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeSPI.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeTimer.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeTimer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeTouch.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeTouch.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/SnoozeUSBSerial.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/SnoozeUSBSerial.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/Snoozelc5vBuffer.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/Snoozelc5vBuffer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10803 -DTEENSYDUINO=137 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"D:\Dropbox\EnchantOS\Source" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Bounce2" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\EEPROM" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FastLED" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Snooze" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire" -I"D:\Dropbox\EnchantOS\Libraries\Adafruit_BluefruitLE_nRF51-master" -I"D:\Dropbox\EnchantOS\Libraries\SparkFun_LSM6DS3_Arduino_Library-master" -I"D:\Dropbox\EnchantOS\Libraries\minIni-master\dev" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SerialFlash" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\utility" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -nostdlib -fno-exceptions -felide-constructors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/Snooze/utility/wake.o: C:/Program\ Files\ (x86)/Arduino/hardware/teensy/avr/libraries/Snooze/utility/wake.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall  -g -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DTEENSYDUINO=137 -DARDUINO=10803 -D__MK64FX512__ -D__ENABLE_SD__ -I"C:\Program Files (x86)\Arduino\hardware\tools\arm\arm-none-eabi\include" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"D:\Dropbox\EnchantOS\Source\LiteFXOS" -std=gnu11 -nostdlib -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


