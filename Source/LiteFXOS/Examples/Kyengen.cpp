#include <Wire.h>
#include "WProgram.h"

#include "FastLED.h"

#include "LiteFXOS/LightFX.h"
#include "LiteFXOS/LiteFXOS.h"

#define ENABLE_5V_PIN	8
#define LED_DATA_PIN	9

#define NUM_LEDS 		20
#define COLOR_ORDER		GRB

CRGB LEDStrip[NUM_LEDS];
CRGB LEDStripFullBrightness[NUM_LEDS];

LIGHT_FX_T 			Animation;
LITE_FX_OS_THREAD_T Thread;




void StartFlicker()
{
	//set y,r,r pattern
	for (uint16_t n = 0; n + 3 < NUM_LEDS; n += 3)
		LEDStrip[n+2] = CRGB::Red;

	for (uint16_t n = 0; n < NUM_LEDS; n++)
		LEDStripFullBrightness[n] = LEDStrip[n];

	// We can reuse the same animation and thread objects since they are no longer used
	// set flicker settings
	LightFX_SetFXColorFaderStrip(&Animation, LEDStrip, 0, NUM_LEDS, 0, true, 100, 120, true, LEDStripFullBrightness, CRGB::Black);
	LiteFXOS_SetThreadCycleArgFreq(&Thread, Animation.Pattern, 60);
	// LiteFXOS_SetThreadMemory(&Thread, &Animation, &LightFX_LoadMemory);
}

void Reset()
{
	// orange light run up the length of the strand
	LightFX_SetFXColorWipe(&Animation, LEDStrip, 0, NUM_LEDS, 0, true, true, CRGB::Orange, CRGB::Black);
	LiteFXOS_SetThreadMomentaryArgTicksTime(&Thread, Animation.Pattern, NUM_LEDS, 1000, StartFlicker);
	LiteFXOS_SetThreadMemory(&Thread, &Animation, &LightFX_LoadMemory);
}

void setup() {
	delay(2000);
	Serial.begin(9600);

	pinMode(ENABLE_5V_PIN, OUTPUT);
	digitalWrite(ENABLE_5V_PIN, HIGH);

	FastLED.addLeds<WS2812B, LED_DATA_PIN, COLOR_ORDER>(LEDStrip, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(120);
	FastLED.show();

	Reset();
}

void loop()
{
	bool updateLEDs;

	LiteFXOS_SetTimerCounter(millis());
	//Process left side Using LiteFXOS
	if (LiteFXOS_ProcThread(&Thread)) updateLEDs = 1;

	if (updateLEDs) FastLED.show();

	if (Serial.available())
	{
		char ch = Serial.read();
		if (ch == 'e')
		{
			Serial.println("echo");
		}
	}
}

extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
//  int _write(){return -1;}
}
