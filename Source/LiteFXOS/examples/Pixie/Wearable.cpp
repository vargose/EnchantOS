#include "Bounce2.h"

#include "LiteFXOS.h"

#define BUTTON_PIN 	0

#define LED_DATA_PIN 	10
#define LED_TYPE    	WS2812B
#define COLOR_ORDER 	GRB
#define NUM_LEDS    	5

CRGB LEDStrip[NUM_LEDS];
CRGB LEDStripBuffer[NUM_LEDS];

LITE_FX_THREAD_T Thread;

Bounce Button1 	= Bounce(BUTTON_PIN, 	10);  // 10 ms debounce

static void LightFXNext(void)
{
	LiteFX_Start(&Thread, LiteFXTable_GetNext(&LiteFXTable_Default), 0);
}

void setup() {
	delay(2000); // sanity delay

	Serial.begin(9600); // USB is always 12 Mbit/sec

	// Enable 5V on Enchant board
	pinMode(8,  INPUT_PULLUP);
	digitalWrite(8, HIGH);

  	pinMode(BUTTON_PIN,	INPUT_PULLUP);

  	Button1 	= Bounce(BUTTON_PIN,	10);  // 10 ms debounce

	FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(LEDStrip, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(0);
	FastLED.show();

	LiteFX_InitDefault(LEDStrip, 0, NUM_LEDS, LEDStripBuffer);
}

void loop()
{
	if (LiteFX_ProcThread(&Thread))
		FastLED.show();

	Button1.update();

	if(Button1.fell()) 	{LightFXNext();}

	if (Serial.available())
	{
		char ch = Serial.read();
		if (ch == '1')	{LightFXNext();}
	}
}
