/*
  Pixie reads data in at 115.2k serial, 8N1.
  Byte order is R1, G1, B1, R2, G2, B2, ... where the first triplet is the
  color of the LED that's closest to the controller. 1ms of silence triggers
  latch. 2 seconds silence (or overheating) triggers LED off (for safety).

  Do not look into Pixie with remaining eye!
*/

#include "SoftwareSerial.h"
#include "Adafruit_Pixie.h"

#include "LiteFXOS.h"

#define BUTTON_PIN 	0

#define LED_STRIP_DATA_PIN 		10
#define LED_STRIP_TYPE    		WS2812B
#define LED_STRIP_COLOR_ORDER 	GRB
#define LED_STRIP_NUM_LEDS    	5

#define NUMPIXELS 		3 // Number of Pixies in the strip
#define PIXIEPIN  		10 // Pin number for SoftwareSerial output

#define TOTAL_LEDS    	LED_STRIP_NUM_LEDS + NUMPIXELS
#define PIXIE_START  	LED_STRIP_NUM_LEDS

SoftwareSerial pixieSerial(-1, PIXIEPIN);

Adafruit_Pixie Pixie = Adafruit_Pixie(NUMPIXELS, &pixieSerial);
// Alternately, can use a hardware serial port for output, e.g.:
// Adafruit_Pixie strip = Adafruit_Pixie(NUMPIXELS, &Serial1);



CRGB LEDImage[TOTAL_LEDS];
CRGB LEDImageBuffer[TOTAL_LEDS];

LITE_FX_THREAD_T Thread;


static void LightFXNext(void)
{
	LiteFX_Start(&Thread, LiteFXTable_GetNext(&LiteFXTable_Default), 0);
}


void setup() {
	Serial.begin(9600);

	pinMode(8,  INPUT_PULLUP);
	digitalWrite(8, HIGH);

	Serial.println("Ready to Pixie!");

	pixieSerial.begin(115200); // Pixie REQUIRES this baud rate
	// Serial1.begin(115200);  // <- Alt. if using hardware serial port

	Pixie.setBrightness(200);  // Adjust as necessary to avoid blinding

	FastLED.addLeds<LED_STRIP_TYPE, LED_STRIP_DATA_PIN, LED_STRIP_COLOR_ORDER>(LEDImage, LED_STRIP_NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(0);
	FastLED.show();

	LiteFX_InitDefault(LEDImage, 0, TOTAL_LEDS, LEDImageBuffer);
}

void loop()
{
	if (LiteFX_ProcThread(&Thread))
	{
		//FastLED.show();

		for(uint8_t i=0; i< NUMPIXELS; i++)
			Pixie.setPixelColor(i, LEDImage[PIXIE_START+i].r, LEDImage[PIXIE_START+i].g, LEDImage[PIXIE_START+i].b);
		Pixie.show();
	}

//	Button1.update();
//
//	if(Button1.fell()) 	{LightFXNext();}

	if (Serial.available())
	{
		char ch = Serial.read();
		if (ch == '1')	{LightFXNext();}
		if (ch == 'e')	{Serial.println("echo");}
	}
}


