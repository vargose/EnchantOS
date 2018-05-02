/**************************************************************************/
/*!
    @file     LawGiver.cpp
    @author   The Firebrand Forge

    @section LICENSE

    Copyright (C) 2018 The Firebrand Forge

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    @brief Judge Dredd LawGiver prop, A example of EnchantOS
*/
/**************************************************************************/
#include <Wire.h>
#include "WProgram.h"

#include <assert.h>

#include "FastLED.h"
#include "Bounce2.h"
#include "SD.h"
#include "SPI.h"
#include "Snooze.h"

extern "C" {
#include "LiteFXOS/OS.h"
}
#include "LiteFXOS/LightFX.h"
#include "LiteFXOS/LiteFX.h"

#include "Menu.h"
#include "SoundFX.h"
#include "Bluefruit.h"
#include "IMU.h"

#include "Board.h"

/*-----------------------------------------------------------------------------
  LED Strip
 *----------------------------------------------------------------------------*/
#define NUM_LEDS 		12
#define COLOR_ORDER		GRB

CRGB LawGiver[NUM_LEDS];

const uint8_t BLUE_SQUARE_START 	= 0; const uint8_t BLUE_SQUARE_LENGTH 	= 1;
const uint8_t RED_TRIANGLE_START 	= 1; const uint8_t RED_TRIANGLE_LENGTH	= 1;
const uint8_t AMMUNITION_START 		= 2; const uint8_t AMMUNITION_LENGTH 	= 5;
const uint8_t BARREL_START 			= 7; const uint8_t BARREL_LENGTH 		= 5;

/*-----------------------------------------------------------------------------
  Threads to run LED patterns - controls timing
 *----------------------------------------------------------------------------*/
// Doesnt hurt to use multiple threads, but this prop can run on 2 common thread since only 2 sections are animate at a time.
//LITE_FX_OS_THREAD_T BlueSquareThread;
LITE_FX_THREAD_T RedTriangleThread;
LITE_FX_THREAD_T AmmunitionThread;
LITE_FX_THREAD_T BarrelThread;

//LIGHT_FX_T BlueSquareFX;
LITE_FX_T RedTriangleBlinkFX;
LITE_FX_T AmmunitionRampUpFX;
LITE_FX_T AmmunitionDownFX;
LITE_FX_T BarrelRapidFireFX;
LITE_FX_T BarrelGrenadeFX;
LITE_FX_T BarrelArmorPiercingFX;
LITE_FX_T BarrelDoubleWammyFX;
LITE_FX_T BarrelFlareFX;
LITE_FX_T RainbowFX;

/*-----------------------------------------------------------------------------
  Buttons
 *----------------------------------------------------------------------------*/
Bounce ButtonRecognition;
Bounce ButtonTrigger;
Bounce ButtonMagazine;

/*-----------------------------------------------------------------------------
  Sound Files
 *----------------------------------------------------------------------------*/
#define WAV_START_RAPID_FIRE 		"/bank1/RANGE1.WAV"
#define WAV_START_GRENADE	 		"/bank1/RANGE2.WAV"
#define WAV_START_ARMOR_PIERCING	"/bank1/RANGE3.WAV"
#define WAV_START_DOUBLE_WAMMY		"/bank1/RANGE4.WAV"
#define WAV_START_FLARE	 			"/bank1/RANGE5.WAV"

#define WAV_TRIGGER_RAPID_FIRE 		"/bank1/BLASTER1.WAV"
#define WAV_TRIGGER_GRENADE	 		"/bank1/BLASTER2.WAV"
#define WAV_TRIGGER_ARMOR_PIERCING	"/bank1/BLASTER3.WAV"
#define WAV_TRIGGER_DOUBLE_WAMMY	"/bank1/BLASTER4.WAV"
#define WAV_TRIGGER_FLARE	 		"/bank1/BLASTER5.WAV"

#define WAV_JUDGE_RECOGNITION 		"/bank1/JUDGEOK.WAV"
#define WAV_RELOAD 					"/bank1/CLIPIN.WAV"
#define WAV_NO_AMMO 				"/bank1/NOCLIP.WAV"
#define WAV_FULL_AMMO				"/bank1/FULL.WAV"

#define SOUND_GROUP_FILE_COUNT_MAX			10
#define SOUND_FILENAME_LEN_MAX				64
// A section of memory to store music file names.
char FilenamesMusic[SOUND_GROUP_FILE_COUNT_MAX][SOUND_FILENAME_LEN_MAX];

const char * const SOUND_DIR_MUSIC = "/Music";

SOUND_FX_T SoundFXMusic;

CRGBPalette16 Palette = RainbowColors_p;

/*-----------------------------------------------------------------------------
  Prop Vars
 *----------------------------------------------------------------------------*/
static uint8_t RapidFireAmmo 		= 5;
static uint8_t GrenadeAmmo 			= 5;
static uint8_t ArmorPiercingAmmo 	= 5;
static uint8_t DoubleWammyAmmo 		= 5;
static uint8_t FlareAmmo 			= 5;

/*-----------------------------------------------------------------------------
  extern Menus for circular dependency
 *----------------------------------------------------------------------------*/
extern MENU_T MENU_PRERECOGNITION;
extern MENU_T MENU_RAPID_FIRE;
extern MENU_T MENU_GRENADE;
extern MENU_T MENU_ARMOR_PIERCING;
extern MENU_T MENU_DOUBLE_WAMMY;
extern MENU_T MENU_FLARE;
extern MENU_T MENU_MUSIC;

/*-----------------------------------------------------------------------------
  Common Functions
 *----------------------------------------------------------------------------*/
void Reload()
{
	SoundFX_PlayFile(WAV_RELOAD);

	RapidFireAmmo 		= 5;
	GrenadeAmmo			= 5;
	ArmorPiercingAmmo 	= 5;
	DoubleWammyAmmo 	= 5;
	FlareAmmo 			= 5;

	// Reset the animation to start at position 0;
	// AmmunitionRampUpFX.ColorWipeVars.Index = 0;
	// or change setting using set function, along with other settings
	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, 6, 10, 0);

	// Place the AmmunitionRampUpFX animation on the AmmunitionThread
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);
}

void NoAmmo()
{
	SoundFX_PlayFile(WAV_NO_AMMO);
	//RedTriangleBlinkFX.BlinkVars.Direction = 1;

	//Change the blink speed here
	LiteFX_SetBlink(&RedTriangleBlinkFX, 1, CRGB::Red, CRGB::Black, 3, 10, 0);
	LiteFX_Start(&RedTriangleThread, &RedTriangleBlinkFX, 0);
}

void FXBarrelOff(void)
{
	LightFX_SetOff(LawGiver, BARREL_START, BARREL_LENGTH);
}

/*-----------------------------------------------------------------------------
  Menu Functions
 *----------------------------------------------------------------------------*/
void BootUp()
{
	LawGiver[BLUE_SQUARE_START] = CRGB::Blue;

	// Change the blink speed here
	LiteFX_SetBlink(&RedTriangleBlinkFX, 1, CRGB::Red, CRGB::Black, 2, 2, 0);
	LiteFX_Start(&RedTriangleThread, &RedTriangleBlinkFX, 0);
}

void Recognition()
{
	SoundFX_PlayFile(WAV_JUDGE_RECOGNITION);

	RapidFireAmmo 		= 5;
	GrenadeAmmo			= 5;
	ArmorPiercingAmmo 	= 5;
	DoubleWammyAmmo 	= 5;
	FlareAmmo 			= 5;

	LawGiver[BLUE_SQUARE_START] = CRGB::Black;

	LiteFX_SetBlink(&RedTriangleBlinkFX, 1, CRGB::Red, CRGB::Black, 1, 2, 0);
	LiteFX_Start(&RedTriangleThread, &RedTriangleBlinkFX, 0);

	// start from 0, color wipe 6 times, at 10 ticks per second, no action on complete
	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, 6, 10, 0);
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);

	Menu_SetMenu(&MENU_RAPID_FIRE); // SetNextMenu does not run menu init function
}

void RapidFireStart()
{
	SoundFX_PlayFile(WAV_START_RAPID_FIRE);

	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, RapidFireAmmo + 1, 10, 0);
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);
}

void GrenadeStart()
{
	SoundFX_PlayFile(WAV_START_GRENADE);

	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, GrenadeAmmo + 1, 10, 0);
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);
}

void ArmorPiercingStart()
{
	SoundFX_PlayFile(WAV_START_ARMOR_PIERCING);

	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, ArmorPiercingAmmo + 1, 10, 0);
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);
}

void DoubleWammyStart()
{
	SoundFX_PlayFile(WAV_START_DOUBLE_WAMMY);

	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, DoubleWammyAmmo + 1, 10, 0);
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);
}

void FlareStart()
{
	SoundFX_PlayFile(WAV_START_FLARE);

	LiteFX_SetColorWipe(&AmmunitionRampUpFX, 0, true, false, CRGB::Red, CRGB::Black, FlareAmmo + 1, 10, 0);
	LiteFX_Start(&AmmunitionThread, &AmmunitionRampUpFX, 0);
}

void RapidFireTrigger()
{
	SoundFX_PlayFile(WAV_TRIGGER_RAPID_FIRE);

	if (RapidFireAmmo)
	{
		RapidFireAmmo--;
		LiteFX_SetColorWipe(&AmmunitionDownFX, RapidFireAmmo, false, false, CRGB::Red, CRGB::Black, 1, 10, 0);
		LiteFX_Start(&AmmunitionThread, &AmmunitionDownFX, 0);

		LiteFX_Start(&BarrelThread, &BarrelRapidFireFX, 0);
	}
	else
	{
		NoAmmo();
	}
}

void GrenadeTrigger()
{
	SoundFX_PlayFile(WAV_TRIGGER_GRENADE);

	if (GrenadeAmmo)
	{
		GrenadeAmmo--;
		LiteFX_SetColorWipe(&AmmunitionDownFX, GrenadeAmmo, false, false, CRGB::Red, CRGB::Black, 1, 10, 0);
		LiteFX_Start(&AmmunitionThread, &AmmunitionDownFX, 0);

		BarrelGrenadeFX.ColorFaderVars.Index = 0;
		BarrelGrenadeFX.ColorFaderVars.Direction = 1;
		LiteFX_Start(&BarrelThread, &BarrelGrenadeFX, 0);
	}
	else
	{
		NoAmmo();
	}
}

void ArmorPiercingTrigger()
{
	SoundFX_PlayFile(WAV_TRIGGER_ARMOR_PIERCING);

	if (ArmorPiercingAmmo)
	{
		ArmorPiercingAmmo--;
		LiteFX_SetColorWipe(&AmmunitionDownFX, ArmorPiercingAmmo, false, false, CRGB::Red, CRGB::Black, 1, 10, 0);
		LiteFX_Start(&AmmunitionThread, &AmmunitionDownFX, 0);

		BarrelArmorPiercingFX.ColorFaderVars.Index = 0;
		BarrelArmorPiercingFX.ColorFaderVars.Direction = 1;
		LiteFX_Start(&BarrelThread, &BarrelArmorPiercingFX, 0);
	}
	else
	{
		NoAmmo();
	}
}

void DoubleWammyTrigger()
{
	SoundFX_PlayFile(WAV_TRIGGER_DOUBLE_WAMMY);

	if (DoubleWammyAmmo)
	{
		DoubleWammyAmmo--;
		LiteFX_SetColorWipe(&AmmunitionDownFX, DoubleWammyAmmo, false, false, CRGB::Red, CRGB::Black, 1, 10, 0);
		LiteFX_Start(&AmmunitionThread, &AmmunitionDownFX, 0);

		BarrelDoubleWammyFX.ColorFaderVars.Index = 0;
		BarrelDoubleWammyFX.ColorFaderVars.Direction = 1;
		LiteFX_Start(&BarrelThread, &BarrelDoubleWammyFX, 0);
	}
	else
	{
		NoAmmo();
	}
}

void FlareTrigger()
{
	SoundFX_PlayFile(WAV_TRIGGER_FLARE);

	if (FlareAmmo)
	{
		FlareAmmo--;
		LiteFX_SetColorWipe(&AmmunitionDownFX, FlareAmmo, false, false, CRGB::Red, CRGB::Black, 1, 10, 0);
		LiteFX_Start(&AmmunitionThread, &AmmunitionDownFX, 0);

		BarrelFlareFX.ColorFaderVars.Index = 0;
		BarrelFlareFX.ColorFaderVars.Direction = 1;
		LiteFX_Start(&BarrelThread, &BarrelFlareFX, 0);
	}
	else
	{
		NoAmmo();
	}
}

void StartMusicMenu()
{
	SoundFX_PlayFX(&SoundFXMusic);
	LiteFX_Start(&RedTriangleThread, &RainbowFX, 0);
	Menu_StartMenu(&MENU_MUSIC);
}

void EndMusicMenu()
{
	SoundFX_Stop();
	LightFX_SetOff(LawGiver, 0, NUM_LEDS);
	Menu_StartMenu(&MENU_PRERECOGNITION);
}

void NextSong()
{
	SoundFX_PlayNext(&SoundFXMusic);
}

/*-----------------------------------------------------------------------------
  Menus
 *----------------------------------------------------------------------------*/
// A Menu is a mode/state.
// Build a Menu for each mode with different functions mapped to a button/event.
/*----------------------------------------------------------------------------*/
enum FUNCTION_MAP_TRANSLATION
{
	BUTTON_FUNCTION_RECOGNITION,
	BUTTON_FUNCTION_TRIGGER,
	BUTTON_FUNCTION_MAGAZINE,
	BUTTON_FUNCTION_MUSIC,
};

MENU_T MENU_PRERECOGNITION =
{
	.NextMenu = &MENU_RAPID_FIRE,
	.PrevMenu = 0,
	.InitFunction = BootUp,
	.FunctionMap = // complier does not allow array indexing within struct, manually order to match translation map
	{
		Recognition,		//.FunctionMap[BUTTON_FUNCTION_RECOGNITION]
		0,					//.FunctionMap[BUTTON_FUNCTION_TRIGGER]
		0,					//.FunctionMap[BUTTON_FUNCTION_MAGAZINE]
		StartMusicMenu,		//.FunctionMap[MENU_START_MUSIC]
	},
};

MENU_T MENU_RAPID_FIRE =
{
	.NextMenu = &MENU_GRENADE,
	.PrevMenu = 0,
	.InitFunction = RapidFireStart,
	.FunctionMap =
	{
		Menu_StartNextMenu,		//.FunctionMap[BUTTON_FUNCTION_RECOGNITION] = Menu_SetNextMenu,
		RapidFireTrigger,		//.FunctionMap[BUTTON_FUNCTION_TRIGGER] 	= RapidFireTrigger,
		Reload,					//.FunctionMap[BUTTON_FUNCTION_MAGAZINE] 	= MagazineCommon,
		StartMusicMenu,			//.FunctionMap[MENU_START_MUSIC]
	},
};

MENU_T MENU_GRENADE =
{
	.NextMenu = &MENU_ARMOR_PIERCING,
	.PrevMenu = 0,
	.InitFunction = GrenadeStart,
	.FunctionMap =
	{
		Menu_StartNextMenu,
		GrenadeTrigger,
		Reload,
		StartMusicMenu,
	},
};

MENU_T MENU_ARMOR_PIERCING =
{
	.NextMenu = &MENU_DOUBLE_WAMMY,
	.PrevMenu = 0,
	.InitFunction = ArmorPiercingStart,
	.FunctionMap =
	{
		Menu_StartNextMenu,
		ArmorPiercingTrigger,
		Reload,
		StartMusicMenu,
	},
};

MENU_T MENU_DOUBLE_WAMMY =
{
	.NextMenu = &MENU_FLARE,
	.PrevMenu = 0,
	.InitFunction = DoubleWammyStart,
	.FunctionMap =
	{
		Menu_StartNextMenu,
		DoubleWammyTrigger,
		Reload,
		StartMusicMenu,
	},
};

MENU_T MENU_FLARE =
{
	.NextMenu = &MENU_RAPID_FIRE,
	.PrevMenu = 0,
	.InitFunction = FlareStart,
	.FunctionMap =
	{
		Menu_StartNextMenu,
		FlareTrigger,
		Reload,
		StartMusicMenu,
	},
};

MENU_T MENU_MUSIC =
{
	.NextMenu = 0,
	.PrevMenu = 0,
	.InitFunction = 0,
	.FunctionMap =
	{
		0,
		NextSong,
		0,
		EndMusicMenu,
	},
};

void setup()
{
	delay(500); 						// sanity delay
	Serial.begin(9600); 				// USB is always 12 Mbit/sec

	pinMode(ENABLE_5V_PIN, OUTPUT);
	digitalWrite(ENABLE_5V_PIN, HIGH);  // 5V enable

	FastLED.addLeds<WS2812B, LED_DATA_PIN1, COLOR_ORDER>(LawGiver, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(0);
	FastLED.show();

	 SPI.setMOSI(SDCARD_MOSI_PIN);
	 SPI.setMISO(SDCARD_MISO_PIN);
	 SPI.setSCK(SDCARD_SCK_PIN);

	if (SD.begin(SDCARD_CS_PIN))
	{
		SoundFX_Init(ENABLE_AMP_PIN);
		SoundFX_InitFXWithDirectory(&SoundFXMusic, (char **)FilenamesMusic, SOUND_FILENAME_LEN_MAX, SOUND_GROUP_FILE_COUNT_MAX, SOUND_DIR_MUSIC);
		debug_SoundFX_PrintFilenames((char **)FilenamesMusic, SOUND_GROUP_FILE_COUNT_MAX, SOUND_FILENAME_LEN_MAX);
	}
	else
	{
		Serial.println("Unable to access the SD card");
	}

	pinMode(13,	 INPUT_PULLUP);
	pinMode(18,  INPUT_PULLUP);
	pinMode(19,	 INPUT_PULLUP);

	ButtonRecognition	= Bounce(13, 10);  // 10 ms debounce
	ButtonTrigger		= Bounce(18, 10);  // 10 ms debounce
	ButtonMagazine		= Bounce(19, 10);  // 10 ms debounce

	//Bluefruit_Init();

	IMU_Init();

	// Set animation behaviors
	// The AmmunitionRampUpFX is configured as a ColorWipe
	// Set to tick 6 times. 1st tick turning 0 LEDs on
	// Set for animation "speed" of 10 tick per second
	// No action required on completion
	LiteFX_InitColorWipe(&AmmunitionRampUpFX, 		LawGiver, AMMUNITION_START, AMMUNITION_LENGTH,	0, true, 	false, CRGB::Red, CRGB::Black, 6, 10, 0);
	LiteFX_InitColorWipe(&AmmunitionDownFX, 		LawGiver, AMMUNITION_START, AMMUNITION_LENGTH, 	6, false, 	false, CRGB::Red, CRGB::Black, 1, 10, 0);

	// The BarrelRapidFireFX is configured as a ColorFader
	// Set to tick 72 times at 60 tick per second, for a total time of 1.2 seconds
	// Make sure the barrel LEDs are off when finished
	LiteFX_InitColorFader(&BarrelRapidFireFX, 		LawGiver, BARREL_START, 	BARREL_LENGTH,		0, true, 	4, 	false, CRGB::Black, CRGB::White, 72, 60, FXBarrelOff);
	LiteFX_InitColorFader(&BarrelGrenadeFX, 		LawGiver, BARREL_START, 	BARREL_LENGTH, 		0, true, 	30, false, CRGB::Black, CRGB::White, 30, 60, FXBarrelOff);
	LiteFX_InitColorFader(&BarrelArmorPiercingFX,	LawGiver, BARREL_START, 	BARREL_LENGTH, 		0, true, 	30, true,  CRGB::Black, CRGB::White, 60, 120, FXBarrelOff);

	// The ColorFader pattern is set for 0 brightness to full brightness in 60 ticks.
	// We want the to reset the pattern to start at 0 brightness, so we set the BoundaryBehavior to false.
	// We want a total time of .8 seconds. We select 120 ticks to complete 2 cycles of 0 to full brightness.
	// 120 / .8 = 150 ticks per second.
	LiteFX_InitColorFader(&BarrelDoubleWammyFX, 	LawGiver, BARREL_START, 	BARREL_LENGTH, 		0, true, 	60, false, CRGB::Black, CRGB::White, 120, 150, FXBarrelOff);
	LiteFX_InitColorFader(&BarrelFlareFX, 			LawGiver, BARREL_START, 	BARREL_LENGTH, 		0, true, 	30, true,  CRGB::Black, CRGB::Red, 	 60, 50, FXBarrelOff);

	LiteFX_InitBlink(&RedTriangleBlinkFX, LawGiver, RED_TRIANGLE_START, RED_TRIANGLE_LENGTH, 1, CRGB::Red, CRGB::Black, 3, 10, 0);
	LiteFX_InitPaletteWipe(&RainbowFX, LawGiver, 0, NUM_LEDS, 0, true, 120, 0, &Palette, 255*2/3, 0, 60, 0);

	Menu_StartMenu(&MENU_PRERECOGNITION);

	Serial.println("End Setup");
}

void loop()
{
	static bool updateLEDs;

	// process LightFX threads
	if (LiteFX_ProcThread(&RedTriangleThread))		updateLEDs = 1;
	if (LiteFX_ProcThread(&AmmunitionThread))		updateLEDs = 1;
	if (LiteFX_ProcThread(&BarrelThread))			updateLEDs = 1;
	if (updateLEDs) 								{updateLEDs = 0; FastLED.show();}

	// Check buttons
	ButtonRecognition.update();
	ButtonTrigger.update();
	ButtonMagazine.update();
	if(ButtonRecognition.fell()) 	{Menu_DoFunction(BUTTON_FUNCTION_RECOGNITION);}
	if(ButtonTrigger.fell()) 		{Menu_DoFunction(BUTTON_FUNCTION_TRIGGER);}
	if(ButtonMagazine.fell()) 		{Menu_DoFunction(BUTTON_FUNCTION_MAGAZINE);}
	if(!ButtonRecognition.read() && ButtonTrigger.fell())	{Menu_DoFunction(BUTTON_FUNCTION_MUSIC);}

	// Check Bluetooth
	Bluefruit_Poll();

	if(Bluefruit_GetUpdateFlag())
	{
		Bluefruit_ResetUpdateFlag();
		//Bluefruit_GetColorR(), Bluefruit_GetColorG(), Bluefruit_GetColorB()
		if(Bluefruit_GetButtonUp())		{}
		if(Bluefruit_GetButtonDown())	{}
		if(Bluefruit_GetButtonLeft())	{}
		if(Bluefruit_GetButtonRight())	{}

		if(Bluefruit_GetButton1())		{Menu_DoFunction(BUTTON_FUNCTION_RECOGNITION);}
		if(Bluefruit_GetButton2())		{Menu_DoFunction(BUTTON_FUNCTION_TRIGGER);}
		if(Bluefruit_GetButton3())		{Menu_DoFunction(BUTTON_FUNCTION_MAGAZINE);}
		if(Bluefruit_GetButton4())		{Menu_DoFunction(BUTTON_FUNCTION_MUSIC);}
	}

	if (Serial.available())
	{
		char ch = Serial.read();
		if (ch == '1') {Menu_DoFunction(BUTTON_FUNCTION_RECOGNITION);}
		if (ch == '2') {Menu_DoFunction(BUTTON_FUNCTION_TRIGGER);}
		if (ch == '3') {Menu_DoFunction(BUTTON_FUNCTION_MAGAZINE);}
		if (ch == '4') {Menu_DoFunction(BUTTON_FUNCTION_MUSIC);}
		if (ch == 'e')
		{
			Serial.println("echo");
		}
		if (ch == 'b')
		{
			//Serial.println(BatteryGetRaw());
			//IMU_PrintSerial();
		}
	}
}

extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
//  int _write(){return -1;}
}
