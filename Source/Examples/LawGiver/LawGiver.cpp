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

#include "Menu.h"
#include "SD.h"
#include "SoundFX.h"

#include "LiteFXOS/LightFX.h"
#include "LiteFXOS/LiteFXOS.h"

#include "Boards/EnchantPins.h"

/*-----------------------------------------------------------------------------
  LED Strip
 *----------------------------------------------------------------------------*/
#define NUM_LEDS 		10
#define COLOR_ORDER		GRB

CRGB LawGiver[NUM_LEDS];

const uint8_t BLUE_SQUARE_START 	= 0; const uint8_t BLUE_SQUARE_LENGTH 	= 1;
const uint8_t RED_TRIANGLE_START 	= 1; const uint8_t RED_TRIANGLE_LENGTH	= 1;
const uint8_t AMMUNITION_START 		= 2; const uint8_t AMMUNITION_LENGTH 	= 5;
const uint8_t BARREL_START 			= 7; const uint8_t BARREL_LENGTH 		= 3;

/*-----------------------------------------------------------------------------
  Threads to run LED patterns - controls timing
 *----------------------------------------------------------------------------*/
// Doesnt hurt to use multiple threads, but this prop can run on 2 common thread since only 2 sections are animate at a time.
//LITE_FX_OS_THREAD_T BlueSquareThread;
LITE_FX_OS_THREAD_T RedTriangleThread;
LITE_FX_OS_THREAD_T AmmunitionThread;
LITE_FX_OS_THREAD_T BarrelThread;

//LIGHT_FX_T BlueSquareFX;
LIGHT_FX_T RedTriangleBlinkFX;
LIGHT_FX_T AmmunitionRampUpFX;
LIGHT_FX_T AmmunitionDownFX;
LIGHT_FX_T BarrelRapidFireFX;
LIGHT_FX_T BarrelGrenadeFX;
LIGHT_FX_T BarrelArmorPiercingFX;
LIGHT_FX_T BarrelDoubleWammyFX;
LIGHT_FX_T BarrelFlareFX;
LIGHT_FX_T RainbowFX;

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
//static bool JudgeRecognition = 1; // one time power on flag
static uint8_t RapidFireAmmo 		= 5;
static uint8_t GrenadeAmmo 			= 5;
static uint8_t ArmorPiercingAmmo 	= 5;
static uint8_t DoubleWammyAmmo 		= 5;
static uint8_t FlareAmmo 			= 5;


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

	// Reset the animation at position 0;
	AmmunitionRampUpFX.IndexPos = 0;

	// Place the AmmunitionRampUpFX animation on the AmmunitionThread
	// The AmmunitionRampUpFX is configured using ColorWipe in setup()
	// Tick 6 times, 1st tick turning 0 LEDs one
	// Set for animation "speed" of 10 tick per second
	// No action required on completion
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, 6, 10, 0);

	// Provide the animation data to the thread
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);
}

void NoAmmo()
{
	SoundFX_PlayFile(WAV_NO_AMMO);
	RedTriangleBlinkFX.Direction = 1;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&RedTriangleThread, RedTriangleBlinkFX.Pattern, 3, 10, 0);
	LiteFXOS_SetThreadMemory(&RedTriangleThread, &RedTriangleBlinkFX, &LightFX_LoadMemory);
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
	RedTriangleBlinkFX.Direction = 1;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&RedTriangleThread, RedTriangleBlinkFX.Pattern, 2, 2, 0);
	LiteFXOS_SetThreadMemory(&RedTriangleThread, &RedTriangleBlinkFX, &LightFX_LoadMemory);
}

void Recognition()
{
	SoundFX_PlayFile(WAV_JUDGE_RECOGNITION);

	LawGiver[BLUE_SQUARE_START] = CRGB::Black;

	LiteFXOS_SetThreadMomentaryArgTicksFreq(&RedTriangleThread, RedTriangleBlinkFX.Pattern, 1, 2, 0);

	AmmunitionRampUpFX.IndexPos = 0;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, 6, 10,  0);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);

	Menu_SetNextMenu(); // SetNextMenu does not run menu init function
}

void RapidFireStart()
{
	SoundFX_PlayFile(WAV_START_RAPID_FIRE);

	AmmunitionRampUpFX.IndexPos = 0;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, RapidFireAmmo + 1, 10, 0);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);
}

void GrenadeStart()
{
	SoundFX_PlayFile(WAV_START_GRENADE);

	AmmunitionRampUpFX.IndexPos = 0;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, GrenadeAmmo + 1, 10, 0);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);
}

void ArmorPiercingStart()
{
	SoundFX_PlayFile(WAV_START_ARMOR_PIERCING);

	AmmunitionRampUpFX.IndexPos = 0;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, ArmorPiercingAmmo + 1, 10, 0);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);
}

void DoubleWammyStart()
{
	SoundFX_PlayFile(WAV_START_DOUBLE_WAMMY);

	AmmunitionRampUpFX.IndexPos = 0;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, DoubleWammyAmmo + 1, 10, 0);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);
}

void FlareStart()
{
	SoundFX_PlayFile(WAV_START_FLARE);

	AmmunitionRampUpFX.IndexPos = 0;
	LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionRampUpFX.Pattern, FlareAmmo + 1, 10, 0);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionRampUpFX, &LightFX_LoadMemory);
}

void RapidFireTrigger()
{
	SoundFX_PlayFile(WAV_TRIGGER_RAPID_FIRE);

	if (RapidFireAmmo)
	{
		RapidFireAmmo--;
		AmmunitionDownFX.IndexPos = RapidFireAmmo;
		LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionDownFX.Pattern, 1, 10, 0);
		LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionDownFX, &LightFX_LoadMemory);

		LiteFXOS_SetThreadMomentaryArgTimeFreq(&BarrelThread, BarrelRapidFireFX.Pattern, 1200, 60, FXBarrelOff);
		LiteFXOS_SetThreadMemory(&BarrelThread, &BarrelRapidFireFX, &LightFX_LoadMemory);
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
		AmmunitionDownFX.IndexPos = GrenadeAmmo;
		LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionDownFX.Pattern, 1, 10, 0);
		LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionDownFX, &LightFX_LoadMemory);

		BarrelGrenadeFX.IndexPos = 0;
		BarrelGrenadeFX.Direction = 1;
		LiteFXOS_SetThreadMomentaryArgTicksTime(&BarrelThread, BarrelGrenadeFX.Pattern, 30, 500, FXBarrelOff);
		LiteFXOS_SetThreadMemory(&BarrelThread, &BarrelGrenadeFX, &LightFX_LoadMemory);
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
		AmmunitionDownFX.IndexPos = ArmorPiercingAmmo;
		LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionDownFX.Pattern, 1, 10, 0);
		LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionDownFX, &LightFX_LoadMemory);

		BarrelArmorPiercingFX.IndexPos = 0;
		BarrelArmorPiercingFX.Direction = 1;
		LiteFXOS_SetThreadMomentaryArgTicksTime(&BarrelThread, BarrelArmorPiercingFX.Pattern, 60, 500, FXBarrelOff);
		LiteFXOS_SetThreadMemory(&BarrelThread, &BarrelArmorPiercingFX, &LightFX_LoadMemory);
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
		AmmunitionDownFX.IndexPos = DoubleWammyAmmo;
		LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionDownFX.Pattern, 1, 10, 0);
		LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionDownFX, &LightFX_LoadMemory);

		BarrelDoubleWammyFX.IndexPos = 0;
		BarrelDoubleWammyFX.Direction = 1;
		LiteFXOS_SetThreadMomentaryArgTicksTime(&BarrelThread, BarrelDoubleWammyFX.Pattern, 120, 800, FXBarrelOff);
		LiteFXOS_SetThreadMemory(&BarrelThread, &BarrelDoubleWammyFX, &LightFX_LoadMemory);
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
		AmmunitionDownFX.IndexPos = FlareAmmo;
		LiteFXOS_SetThreadMomentaryArgTicksFreq(&AmmunitionThread, AmmunitionDownFX.Pattern, 1, 10, 0);
		LiteFXOS_SetThreadMemory(&AmmunitionThread, &AmmunitionDownFX, &LightFX_LoadMemory);

		BarrelFlareFX.IndexPos = 0;
		BarrelFlareFX.Direction = 1;
		LiteFXOS_SetThreadMomentaryArgTicksTime(&BarrelThread, BarrelFlareFX.Pattern, 60, 1200, FXBarrelOff);
		LiteFXOS_SetThreadMemory(&BarrelThread, &BarrelFlareFX, &LightFX_LoadMemory);
	}
	else
	{
		NoAmmo();
	}
}

/*-----------------------------------------------------------------------------
  Menus
 *----------------------------------------------------------------------------*/
// A Menu is a mode. Build a Menu for each mode with different functions mapped to a button/event.
/*----------------------------------------------------------------------------*/
enum FUNCTION_MAP_TRANSLATION
{
	BUTTON_FUNCTION_RECOGNITION,
	BUTTON_FUNCTION_TRIGGER,
	BUTTON_FUNCTION_MAGAZINE,
};

// We will skip making a header file.
extern MENU_T MENU_RAPID_FIRE;
extern MENU_T MENU_GRENADE;
extern MENU_T MENU_ARMOR_PIERCING;
extern MENU_T MENU_DOUBLE_WAMMY;
extern MENU_T MENU_FLARE;

MENU_T MENU_JUDGE_RECOGNITION =
{
	.NextMenu = &MENU_RAPID_FIRE,
	.PrevMenu = 0,
	.InitFunction = BootUp,
	.FunctionMap = // complier does not allow array indexing within struct, manually order to match translation map
	{
		Recognition,	//.FunctionMap[BUTTON_FUNCTION_RECOGNITION] = ,
		0,				//.FunctionMap[BUTTON_FUNCTION_TRIGGER] 	= ,
		0,				//.FunctionMap[BUTTON_FUNCTION_MAGAZINE] 	= ,
	},
};

MENU_T MENU_RAPID_FIRE =
{
	.NextMenu = &MENU_GRENADE,
	.PrevMenu = 0,
	.InitFunction = RapidFireStart,
	.FunctionMap =
	{
		Menu_StartNextMenu,	//.FunctionMap[BUTTON_FUNCTION_RECOGNITION] = Menu_SetNextMenu,
		RapidFireTrigger,	//.FunctionMap[BUTTON_FUNCTION_TRIGGER] 	= RapidFireTrigger,
		Reload,				//.FunctionMap[BUTTON_FUNCTION_MAGAZINE] 	= MagazineCommon,
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
	},
};

void NextSong()
{
	SoundFX_PlayNext(&SoundFXMusic);
}

void StartMusic()
{
	SoundFX_PlayFX(&SoundFXMusic);

	LiteFXOS_SetThreadCycleArgFreq(&AmmunitionThread, RainbowFX.Pattern, 60);
	LiteFXOS_SetThreadMemory(&AmmunitionThread, &RainbowFX, &LightFX_LoadMemory);
}

MENU_T MENU_MUSIC =
{
	.NextMenu = 0,
	.PrevMenu = 0,
	.InitFunction = StartMusic,
	.FunctionMap =
	{
		Menu_StartNextMenu,
		NextSong,
		0,
	},
};

void setup()
{
	delay(500); 			// sanity delay
	Serial.begin(9600); 	// USB is always 12 Mbit/sec

	pinMode(ENABLE_5V_PIN, OUTPUT);
	digitalWrite(ENABLE_5V_PIN, HIGH);  // 5V enable

	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH); 	// turn on the amplifier

	FastLED.addLeds<WS2812B, LED_DATA_PIN, COLOR_ORDER>(LawGiver, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.showColor(0);
	FastLED.show();

	if (SD.begin(SDCARD_CS_PIN))
	{
		SoundFX_Init();
		SoundFX_InitFXWithDirectory(&SoundFXMusic, (char **)FilenamesMusic, SOUND_FILENAME_LEN_MAX, SOUND_GROUP_FILE_COUNT_MAX, SOUND_DIR_MUSIC);
	}
	else
	{
		Serial.println("Unable to access the SD card");
	}

	pinMode(18,	 INPUT_PULLUP);
	pinMode(19,  INPUT_PULLUP);
	pinMode(20,	 INPUT_PULLUP);

	ButtonRecognition	= Bounce(18, 10);  // 10 ms debounce
	ButtonTrigger		= Bounce(19, 10);  // 10 ms debounce
	ButtonMagazine		= Bounce(20, 10);  // 10 ms debounce

	LightFX_SetFXPaletteWipe(&RainbowFX, 			LawGiver, 0, NUM_LEDS, 120, &Palette, 255*2/3);
	LightFX_SetFXBlink(&RedTriangleBlinkFX, 		LawGiver, RED_TRIANGLE_START, RED_TRIANGLE_LENGTH, 1, CRGB::Red, CRGB::Black);
	LightFX_SetFXColorWipe(&AmmunitionRampUpFX, 	LawGiver, AMMUNITION_START, AMMUNITION_LENGTH, 0, 1, false, CRGB::Red, CRGB::Black);
	LightFX_SetFXColorWipe(&AmmunitionDownFX, 		LawGiver, AMMUNITION_START, AMMUNITION_LENGTH, 6, 0, false, CRGB::Red, CRGB::Black);
	LightFX_SetFXColorFader(&BarrelRapidFireFX, 	LawGiver, BARREL_START, BARREL_LENGTH, 0, 1, 4,  false, CRGB::Black, CRGB::White);
	LightFX_SetFXColorFader(&BarrelGrenadeFX, 		LawGiver, BARREL_START, BARREL_LENGTH, 0, 1, 30, false, CRGB::Black, CRGB::White);
	LightFX_SetFXColorFader(&BarrelArmorPiercingFX, LawGiver, BARREL_START, BARREL_LENGTH, 0, 1, 30, true,  CRGB::Black, CRGB::White);
	LightFX_SetFXColorFader(&BarrelDoubleWammyFX, 	LawGiver, BARREL_START, BARREL_LENGTH, 0, 1, 60, false, CRGB::Black, CRGB::White);
	LightFX_SetFXColorFader(&BarrelFlareFX, 		LawGiver, BARREL_START, BARREL_LENGTH, 0, 1, 30, true,  CRGB::Black, CRGB::Red);

	Menu_StartMenu(&MENU_JUDGE_RECOGNITION);

	Serial.println("End Setup");
}

void loop()
{
	static bool updateLEDs;

	LiteFXOS_SetTimerCounter(millis());

	if (LiteFXOS_ProcThread(&RedTriangleThread))	updateLEDs = 1;
	if (LiteFXOS_ProcThread(&AmmunitionThread))		updateLEDs = 1;
	if (LiteFXOS_ProcThread(&BarrelThread))			updateLEDs = 1;
	if (updateLEDs) 								{updateLEDs = 0; FastLED.show();}

	ButtonRecognition.update();
	ButtonTrigger.update();
	ButtonMagazine.update();
	if(ButtonRecognition.fell()) 	{Menu_DoFunction(BUTTON_FUNCTION_RECOGNITION);}
	if(ButtonTrigger.fell()) 		{Menu_DoFunction(BUTTON_FUNCTION_TRIGGER);}
	if(ButtonMagazine.fell()) 		{Menu_DoFunction(BUTTON_FUNCTION_MAGAZINE);}

	if (Serial.available())
	{
		char ch = Serial.read();
		if (ch == '1')	{Menu_DoFunction(BUTTON_FUNCTION_RECOGNITION);}
		if (ch == '2')	{Menu_DoFunction(BUTTON_FUNCTION_TRIGGER);}
		if (ch == '3')	{Menu_DoFunction(BUTTON_FUNCTION_MAGAZINE);}
		if (ch == '4')
		{
			if (Menu_GetMenu() == &MENU_MUSIC)
			{
				LightFX_SetOff(LawGiver, 0, NUM_LEDS);
				LiteFXOS_SetThreadCycleArgFreq(&AmmunitionThread, 0, 60); //stop thread music was set on
				SoundFX_Stop();
				Menu_StartMenu(&MENU_JUDGE_RECOGNITION);
			}
			else
			{
				Menu_StartMenu(&MENU_MUSIC);
			}
		}
		if (ch == 'e')
		{
			Serial.println("echo");
			debug_SoundFX_PrintFilenames((char **)FilenamesMusic, SOUND_GROUP_FILE_COUNT_MAX, SOUND_FILENAME_LEN_MAX);
		}
	}
}

extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
//  int _write(){return -1;}
}
