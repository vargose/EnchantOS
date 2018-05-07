#ifndef SOURCE_ENCHANTBOARD_H_
#define SOURCE_ENCHANTBOARD_H_

#ifdef __MK64FX512__
	// Use these with the Teensy 3.5 & 3.6 SD card
	#define SDCARD_CS_PIN    BUILTIN_SDCARD
	#define SDCARD_MOSI_PIN  11  // not actually used
	#define SDCARD_SCK_PIN   13  // not actually used
	#define SDCARD_MISO_PIN  12  // not actually used
#endif
#ifdef __MK20DX256__
	#define SDCARD_CS_PIN    15
	#define SDCARD_SCK_PIN   14
	#define SDCARD_MOSI_PIN  11
	#define SDCARD_MISO_PIN  12
#endif

#define SDCARD_DETECT  		7

#define ENABLE_AMP_PIN		2
#define ENABLE_5V_PIN		8

#define LED_DATA_PIN0		9
#define LED_DATA_PIN1		10

#define BUTTON_PIN_POWER 	13
#define BUTTON_PIN_AUX		18

//#define BUTTON_PIN_19	19
//#define BUTTON_PIN_20	20
//#define BUTTON_PIN_21	21
//#define BUTTON_PIN_22 22
//#define BUTTON_PIN_23	23

//#define PWM_PIN_2	21
//#define PWM_PIN_1 22
//#define PWM_PIN_0	23

#define BATTERY_PIN 	A11
#define MICROPHONE_PIN 	A10

inline void BoardInit()
{
	pinMode(ENABLE_AMP_PIN,	 INPUT_PULLUP);
	pinMode(ENABLE_5V_PIN,  INPUT_PULLUP);
}

inline void BoardOn()
{
	digitalWrite(ENABLE_AMP_PIN, HIGH);
	digitalWrite(ENABLE_5V_PIN, HIGH);
}

inline void BoardOff()
{
	digitalWrite(ENABLE_AMP_PIN, LOW);
	digitalWrite(ENABLE_5V_PIN, LOW);
}

//inline bool BatteryLessThan3V()
//{
//	return (analogRead(BATTERY_PIN) < 240);
//}

inline void BatterySensorInit()
{
	analogReadAveraging(32);
}

inline uint32_t BatteryGetRaw()
{
	return analogRead(BATTERY_PIN);
}

#define MICROPHONE_SAMPLE_INTERVAL 30

inline float MicrophoneGetPeakToPeak()
{
	static uint32_t startMillis = 0;  // Start of sample window

	static uint16_t sample;
	static uint16_t peakToPeak = 0;   // peak-to-peak level
	static uint16_t signalMax = 0;
	static uint16_t signalMin = 1024;

	sample = analogRead(MICROPHONE_PIN);

	 if (sample > signalMax)
	 {
		signalMax = sample;  // save just the max levels
	 }
	 else if (sample < signalMin)
	 {
		signalMin = sample;  // save just the min levels
	 }

	if (millis() - startMillis > MICROPHONE_SAMPLE_INTERVAL) //new sample
	{
		peakToPeak = signalMax - signalMin;

		startMillis = millis();
		signalMax = 0;
		signalMin = 1024;
	}

	return float(peakToPeak)/1023;
}

#endif /* SOURCE_ENCHANTBOARD_H_ */
