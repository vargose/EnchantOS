/**************************************************************************/
/*!
    @file     Lightsaber.cpp
    @author   The Firebrand Forge

    @section LICENSE

    Copyright (C) 2018 The Firebrand Forge Inc

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

    @brief Main Lightsaber program
*/
/**************************************************************************/

#include "WProgram.h"

#include <stdint.h>
#include <stdbool.h>

#include "SD.h"
#include "Bounce2.h"
#include "Snooze.h"
#include "src/SparkFunLSM6DS3.h"
#include "minIni.h"
#include "src/OneButton.h"

#include "LiteFXOS.h"
#include "Menu.h"
#include "SoundFX.h"
#include "Bluefruit.h"
#include "IMU.h"
#include "Board.h"

/*-----------------------------------------------------------------------------
  LED Data
 *----------------------------------------------------------------------------*/
#define NUM_LEDS 		256	// Max LEDs accessible by config file

//One LED array for everything that could possibly happen at the same time
CRGB BladeBase[NUM_LEDS];
CRGB BladeFlashLayer[NUM_LEDS];
CRGB BladeScrollLayer[NUM_LEDS];
CRGB BladeShow[NUM_LEDS];
CRGB BladeBuffer[NUM_LEDS]; //Buffer for some FX
uint8_t BladeVisualizerBuffer[NUM_LEDS]; //Additional buffer used by some Visualizer FX

//One thread for everything that could possibly happen at the same time
LITE_FX_THREAD_T ThreadBladeBase;
LITE_FX_THREAD_T ThreadBladeFlash;
LITE_FX_THREAD_T ThreadBladeScroll;

LITE_FX_T BladeFXScroll;
LITE_FX_T BladeFXFlash;

// defaults, overwrite with config file
uint8_t BladeStart = 0;
uint8_t BladeLength = 144;
uint16_t BladeScrollTime = 500;
uint16_t FlashTime = 200;
CRGB FlashColor = CRGB::White;
CRGB MainColor = CRGB::Gold;
uint8_t BladeScrollTipLength = 24;
bool BootSolidColor = 0;

/*-----------------------------------------------------------------------------
  Blade FX Table - Defined in BladeFXTable.cpp
 *----------------------------------------------------------------------------*/
extern LITE_FX_TABLE_T BladeFXTable;
extern void BladeFXTableInit(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB * ledStripBuffer);

/*-----------------------------------------------------------------------------
  Buttons
 *----------------------------------------------------------------------------*/
Bounce SDCardDetect	= Bounce(SDCARD_DETECT, 	10);  // 10 ms debounce
//Bounce ButtonPower 	= Bounce(BUTTON_PIN_POWER, 	10);  // 10 ms debounce
//Bounce ButtonAux 	= Bounce(BUTTON_PIN_AUX, 	10);  // 10 ms debounce
OneButton ButtonPower(BUTTON_PIN_POWER, true, true);
OneButton ButtonAux(BUTTON_PIN_AUX, true, true);

/*-----------------------------------------------------------------------------
  SoundFX
 *----------------------------------------------------------------------------*/
#define SOUND_FX_FILE_COUNT_MAX			16
#define SOUND_FX_FILENAME_LEN_MAX		32	//including directory

//Buffers used by SoundFX
char FilenamesMenu		[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesBoot		[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesPowerOn	[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesPowerOff	[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesHum		[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesSwing		[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesClash		[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesLockUp	[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesBlaster	[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];

SOUND_FX_T SoundFXMenu;
SOUND_FX_T SoundFXBoot;
SOUND_FX_T SoundFXPowerOn;
SOUND_FX_T SoundFXPowerOff;
SOUND_FX_T SoundFXHum;
SOUND_FX_T SoundFXSwing;
SOUND_FX_T SoundFXClash;
SOUND_FX_T SoundFXLockUp;
SOUND_FX_T SoundFXBlaster;

// Default file prefix name or directory name
const char * WAV_DEFAULT_FONT_DIR1 	= "bank1";
const char * WAV_DEFAULT_FONT_DIR2 	= "bank2";
const char * WAV_DEFAULT_FONT_DIR3 	= "bank3";
const char * WAV_DEFAULT_BOOT 		= "BOOT";
const char * WAV_DEFAULT_POWERON 	= "POWERON";
const char * WAV_DEFAULT_POWEROFF 	= "POWEROFF";
const char * WAV_DEFAULT_HUM		= "HUM";
const char * WAV_DEFAULT_SWING		= "SWING";
const char * WAV_DEFAULT_CLASH		= "CLASH";
const char * WAV_DEFAULT_LOCKUP		= "LOCKUP";
const char * WAV_DEFAULT_BLASTER 	= "BLASTER";
const char * WAV_DEFAULT_MENU	 	= "MENU";
const char * WAV_DEFAULT_PWROFF 	= "PWROFF";

// Custom naming data, read these from config file
#define WAV_PREFIX_LEN_MAX				10

char FontDir1[WAV_PREFIX_LEN_MAX] = "bank1";
char FontDir2[WAV_PREFIX_LEN_MAX] = "bank2";
char FontDir3[WAV_PREFIX_LEN_MAX] = "bank3";

// Use enchant directory system or plecter file name prefix system
bool 	Font1UseClassicStyle = 0;
bool 	Font2UseClassicStyle = 0;
bool 	Font3UseClassicStyle = 0;

//uint8_t FontBootUp = 1;

//possibly implement user configurable wav names/prefixes later
//bool WavUseCustomNaming = 0;
//char WavPrefixBoot		[WAV_PREFIX_LEN_MAX];
//char WavPrefixPowerOn		[WAV_PREFIX_LEN_MAX];
//char WavPrefixPowerOff	[WAV_PREFIX_LEN_MAX];

// Active font
uint8_t FontActiveIndex;
char * 	FontActiveDir;
bool 	FontActiveUseClassicStyle;

/*-----------------------------------------------------------------------------
  Music Player
 *----------------------------------------------------------------------------*/
SOUND_FX_T SoundFXMusic;
char FilenamesMusic[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
const char * const SOUND_DIR_MUSIC = "/Music";

/*-----------------------------------------------------------------------------
  SmoothSwing
 *----------------------------------------------------------------------------*/
bool SmoothSwingEnable = 1;
uint8_t SmoothSwingVersion = 1;

SOUND_FX_T SoundFXSmoothSwingA;
SOUND_FX_T SoundFXSmoothSwingB;

char FilenamesSmoothSwingA[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];
char FilenamesSmoothSwingB[SOUND_FX_FILE_COUNT_MAX][SOUND_FX_FILENAME_LEN_MAX];

const char * WAV_DEFAULT_SMOOTH_SWING_A 	= "SWINGA";
const char * WAV_DEFAULT_SMOOTH_SWING_B 	= "SWINGB";

//functions defined in SmoothSwing.cpp
extern void SmoothSwingV1(uint32_t nCurrentTime, float gx, float gy, float gz);
extern void SmoothSwingV1S(uint32_t nCurrentTime, float gx, float gy, float gz);
extern void SmoothSwingV2(uint32_t nCurrentTime, float gx, float gy, float gz);
extern void SmoothSwingV2S(uint32_t nCurrentTime, float gx, float gy, float gz);
extern float SmoothSwing_GetSwingA();
extern float SmoothSwing_GetSwingB();
extern float SmoothSwing_GetSwingStrength();
extern float SmoothSwing_GetSwingAGain();
extern float SmoothSwing_GetSwingBGain();
extern float SmoothSwing_GetHumGain();
extern void debug_SmoothSwing();

bool SmoothSwingBlock; // for disabling SmoothSwing while playing other sounds

/*-----------------------------------------------------------------------------
  Hibernate
 *----------------------------------------------------------------------------*/
SnoozeDigital digital;	// this is the pin wakeup driver
SnoozeBlock config_teensy3x(digital);	//config for card detect pin

uint32_t HibernateDelay = 10000;
bool HibernateEnable = 0;

/*-----------------------------------------------------------------------------
  Lightsaber State Data
 *----------------------------------------------------------------------------*/
MENU_T * MenuPowerOn;
uint32_t TimeOff = 0;

/*-----------------------------------------------------------------------------
  Lightsaber Other Configuration
 *----------------------------------------------------------------------------*/
bool BluetoothEnable = 0;

/*-----------------------------------------------------------------------------
  Forward Declare Menus
 *----------------------------------------------------------------------------*/
extern MENU_T MENU_POWER_OFF;
extern MENU_T MENU_LIGHT_FX;
extern MENU_T MENU_SABER_FX;
extern MENU_T MENU_VISUALIZER;

/*-----------------------------------------------------------------------------
  Config File Read
 *----------------------------------------------------------------------------*/
const char ConfigFilename[] = "config.txt";

void ConfigDefault()
{
	//Default settings
	MenuPowerOn = &MENU_LIGHT_FX;

	HibernateEnable 	= 1;
	HibernateDelay 		= 60000;
	BluetoothEnable		= 0;

	BladeLength 		= 144;
	BladeScrollTime 	= 500;

	BladeScrollTipLength = 24;

	FlashTime = 200;
	FlashColor = CRGB::White;
	MainColor = CRGB::Gold;

	Font1UseClassicStyle	= 0;
	Font2UseClassicStyle 	= 0;
	Font3UseClassicStyle 	= 0;

	snprintf(FontDir1, WAV_PREFIX_LEN_MAX, "%s", "bank1");
	snprintf(FontDir2, WAV_PREFIX_LEN_MAX, "%s", "bank2");
	snprintf(FontDir3, WAV_PREFIX_LEN_MAX, "%s", "bank3");

	SmoothSwingEnable 	= 0;
	SmoothSwingVersion 	= 2;
}

void ConfigRead()
{
    if(SD.exists(ConfigFilename))
    {
    	if(ini_getl("General", "Boot_Saber_Menu", 0, ConfigFilename) == 0)
    		MenuPowerOn = &MENU_LIGHT_FX;
    	else
    		MenuPowerOn = &MENU_SABER_FX;

    	HibernateEnable 	= ini_getl("General", "Hibernate_Enable", 	1, 		ConfigFilename);
    	HibernateDelay 		= ini_getl("General", "Hibernate_Delay", 	60000, 	ConfigFilename);
    	BluetoothEnable		= ini_getl("General", "Bluetooth_Enable", 	0, 		ConfigFilename);

    	BladeLength 		= ini_getl("Blade", "LED_Count",	144, ConfigFilename);
    	BladeScrollTime 	= ini_getl("Blade", "Scroll_Time", 	500, ConfigFilename);

    	BladeScrollTipLength = ini_getl("Blade", "Scroll_Tip_Fade_Length", 	24, ConfigFilename);

    	BootSolidColor 	= ini_getl("Blade", "Boot_Solid_Color", 0, ConfigFilename);

       	MainColor.r 	= ini_getl("Blade", "Main_Color_R", 	255, ConfigFilename);
       	MainColor.g 	= ini_getl("Blade", "Main_Color_G", 	215, ConfigFilename);
       	MainColor.b 	= ini_getl("Blade", "Main_Color_B", 	0, 	ConfigFilename);

    	FlashTime 		= ini_getl("Blade", "Flash_Time", 		200, ConfigFilename);
    	FlashColor.r 	= ini_getl("Blade", "Flash_Color_R", 	255, ConfigFilename);
    	FlashColor.g 	= ini_getl("Blade", "Flash_Color_G", 	255, ConfigFilename);
    	FlashColor.b 	= ini_getl("Blade", "Flash_Color_B", 	255, ConfigFilename);

    	Font1UseClassicStyle = ini_getl("Sound", "Font1_Style", 		0, ConfigFilename);
    	Font2UseClassicStyle = ini_getl("Sound", "Font2_Style", 		0, ConfigFilename);
    	Font3UseClassicStyle = ini_getl("Sound", "Font3_Style", 		0, ConfigFilename);

    	if(ini_getl("Sound", "Use_Custom_Naming", 	0, ConfigFilename))
    	{
    		ini_gets("Sound", "Font1_Name", 0,	FontDir1, 	sizeof(FontDir1)/sizeof(FontDir1[0]), 	ConfigFilename);
    		ini_gets("Sound", "Font2_Name", 0,	FontDir1, 	sizeof(FontDir2)/sizeof(FontDir2[0]), 	ConfigFilename);
    		ini_gets("Sound", "Font3_Name", 0,	FontDir1, 	sizeof(FontDir3)/sizeof(FontDir3[0]), 	ConfigFilename);
    	}

    	SmoothSwingEnable 	= ini_getl("Sound", "SmoothSwing_Enable", 	0, ConfigFilename);
    	SmoothSwingVersion 	= ini_getl("Sound", "SmoothSwing_Version", 	1, ConfigFilename);
    }
    else
    {
    	Serial.println("No file");
    }
}


/*-----------------------------------------------------------------------------
  Boot/Reboot
 *----------------------------------------------------------------------------*/
static void LoadFont(const char * fontDirName, bool style)
{
	// buffer to build complete filename with directory path
	char PathBuffer[SOUND_FX_FILENAME_LEN_MAX];

	if (style == 0)	// Enchant Style
	{
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_MENU);
		SoundFX_InitFXWithDirectory(&SoundFXMenu, 		(char **)FilenamesMenu, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_BOOT);
		SoundFX_InitFXWithDirectory(&SoundFXBoot, 		(char **)FilenamesBoot, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_POWERON);
		SoundFX_InitFXWithDirectory(&SoundFXPowerOn, 	(char **)FilenamesPowerOn, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_POWEROFF);
		SoundFX_InitFXWithDirectory(&SoundFXPowerOff, 	(char **)FilenamesPowerOff, SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_HUM);
		SoundFX_InitFXWithDirectory(&SoundFXHum, 		(char **)FilenamesHum, 		SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_SWING);
		SoundFX_InitFXWithDirectory(&SoundFXSwing, 		(char **)FilenamesSwing, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_CLASH);
		SoundFX_InitFXWithDirectory(&SoundFXClash, 		(char **)FilenamesClash, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_LOCKUP);
		SoundFX_InitFXWithDirectory(&SoundFXLockUp, 	(char **)FilenamesLockUp, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_BLASTER);
		SoundFX_InitFXWithDirectory(&SoundFXBlaster, 	(char **)FilenamesBlaster, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
//			debug_SoundFX_PrintFilenames((char **)FilenamesMenu, 		SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesBoot, 		SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesPowerOn, 	SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesPowerOff, 	SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesHum, 		SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesSwing, 		SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesClash, 		SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesLockUp, 		SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
//			debug_SoundFX_PrintFilenames((char **)FilenamesBlaster, 	SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);

		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_SMOOTH_SWING_A);
		SoundFX_InitFXWithDirectory(&SoundFXSmoothSwingA, 	(char **)FilenamesSmoothSwingA, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s", fontDirName, "/", WAV_DEFAULT_SMOOTH_SWING_B);
		SoundFX_InitFXWithDirectory(&SoundFXSmoothSwingB, 	(char **)FilenamesSmoothSwingB, 	SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, PathBuffer);
	}
	else	//Classic Style
	{
	//	SoundFX_InitFX(&SoundFXPowerOn, (char **)FilenamesPowerOn, SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);
	//	// run once looking for the prefix without a following number
	//	snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s%s", FontActiveDir, "/", WAV_DEFAULT_POWERON, ".WAV");
	//	SoundFX_AddFile(&SoundFXPowerOn, PathBuffer);
	//	// look for prefix1 through prefix16
	//	for (uint8_t i = 1; i< SOUND_FX_FILE_COUNT_MAX; i++)
	//	{
	//		snprintf(PathBuffer, SOUND_FX_FILENAME_LEN_MAX, "%s%s%s%d%s", FontActiveDir, "/", WAV_DEFAULT_POWERON, i, ".WAV");
	//		SoundFX_AddFile(&SoundFXPowerOn, PathBuffer);
	//	}
	}
}

static void LoadFontConfig(const char * fontDirName, bool style)
{

}

static void LoadBladeFX()
{
	BladeFXTableInit(BladeBase, 0, BladeLength, BladeBuffer);
	//BladeFXTableInit(Blade, 0, BladeLength, BladeBuffer, MainColor);
	//LiteFX_SetColor(LiteFXTable_GetFX(&BladeFXTable),  MainColor);

	LiteFX_InitBladeScroll(&BladeFXScroll, BladeScrollLayer, 0, BladeLength, 0, true, 50, CRGB::White, CRGB::Black, BladeScrollTipLength, 51, 100, 0);
	LiteFX_SetNoIndexReset(&BladeFXScroll); // This way extend will pick up where retract left off
	//LiteFX_SetBladeScrollTime(&FXBladeScroll, BladeScrollTime, 100);

	//LiteFX_InitBladeScroll(&BladeFXReverseScroll, BladeScrollLayer, 0, BladeLength, 0, true, 50, CRGB::Black, CRGB::White, BladeScrollTipLength, 51, 100, 0);

	LiteFX_InitColorFader(&BladeFXFlash, BladeFlashLayer, 0, BladeLength, 0, true, 10, true, CRGB::Black, FlashColor, 21, 100, 0);
	//LiteFX_SetColorFaderCycleTime(&FXFlash, FlashTime, 240); //overwrite ticks
	//LiteFX_InitColorFaderStrip(&FXFlash, BladeFlashLayer, 0, BladeLength, 0, true, 12, 12, true, Blade, CRGB::White, 25, 60, 0);

	if(BootSolidColor)
	{
		BladeFXTable.TableIndex = 2; // create interface function later
		LiteFX_SetColor(LiteFXTable_GetFX(&BladeFXTable),  MainColor); //set handle
		// set all fx main color?
	}

	VisualizerFX_SetThresholdWav();
	VisualizerFX_Init(BladeBase, 0, BladeLength,  PaletteTable_GetPalette(&PaletteTable_Default), BladeBuffer, BladeVisualizerBuffer, 0);
}

static void Boot(void) // initial battery connect, coming out of hibernate, SD Card insert
{
	BoardOn();
	FontActiveIndex = 0;

	delay(50); // delay before trying SD card

	if (SD.begin(SDCARD_CS_PIN))
	{
		ConfigRead();

		SoundFX_InitFXWithDirectory(&SoundFXMusic, (char **)FilenamesMusic, SOUND_FX_FILENAME_LEN_MAX, SOUND_FX_FILE_COUNT_MAX, SOUND_DIR_MUSIC);
		//debug_SoundFX_PrintFilenames((char **)FilenamesMusic, SOUND_FX_FILE_COUNT_MAX, SOUND_FX_FILENAME_LEN_MAX);

		LoadFont(FontDir1, Font1UseClassicStyle); //always load font1 on reboot.
	}
	else
	{
		Serial.println("Unable to access the SD card");

		ConfigDefault();
	}

	if (BluetoothEnable) Bluefruit_Init();//Bluefruit_Restart();

	LoadBladeFX();

	Menu_StartMenu(&MENU_POWER_OFF);

	SoundFX_PlayFXRandom(&SoundFXBoot);
}

/*-----------------------------------------------------------------------------
  Common Menu Functions
 *----------------------------------------------------------------------------*/
static void PowerOn(void)
{
	LiteFX_Start(&ThreadBladeScroll, &BladeFXScroll, 0);
	LiteFX_SetDirection(&BladeFXScroll, true);	// change direction to forward
	SoundFX_StopAll();

	if (SmoothSwingEnable)
	{
		SmoothSwingBlock = 1;

		MixerWav2Wav3.gain(PLAY_WAV_2_L, .5);
		MixerWav2Wav3.gain(PLAY_WAV_2_R, .5);
		MixerWav2Wav3.gain(PLAY_WAV_3_L, 0);
		MixerWav2Wav3.gain(PLAY_WAV_3_R, 0);

		SoundFXMixer.gain(PLAY_WAV_1_L, 0);
		SoundFXMixer.gain(PLAY_WAV_1_R, 0);
		SoundFXMixer.gain(PLAY_WAV_2_WAV_3_MASTER, 	1);
		SoundFXMixer.gain(SOUND_FX_CHANNEL_3, 0);

		PlayWav3.stop();
		PlayWav2.play(SoundFX_GetFXNextFile(&SoundFXPowerOn));
	}
	else
	{
		SoundFX_PlayFX(&SoundFXPowerOn);
	}

	Menu_StartMenu(MenuPowerOn); //do this last, visualizer menu will play a sound to overwrite SoundFXPowerOn.
}

static void PowerOff(void)
{
	MenuPowerOn = Menu_GetMenu(); // return to current menu on power on
	Menu_StartMenu(&MENU_POWER_OFF);

	LiteFX_Start(&ThreadBladeScroll, &BladeFXScroll, 0);
	LiteFX_SetDirection(&BladeFXScroll, false);
	SoundFX_StopAll();
	SoundFX_PlayFX(&SoundFXPowerOff);
}

static void MenuChange(void)
{
	SoundFX_StopAll();
	SoundFX_PlayFX(&SoundFXMenu);

	Menu_StartNextMenu();
	LiteFX_Start(&ThreadBladeFlash, &BladeFXFlash, 0);
}

/*-----------------------------------------------------------------------------
  Power Off Menu
 *----------------------------------------------------------------------------*/
static void Hibernate(void)
{
	int who;

	if (HibernateEnable)
	{
		if(millis() - TimeOff > HibernateDelay)
		{
			// go into low power mode
			BoardOff();
			who = Snooze.hibernate( config_teensy3x );

			WRITE_RESTART();
			//delay(50); // delay before trying SD card
			//Boot();
			//delay(50); // delay before polling buttons in main loop
		}
	}
}

void NextFont()
{
	TimeOff = millis(); //an action was made, reset hibernate timer

	for (uint8_t i = 0; i < 3; i++) // try at most 3 times
	{
		FontActiveIndex = (FontActiveIndex + 1) % 3;

		// set sound active dir, can change to array indexing later, depending on font count requirements
		switch (FontActiveIndex)
		{
			case 0:
				FontActiveDir				= FontDir1;
				FontActiveUseClassicStyle	= Font1UseClassicStyle;
				break;
			case 1:
				FontActiveDir 				= FontDir2;
				FontActiveUseClassicStyle	= Font2UseClassicStyle;
				break;
			case 2:
				FontActiveDir 				= FontDir3;
				FontActiveUseClassicStyle	= Font3UseClassicStyle;
				break;
		}

		if(FontActiveDir[0]) // if directory was found during config file read
			break;
	}

	LoadFont(FontActiveDir, FontActiveUseClassicStyle);

	SoundFX_PlayFXRandom(&SoundFXBoot);
}

void PowerOffMenuStart()
{
	TimeOff = millis();
}

/*-----------------------------------------------------------------------------
  LightFX Menu
 *----------------------------------------------------------------------------*/
static void LightFXNext(void)
{
	LiteFX_Start(&ThreadBladeBase, LiteFXTable_GetNext(&BladeFXTable), 0);
}

static void LightFXPrev(void)
{
	LiteFX_Start(&ThreadBladeBase, LiteFXTable_GetPrevious(&BladeFXTable), 0);
}

static void LightFXInteractionUp(void)
{
	LiteFX_DoInterfaceUp(LiteFXTable_GetFX(&BladeFXTable));
}

static void LightFXInteractionDown(void)
{
	LiteFX_DoInterfaceDown(LiteFXTable_GetFX(&BladeFXTable));
}

static void LightFXMenuStart(void)
{
	LiteFX_Start(&ThreadBladeBase, LiteFXTable_GetFX(&BladeFXTable), 0);
}

/*-----------------------------------------------------------------------------
  SaberFX Menu
 *----------------------------------------------------------------------------*/
static void SaberFXBlaster(void)
{
	LiteFX_Start(&ThreadBladeFlash, &BladeFXFlash, 0);

	if (SmoothSwingEnable)
	{
		SmoothSwingBlock = 1;

		MixerWav2Wav3.gain(PLAY_WAV_2_L, .5);
		MixerWav2Wav3.gain(PLAY_WAV_2_R, .5);
		MixerWav2Wav3.gain(PLAY_WAV_3_L, 0);
		MixerWav2Wav3.gain(PLAY_WAV_3_R, 0);

		SoundFXMixer.gain(PLAY_WAV_1_L, 0.15);
		SoundFXMixer.gain(PLAY_WAV_1_R, 0.15);
		SoundFXMixer.gain(PLAY_WAV_2_WAV_3_MASTER, 	0.7);
		SoundFXMixer.gain(SOUND_FX_CHANNEL_3, 0);

		PlayWav3.stop();
		PlayWav2.play(SoundFX_GetFXNextFile(&SoundFXBlaster));
	}
	else
	{
		SoundFX_PlayFXLayered(&SoundFXBlaster, 1);
	}
}

static void SaberFXClash(void)
{
	LiteFX_Start(&ThreadBladeFlash, &BladeFXFlash, 0);

	if (SmoothSwingEnable)
	{
		SmoothSwingBlock = 1; 	// Stop reseting gains for SmoothSwing until clash, PlayWav2, stops playing

		MixerWav2Wav3.gain(PLAY_WAV_2_L, .5);
		MixerWav2Wav3.gain(PLAY_WAV_2_R, .5);
		MixerWav2Wav3.gain(PLAY_WAV_3_L, 0);
		MixerWav2Wav3.gain(PLAY_WAV_3_R, 0);

		SoundFXMixer.gain(PLAY_WAV_1_L, 0.15);
		SoundFXMixer.gain(PLAY_WAV_1_R, 0.15);
		SoundFXMixer.gain(PLAY_WAV_2_WAV_3_MASTER, 	0.7);
		SoundFXMixer.gain(SOUND_FX_CHANNEL_3, 0);

		PlayWav3.stop();
		PlayWav2.play(SoundFX_GetFXNextFile(&SoundFXClash)); // Play clash using swing slot
	}
	else
	{
		SoundFX_PlayFXLayered(&SoundFXClash, 1);
	}

}

static void SaberFXSwing(void)
{
	if(SmoothSwingEnable) return;

	SoundFX_PlayFXLayered(&SoundFXSwing, 1);
}

static void SaberFXLoop(void)
{
	if (SmoothSwingEnable)
	{
		if (!PlayWav1.isPlaying()) PlayWav1.play(SoundFX_GetFXNextFile(&SoundFXHum));

		if (!PlayWav2.isPlaying()) SmoothSwingBlock = 0; //clash or blaster stopped playing

		if(!SmoothSwingBlock)
		{
			switch (SmoothSwingVersion)
			{
				case 1:
					//SmoothSwingV1(millis(), IMU.readFloatGyroX(), IMU.readFloatGyroY(), IMU.readFloatGyroZ());
					SmoothSwingV1S(millis(), IMU.readFloatGyroX(), IMU.readFloatGyroY(), IMU.readFloatGyroZ());
					break;
				case 2:
					//SmoothSwingV2(millis(), IMU.readFloatGyroX(), IMU.readFloatGyroY(), IMU.readFloatGyroZ());
					SmoothSwingV2S(millis(), IMU.readFloatGyroX(), IMU.readFloatGyroY(), IMU.readFloatGyroZ());
					break;
			}

			// SwingAGain + SwingBGain + HumGain <= 1 + Overdrive
			SoundFXMixer.gain(PLAY_WAV_1_L, SmoothSwing_GetHumGain()/2);
			SoundFXMixer.gain(PLAY_WAV_1_R, SmoothSwing_GetHumGain()/2);
			SoundFXMixer.gain(PLAY_WAV_2_WAV_3_MASTER, SmoothSwing_GetSwingAGain()+SmoothSwing_GetSwingBGain());
			SoundFXMixer.gain(SOUND_FX_CHANNEL_3, 0);

			// SmoothSwing_GetSwingA + SmoothSwing_GetSwingB == 1
			MixerWav2Wav3.gain(PLAY_WAV_2_L, SmoothSwing_GetSwingA()/2);
			MixerWav2Wav3.gain(PLAY_WAV_2_R, SmoothSwing_GetSwingA()/2);
			MixerWav2Wav3.gain(PLAY_WAV_3_L, SmoothSwing_GetSwingB()/2);
			MixerWav2Wav3.gain(PLAY_WAV_3_R, SmoothSwing_GetSwingB()/2);

			// change SwingA and SwingB files.
			if (SmoothSwing_GetSwingStrength() < .05)
			{
				SoundFX_GetFXNextFile(&SoundFXSmoothSwingA);
				SoundFX_GetFXNextFile(&SoundFXSmoothSwingB);
			}

			if (
				IMU_DetectSwingDirectionChange() ||
				(!PlayWav2.isPlaying() && !PlayWav3.isPlaying())
				)
			{
				PlayWav2.play(SoundFX_GetFXNextFile(&SoundFXSmoothSwingA));
				PlayWav3.play(SoundFX_GetFXNextFile(&SoundFXSmoothSwingB));
			}
		}
	}
	else
	{
		SoundFX_PlayFXLayeredBackgroudLoop(&SoundFXHum);
	}
}

static void SaberFXMenuStart(void)
{
	LiteFX_Start(&ThreadBladeBase, LiteFXTable_GetFX(&BladeFXTable), 0);
}

/*-----------------------------------------------------------------------------
  Visualizer Menu
 *----------------------------------------------------------------------------*/
static void VisualizerUpdate()
{
	//VisualizerFX_Update(SoundFX_GetRMS());
	VisualizerFX_Update(SoundFX_GetPeak());
//		VisualizerFX_CalcVars(); here or in 30 fps
}

static void VisualizerNextSong()
{
	VisualizerFX_ResetVars();
	SoundFX_StopAll();
	SoundFX_PlayNext(&SoundFXMusic);
}

static void VisualizerPrevSong()
{
	VisualizerFX_ResetVars();
	SoundFX_StopAll();
	SoundFX_PlayPrevious(&SoundFXMusic);
}

static void VisualizerNextFX(void)
{
	for (uint16_t n = 0; n < BladeLength; n++)
		BladeBase[n] = 0;

	LiteFXOS_StartThreadCycleArgFreq(&ThreadBladeBase.Thread, 0, 0, VisualizerFXTableGetNext(), 30);
}

static void VisualizerPrevFX(void)
{
	for (uint16_t n = 0; n < BladeLength; n++)
		BladeBase[n] = 0;

	LiteFXOS_StartThreadCycleArgFreq(&ThreadBladeBase.Thread, 0, 0, VisualizerFXTableGetPrev(), 30);
}

static void VisualizerMenuStart(void)
{
	VisualizerFX_ResetVars();
	VisualizerFX_SetThresholdWav();

	SoundFX_PlayFX(&SoundFXMusic);

	for (uint16_t n = 0; n < BladeLength; n++)
		BladeBase[n] = 0;

	LiteFXOS_StartThreadCycleArgFreq(&ThreadBladeBase.Thread, 0, 0, VisualizerFXTableGet(), 30);
}

/*-----------------------------------------------------------------------------
  Lightsaber Menus/States
 *----------------------------------------------------------------------------*/
enum FUNCTION_MAP_TRANSLATION
{
	MENU_LOOP_FUNCTION,
	BUTTON_FUNCTION_POWER,
	BUTTON_FUNCTION_MENU,
	BUTTON_FUNCTION_NEXT,
	BUTTON_FUNCTION_PREV,
	BUTTON_FUNCTION_ALT_NEXT,
	BUTTON_FUNCTION_ALT_PREV,
	MOTION_FUCTION_SWING,
	MOTION_FUNCTION_CLASH,
};

MENU_T
MENU_POWER_OFF =
{
	.NextMenu = 0,
	.PrevMenu = 0,
	.InitFunction = PowerOffMenuStart,
	.FunctionMap = // compiler does not allow array indexing in struct, voluntarily conform to translation map order
	{
		Hibernate,	//.FunctionMap[MENU_LOOP_FUNCTION]
		PowerOn,	//.FunctionMap[BUTTON_FUNCTION_POWER]
		0,			//.FunctionMap[BUTTON_FUNCTION_MENU]
		NextFont,	//.FunctionMap[BUTTON_FUNCTION_NEXT]
		NextFont,	//.FunctionMap[BUTTON_FUNCTION_ALT_NEXT]
		0,			//.FunctionMap[BUTTON_FUNCTION_PREV]
		0,			//.FunctionMap[BUTTON_FUNCTION_ALT_PREV]
		0,			//.FunctionMap[MOTION_FUCTION_SWING]
		0,			//.FunctionMap[MOTION_FUNCTION_CLASH]
	},
};

MENU_T
MENU_LIGHT_FX =
{
	.NextMenu = &MENU_SABER_FX,
	.PrevMenu = &MENU_VISUALIZER,
	.InitFunction = LightFXMenuStart,
	.FunctionMap =
	{
		0,							//.FunctionMap[MENU_LOOP_FUNCTION]
		PowerOff,					//.FunctionMap[BUTTON_FUNCTION_POWER]
		MenuChange,					//.FunctionMap[BUTTON_FUNCTION_MENU]
		LightFXNext,				//.FunctionMap[BUTTON_FUNCTION_NEXT]
		LightFXPrev,				//.FunctionMap[BUTTON_FUNCTION_PREV]
		LightFXInteractionUp,		//.FunctionMap[BUTTON_FUNCTION_ALT_NEXT]
		LightFXInteractionDown,		//.FunctionMap[BUTTON_FUNCTION_ALT_PREV]
		0,							//.FunctionMap[MOTION_FUCTION_SWING]
		0,							//.FunctionMap[MOTION_FUNCTION_CLASH]
	},
};

MENU_T
MENU_SABER_FX =
{
	.NextMenu = &MENU_VISUALIZER,
	.PrevMenu = &MENU_LIGHT_FX,
	.InitFunction = SaberFXMenuStart,
	.FunctionMap =
	{
		SaberFXLoop,				//.FunctionMap[MENU_LOOP_FUNCTION]
		PowerOff,					//.FunctionMap[BUTTON_FUNCTION_POWER]
		MenuChange,					//.FunctionMap[BUTTON_FUNCTION_MENU]
		SaberFXBlaster,//lockup		//.FunctionMap[BUTTON_FUNCTION_NEXT]
		0,							//.FunctionMap[BUTTON_FUNCTION_PREV]
		SaberFXBlaster,				//.FunctionMap[BUTTON_FUNCTION_ALT_NEXT]
		0,							//.FunctionMap[BUTTON_FUNCTION_ALT_PREV]
		SaberFXSwing,				//.FunctionMap[MOTION_FUCTION_SWING]
		SaberFXClash,				//.FunctionMap[MOTION_FUNCTION_CLASH]
	}
};

MENU_T
MENU_VISUALIZER =
{
	.NextMenu = &MENU_LIGHT_FX,
	.PrevMenu = &MENU_SABER_FX,
	.InitFunction = VisualizerMenuStart,
	.FunctionMap =
	{
		VisualizerUpdate,			//.FunctionMap[MENU_LOOP_FUNCTION]
		PowerOff,					//.FunctionMap[BUTTON_FUNCTION_POWER]
		MenuChange,					//.FunctionMap[BUTTON_FUNCTION_MENU]
		VisualizerNextFX,			//.FunctionMap[BUTTON_FUNCTION_NEXT]
		VisualizerPrevFX,			//.FunctionMap[BUTTON_FUNCTION_PREV]
		VisualizerNextSong,			//.FunctionMap[BUTTON_FUNCTION_ALT_NEXT]
		VisualizerPrevSong,			//.FunctionMap[BUTTON_FUNCTION_ALT_PREV]
		0,							//.FunctionMap[MOTION_FUCTION_SWING]
		0,							//.FunctionMap[MOTION_FUNCTION_CLASH]
	},
};

//void-void wrapper for OneButton
void PowerButtonClick(void){Menu_DoFunction(BUTTON_FUNCTION_POWER);}
void PowerButtonHold(void){Menu_DoFunction(BUTTON_FUNCTION_MENU);}
void AuxButtonClick(void){Menu_DoFunction(BUTTON_FUNCTION_ALT_NEXT);}
void AuxButtonHold(void){Menu_DoFunction(BUTTON_FUNCTION_NEXT);}

void setup() {
//	delay(500); // sanity delay
	Serial.begin(9600); // USB is always 12 Mbit/sec

  	pinMode(SDCARD_DETECT,	 	INPUT_PULLUP);
  	pinMode(BUTTON_PIN_POWER,	INPUT_PULLUP);
  	pinMode(BUTTON_PIN_AUX,		INPUT_PULLUP);

  	SDCardDetect 	= Bounce(SDCARD_DETECT,	 	10);  // 10 ms debounce
  	//ButtonPower 	= Bounce(BUTTON_PIN_POWER,	10);  // 10 ms debounce
  	//ButtonAux 		= Bounce(BUTTON_PIN_AUX,	10);  // 10 ms debounce

  	ButtonPower.attachClick(PowerButtonClick);
  	ButtonPower.attachLongPressStart(PowerButtonHold);
  	ButtonAux.attachClick(AuxButtonClick);
  	ButtonAux.attachLongPressStart(AuxButtonHold);

  	// For hibernate
    digital.pinMode(SDCARD_DETECT, 		INPUT_PULLUP, FALLING);//pin, mode, type
    digital.pinMode(BUTTON_PIN_POWER, 	INPUT_PULLUP, FALLING);//pin, mode, type

  	BoardInit();

	//FastLED.addLeds<WS2812B, LED_DATA_PIN1, GRB>(BladeShow, NUM_LEDS).setCorrection( TypicalLEDStrip );
  	FastLED.addLeds<WS2812SERIAL,LED_DATA_PIN1,BRG>(BladeShow,NUM_LEDS).setCorrection( TypicalLEDStrip );
  	FastLED.setCorrection(TypicalLEDStrip);
	FastLED.showColor(0);
	FastLED.show();

	SPI.setMOSI(SDCARD_MOSI_PIN);
	SPI.setMISO(SDCARD_MISO_PIN);
	SPI.setSCK(SDCARD_SCK_PIN);

	SoundFX_Init(ENABLE_AMP_PIN);
	IMU_Init();
	Bluefruit_Init();

	Boot();

	Serial.println("Init Done\n");
}

void loop()
{
	uint32_t currentTime = millis(); // update time once through each loop is enough

	static bool updateLEDs;

	if (LiteFX_ProcThread(&ThreadBladeBase, currentTime)) 	updateLEDs = 1;
	if (LiteFX_ProcThread(&ThreadBladeFlash, currentTime)) 	updateLEDs = 1;
	if (LiteFX_ProcThread(&ThreadBladeScroll, currentTime)) 	updateLEDs = 1;

	if (updateLEDs)
	{
		updateLEDs = 0;

		for (uint8_t n = 0; n < BladeLength; n++)
		{
			BladeShow[n] = (BladeBase[n] | BladeFlashLayer[n]) & BladeScrollLayer[n];
		}

		FastLED.show();
	}

	SDCardDetect.update();
  	//ButtonPower.update();
  	//ButtonAux.update();
  	ButtonPower.tick();
  	ButtonAux.tick();

	IMU_Poll(currentTime);

	// interesting, if we run this function before updating LEDs, there is not enough time for "!PlayWav1.isPlaying()" to detect there is a sound playing
	// and will over write the sound played by Menu_DoFunction(BUTTON_FUNCTION_POWER)
	Menu_DoFunction(MENU_LOOP_FUNCTION);

  	if(SDCardDetect.fell())	{Boot();}

//  	if(ButtonPower.fell() && !ButtonAux.read())	{Menu_DoFunction(BUTTON_FUNCTION_ALT_NEXT);}
//  	else if(ButtonPower.fell())					{Menu_DoFunction(BUTTON_FUNCTION_POWER);}
//	if(ButtonAux.fell() && !ButtonPower.read())	{Menu_DoFunction(BUTTON_FUNCTION_MENU);}
//	else if(ButtonAux.fell())					{Menu_DoFunction(BUTTON_FUNCTION_NEXT);}

	if(IMU_DetectSwing(currentTime, 500)) 		{Menu_DoFunction(MOTION_FUCTION_SWING);}
	if(IMU_DetectImpact(currentTime, 500)) 		{Menu_DoFunction(MOTION_FUNCTION_CLASH);}

	if (BluetoothEnable)
	{
		Bluefruit_Poll();

		if(Bluefruit_GetUpdateFlag())
		{
			Bluefruit_ResetUpdateFlag();

			//set the color of the current FX to the chosen color, some FX will not have color options.
			LiteFX_SetColor( LiteFXTable_GetFX(&BladeFXTable), CRGB(Bluefruit_GetColorR(), Bluefruit_GetColorG(), Bluefruit_GetColorB()) );

			if(Bluefruit_GetButtonUp())		{Menu_DoFunction(BUTTON_FUNCTION_ALT_PREV);}
			if(Bluefruit_GetButtonDown())	{Menu_DoFunction(BUTTON_FUNCTION_ALT_NEXT);}
			if(Bluefruit_GetButtonLeft())	{Menu_DoFunction(BUTTON_FUNCTION_PREV);}
			if(Bluefruit_GetButtonRight())	{Menu_DoFunction(BUTTON_FUNCTION_NEXT);}

			if(Bluefruit_GetButton1())		{Menu_DoFunction(BUTTON_FUNCTION_POWER);}
			if(Bluefruit_GetButton2())		{Menu_DoFunction(BUTTON_FUNCTION_MENU);}
			if(Bluefruit_GetButton3())		{}
			if(Bluefruit_GetButton4())		{}
		}
	}

	if (Serial.available())
	{
		char ch = Serial.read();
		if (ch == '1')	{Menu_DoFunction(BUTTON_FUNCTION_POWER);}
		if (ch == '2')	{Menu_DoFunction(BUTTON_FUNCTION_MENU);}
		if (ch == '3')	{Menu_DoFunction(BUTTON_FUNCTION_NEXT);}
		if (ch == '4')	{Menu_DoFunction(BUTTON_FUNCTION_PREV);}
		if (ch == '5')	{Menu_DoFunction(BUTTON_FUNCTION_ALT_NEXT);}
		if (ch == '6')	{Menu_DoFunction(BUTTON_FUNCTION_ALT_PREV);}
		if (ch == '7')	{Menu_DoFunction(MOTION_FUCTION_SWING);}
		if (ch == '8')	{Menu_DoFunction(MOTION_FUNCTION_CLASH);}
		if (ch == 'e')	{Serial.println("echo");}
		if (ch == 'b')	{Serial.println(BatteryGetRaw());}
		if (ch == 'i')	{IMU_PrintSerial();}
		if (ch == 's')	{SoundFX_PlayFile("bank2/boot/BOOT1.WAV");}
		if (ch == 'a')	{SoundFX_PlayFile("bank2/poweron/POWERON.WAV");}
	}
}

extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
//  int _write(){return -1;}
}


