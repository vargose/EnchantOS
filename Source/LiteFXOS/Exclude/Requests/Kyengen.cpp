#include <Wire.h>
#include "WProgram.h"

#include "FastLED.h"

extern "C" {
#include "LiteFXOS.h"
}
#include "LiteFXOS/LightFX.h"
#include "LiteFXOS/LiteFX.h"

#define ENABLE_5V_PIN	8
#define LED_DATA_PIN	9

#define NUM_LEDS 		20
#define COLOR_ORDER		GRB

CRGB LEDStrip[NUM_LEDS];
CRGB LEDStripFullBrightness[NUM_LEDS];

LITE_FX_T 		 Animation;
LITE_FX_THREAD_T Thread;

void StartFlicker()
{
	//set y,r,r pattern
	for (uint16_t n = 0; n + 3 < NUM_LEDS; n += 3)
		LEDStrip[n+2] = CRGB::Red;

	for (uint16_t n = 0; n < NUM_LEDS; n++)
		LEDStripFullBrightness[n] = LEDStrip[n];

	// We can reuse the same animation and thread objects since they are no longer used
	// set flicker settings

	LiteFX_InitColorFaderStrip(&Animation, LEDStrip, 0, NUM_LEDS, 0, true, 100, 120, true, LEDStripFullBrightness, CRGB::Black, 0, 60, 0);
}

void Reset()
{
	// orange light run up the length of the strand
	LiteFX_InitColorWipe(&Animation, LEDStrip, 0, NUM_LEDS, 0, true, true, CRGB::Orange, CRGB::Black, NUM_LEDS, 20, StartFlicker);
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

	if (LiteFX_ProcThread(&Thread)) updateLEDs = 1;
	if (updateLEDs) FastLED.show();
}

extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
//  int _write(){return -1;}
}
