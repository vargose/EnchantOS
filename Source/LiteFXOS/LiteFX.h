/**************************************************************************/
/*!
    @file     LiteFX.h
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
*/
/**************************************************************************/
#ifndef SOURCE_LITEFXOS_LITEFX_H_
#define SOURCE_LITEFXOS_LITEFX_H_

typedef struct LiteFX LITE_FX_T;
typedef const struct LiteFXTickRate LITE_FX_TICK_RATE_T;
typedef const struct LiteFXInterface LITE_FX_INTERFACE_T;

/*-----------------------------------------------------------------------------
  LiteFX Thread
 *----------------------------------------------------------------------------*/
typedef struct
{
	LITE_FX_OS_THREAD_T Thread;
} LITE_FX_THREAD_T;

/*-----------------------------------------------------------------------------
  LiteFX
 *----------------------------------------------------------------------------*/
struct LiteFX
{
	LiteFX(){}
	LIGHT_FX_T LightFX;
	union
	{
		FX_COLOR_FADER_VARS_T 				ColorFaderVars;
		FX_COLOR_FADER_STRIP_VARS_T 		ColorFaderStripVars;
		FX_COLOR_WIPE_VARS_T				ColorWipeVars;
		FX_BLINK_VARS_T						BlinkVars;
		FX_PALETTE_WIPE_VARS_T				PaletteWipeVars;
		FX_PALETTE_FADER_VARS_T				PaletteFaderVars;
		FX_FIRE2012_VARS_T					Fire2012Vars;
		FX_FIRE2012_WITH_PALETTE_VARS_T		Fire2012WithPaletteVars;
		FX_BLADE_SCROLL_VARS_T				BladeScrollVars;
		struct
		{
			CRGB Color;
			uint32_t Index;
			const CRGBPalette16 * p_Palette;
			const CRGB * p_ColorArray;
		} SolidColorVars;
	};

	// need either type or handles
	//	enum
	//	{
	//		FX_COLOR_FADER_T,
	//		FX_COLOR_FADER_STRIP_T,
	//		FX_COLOR_WIPE_T,
	//	}	FXType;

	// For FX vars that can be changed at runtime
	uint32_t *				p_IndexHandle;
	bool *					p_DirectionHandle;
	CRGB *					p_Color1Handle;
	//CRGB *					p_Color2Handle;
	const CRGBPalette16 **	p_PaletteHandle;
	LITE_FX_THREAD_T * 		p_ThreadHandle;

	uint16_t	Freq;			// Ticks per second
	uint32_t	Ticks;			// Ticks to run for Momentary FX / Set to 0 for cycle
	void (* OnComplete)(void);	// Runs on complete, momentary only

	uint32_t	InitialIndex;		// The initial step in the pattern, the pattern always starts here
	bool		InitialDirection;	// The initial direction of the pattern
	bool		SameStartOptOut;	// by default, for momentary FX, use the same initial state every time we start the pattern.

	PALETTE_TABLE_T		* p_PaletteTable;
	LITE_FX_TICK_RATE_T * p_TickRate;
	LITE_FX_INTERFACE_T * p_Interface;
};

/*-----------------------------------------------------------------------------
  LiteFX Tick Rate
 *----------------------------------------------------------------------------*/
struct LiteFXTickRate
{
	uint8_t Default;
	uint8_t Min;
	uint8_t Max;
	//uint8_t StepSize;
	float StepMultiplier;
};

/*-----------------------------------------------------------------------------
  LiteFX Interface
 *----------------------------------------------------------------------------*/
struct LiteFXInterface
{
	void (*Up)	(LITE_FX_T * LiteFX); 	//map to speed increase, or special change;
	void (*Down)(LITE_FX_T * LiteFX);
};

/*-----------------------------------------------------------------------------
  LiteFX Table
 *----------------------------------------------------------------------------*/
typedef struct
{
	LITE_FX_T ** p_LiteFXArray;
	uint8_t TableSize;
	uint8_t TableIndex;
} LITE_FX_TABLE_T;

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
bool LiteFX_ProcThread(LITE_FX_THREAD_T * thread);
void LiteFX_Start(LITE_FX_THREAD_T * thread, LITE_FX_T * fx, void(*onComplete)(void));

void LiteFX_InitColorFader(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetColorFader(LITE_FX_T * fx, uint16_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetColorFaderCycleTime(LITE_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

void LiteFX_InitColorFaderStrip(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, CRGB * ledStripInitialColor, CRGB color1, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetColorFaderStrip(LITE_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, CRGB * ledStripInitialColor, CRGB color1, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetColorFaderStripCycleTime(LITE_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

void LiteFX_InitColorWipe(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetColorWipe(LITE_FX_T * fx, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));

void LiteFX_InitBlink(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool startingDirection, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetBlink(LITE_FX_T * fx, bool startingDirection, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));

void LiteFX_InitPaletteWipe(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));
void LiteFX_SetPaletteWipe(LITE_FX_T * fx, uint16_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));

void LiteFX_InitPaletteFader(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));
void LiteFX_SetPaletteFader(LITE_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));

void LiteFX_InitPaletteSolid(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * palette);
void LiteFX_NextPaletteColor(LITE_FX_T * fx);
void LiteFX_PreviousPaletteColor(LITE_FX_T * fx);

//void LiteFX_InitPaletteTableSolid(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, PALETTE_TABLE_T * table);
//void LiteFX_SetPaletteTableSolid(LITE_FX_T * fx, const CRGBPalette16 * palette);

//void LiteFX_InitColorArraySolid(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGB * colorArray, uint8_t colorCount);
//void LiteFX_SetColorArraySolid(LITE_FX_T * fx, const CRGB * colorArray);

void LiteFX_InitFire2012(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint8_t * ledsBuffer, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));
void LiteFX_SetFire2012(LITE_FX_T * fx, bool direction, uint8_t cooling, uint8_t sparkling, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));
void LiteFX_Fire2012Reset(LITE_FX_T * fx);

void LiteFX_InitFire2012WithPalette(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t * ledsBuffer, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));
void LiteFX_SetFire2012WithPalette(LITE_FX_T * fx, bool direction, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t cooling, uint8_t sparkling, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void));

void LiteFX_InitBladeScroll(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, float pos, bool direction, uint16_t steps, CRGB color1, CRGB color2, uint16_t tipLength, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetBladeScroll(LITE_FX_T * fx, float pos, bool direction, uint16_t steps, CRGB color1, CRGB color2, uint16_t tipLength, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void));
void LiteFX_SetBladeScrollTime(LITE_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
//void LiteFX_SetCycle(LITE_FX_T * fx, uint8_t ticksPerSecond);
//void LiteFX_SetMomentary(LITE_FX_T * fx, uint32_t ticks, uint32_t ticksPerSecond, void (*onComplete)(void));
void LiteFX_SetContinous(LITE_FX_T * fx);
void LiteFX_SetIndex(LITE_FX_T * fx, uint32_t index);
void LiteFX_SetPos(LITE_FX_T * fx, float pos);
void LiteFX_SetDirection(LITE_FX_T * fx, bool dir);
void LiteFX_SetColor(LITE_FX_T * fx, CRGB color);
void LiteFX_SetPalette(LITE_FX_T * fx, const CRGBPalette16 * palette);

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
void LiteFX_AttachPaletteTable(LITE_FX_T * fx, PALETTE_TABLE_T * table);
void LiteFX_NextPalette(LITE_FX_T * fx);
void LiteFX_PreviousPalette(LITE_FX_T * fx);
void LiteFX_NextPaletteTableColor(LITE_FX_T * fx);
void LiteFX_PreviousPaletteTableColor(LITE_FX_T * fx);

void LiteFX_AttachTickRate(LITE_FX_T * fx, LITE_FX_TICK_RATE_T * rate);
void LiteFX_SetTickRateDefaultFreq(LITE_FX_T * fx);
void LiteFX_SetTickRateFaster(LITE_FX_T * fx);
void LiteFX_SetTickRateSlower(LITE_FX_T * fx);

void LiteFX_AttachInterface(LITE_FX_T * fx, LITE_FX_INTERFACE_T * interface);
void LiteFX_DoInterfaceUp(LITE_FX_T * fx);
void LiteFX_DoInterfaceDown(LITE_FX_T * fx);

void LiteFXTable_InitTable(LITE_FX_TABLE_T * table, LITE_FX_T * fxArray, uint8_t fxCount);
LITE_FX_T * LiteFXTable_GetFX(LITE_FX_TABLE_T * table);
LITE_FX_T * LiteFXTable_GetNext(LITE_FX_TABLE_T * table);
LITE_FX_T * LiteFXTable_GetPrevious(LITE_FX_TABLE_T * table);

/*-----------------------------------------------------------------------------
 Defaults
 *----------------------------------------------------------------------------*/
#ifndef NO_LITE_FX_DEFAULTS

void LiteFX_InitDefault(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB * ledStripBuffer);

extern LITE_FX_T LiteFX_DefaultPaletteWipe;
extern LITE_FX_T LiteFX_DefaultPaletteFader;
extern LITE_FX_T LiteFX_DefaultPaletteSolid;

extern LITE_FX_T LiteFX_DefaultFire;
extern LITE_FX_T LiteFX_DefaultFireWithPalette;
extern LITE_FX_T LiteFX_DefaultFireWithDynamicPalette;

extern LITE_FX_T LiteFX_DefaultStrobe;
extern LITE_FX_T LiteFX_DefaultCylon;
extern LITE_FX_T LiteFX_DefaultSineWipe;

extern LITE_FX_T LiteFX_DefaultRainbowWithGlitter;
extern LITE_FX_T LiteFX_DefaultConfetti;
extern LITE_FX_T LiteFX_DefaultBPM;
extern LITE_FX_T LiteFX_DefaultJuggle;

extern LITE_FX_T LiteFX_DefaultFlash;


/*-----------------------------------------------------------------------------
  Palette Table for Fire2012
 *----------------------------------------------------------------------------*/
extern PALETTE_TABLE_T LiteFX_DefaultFirePaletteTable;

/*-----------------------------------------------------------------------------
  LiteFX Tick Rate Defaults
 *----------------------------------------------------------------------------*/
extern LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_60;
extern LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_90;
extern LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_120;

/*-----------------------------------------------------------------------------
  LiteFX Interface Defaults
 *----------------------------------------------------------------------------*/
extern LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_PALETTE;
extern LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_PALETTE_COLOR;
extern LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_SPEED;
extern LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_FIRE_RESET;
/*-----------------------------------------------------------------------------
  LiteFX Table Default
 *----------------------------------------------------------------------------*/
extern LITE_FX_TABLE_T LiteFXTable_Default;

#endif

#endif /* SOURCE_LITEFXOS_LITEFX_H_ */
