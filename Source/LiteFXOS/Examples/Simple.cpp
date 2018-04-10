#include <Wire.h>
#include "WProgram.h"

#include "FastLED.h"

#include "LiteFXOS/LightFX.h"
#include "LiteFXOS/LiteFXOS.h"

#define ENABLE_5V_PIN	8
#define LED_DATA_PIN	9

#define NUM_LEDS 		10
#define COLOR_ORDER		GRB

CRGB LEDStrip[NUM_LEDS];

LIGHT_FX_T 			AnimationLeft;
LITE_FX_OS_THREAD_T AnimationLeftThread;

LITE_FX_OS_THREAD_T AnimationRightThread;

CRGBPalette16 Palette = RainbowColors_p;

void AnimationRight()
{
	LightFX_ProcPaletteWipe(LEDStrip, 5, 5, 120, &Palette, 255/2/3);
}

void setup() {
	delay(2000);
	Serial.begin(9600);

	pinMode(ENABLE_5V_PIN, OUTPUT);
	digitalWrite(ENABLE_5V_PIN, HIGH);

	FastLED.addLeds<WS2812B, LED_DATA_PIN, COLOR_ORDER>(LEDStrip, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(120);
	FastLED.show();

	//Left Side
		//choose animation
		LightFX_SetFXColorWipe(&AnimationLeft, LEDStrip, 0, 5, 0, 1, 1, CRGB::Red, CRGB::Black);
		//choose timing
		LiteFXOS_SetThreadCycleArgFreq(&AnimationLeftThread, AnimationLeft.Pattern, 10);
		//provide thread with LightFX data
		LiteFXOS_SetThreadMemory(&AnimationLeftThread, &AnimationLeft, &LightFX_LoadMemory);

	//Right Side
		//provide a function wrapped in void-void format
		LiteFXOS_SetThreadCycleArgFreq(&AnimationRightThread, AnimationRight, 60);

}

void loop()
{
	bool updateLEDs;

	LiteFXOS_SetTimerCounter(millis());
	//Process left side Using LiteFXOS
	if (LiteFXOS_ProcThread(&AnimationLeftThread)) updateLEDs = 1;
	//Process right side Using LiteFXOS
	if (LiteFXOS_ProcThread(&AnimationRightThread)) updateLEDs = 1;
	if (updateLEDs) FastLED.show();
}

extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
//  int _write(){return -1;}
}
