#include "LiteFXOS.h"

#define ENABLE_5V_PIN	8
#define LED_DATA_PIN	10

#define NUM_LEDS 		15
#define COLOR_ORDER		GRB

CRGB LEDStrip[NUM_LEDS];

CRGBPalette16 Palette1 = RainbowColors_p;
CRGBPalette16 Palette2 = ForestColors_p;

// Method 1
LIGHT_FX_T AnimationLeft; 					// Contains the animation
LITE_FX_OS_THREAD_T AnimationLeftThread;	// Each animation running simultaneously needs to be placed onto a thread.
FX_PALETTE_WIPE_VARS_T AnimationLeftVars;	// The user must provide a variables struct to store state and settings.

// Method 2 - This is the most streamlined way.
LITE_FX_T			AnimationRight;			// This will allow you to set the FX timing variables in line with the FX
LITE_FX_THREAD_T 	AnimationRightThread; 	// Wrapper from LiteFX module

// A Momentary FX
LITE_FX_T			AnimationMiddle;		// This will allow you to set the FX timing variables in line with the FX
LITE_FX_THREAD_T 	AnimationMiddleThread; 	// Wrapper from LiteFX module

void setup() {
	delay(2000);
	Serial.begin(9600);

	// for Enchant board only
	pinMode(ENABLE_5V_PIN, OUTPUT);
	digitalWrite(ENABLE_5V_PIN, HIGH);

	FastLED.addLeds<WS2812B, LED_DATA_PIN, COLOR_ORDER>(LEDStrip, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(120);
	FastLED.show();

	// Method 1
	// Configure animation
	// LightFX_InitFXPaletteWipe(LIGHT_FX_T * fx, FX_PALETTE_WIPE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, CRGBPalette16 * palette, uint8_t visibleSpectrum)
	LightFX_InitFXPaletteWipe(&AnimationLeft, &AnimationLeftVars, LEDStrip, 0, 5, 0, 1, 120, 0, &Palette1, 255*2/3);
	// Configure and start thread running the animation. This is often in a different part of the program.
	LiteFXOS_StartThreadCycleArgFreq(&AnimationLeftThread, &AnimationLeft, AnimationLeft.LoadFX, AnimationLeft.Pattern, 60);

	// Method 2
	//Configure animation with timing
	//LiteFX_InitPaletteWipe(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, CRGBPalette16 * palette, uint8_t visibleSpectrum, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
	LiteFX_InitPaletteWipe(&AnimationRight, LEDStrip, 10, 5, 0, 0, 120, 0, &Palette1, 255*2/3, 0, 60, 0);
	// This can be called in another part of the program to start the animation
	LiteFX_Start(&AnimationRightThread, &AnimationRight, 0);

	// A Momentary FX
	//Configure animation with timing
	//LiteFX_InitColorWipe(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
	LiteFX_InitColorWipe(&AnimationMiddle, LEDStrip, 5, 5, 0, true, false, CRGB::Gold, CRGB::Black, 100, 10, LightFX_SetContextOff);
	// This can be called in another part of the program to start the animation
	LiteFX_Start(&AnimationMiddleThread, &AnimationMiddle, 0);
}

void loop()
{
	bool updateLEDs;

	//Process left Using LiteFXOS Threads
	LiteFXOS_SetTimerCounter(millis());
	if (LiteFXOS_ProcThread(&AnimationLeftThread)) updateLEDs = 1;

	//Process middle and right Using LiteFX Threads
	if (LiteFX_ProcThread(&AnimationRightThread)) updateLEDs = 1;
	if (LiteFX_ProcThread(&AnimationMiddleThread)) updateLEDs = 1;

	if (updateLEDs) FastLED.show();
}
