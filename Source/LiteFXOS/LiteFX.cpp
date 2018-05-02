/**************************************************************************/
/*!
    @file     LiteFX.cpp
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

    @brief A more streamlined interface for configuring Light FX.
*/
/**************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "FastLED.h"

#include "PaletteTable.h"

extern "C" {
#include "OS.h"
}
#include "LightFX.h"
#include "LiteFX.h"

bool LiteFX_ProcThread(LITE_FX_THREAD_T * thread)
{
	LiteFXOS_SetTimerCounter(millis());
	if (LiteFXOS_ProcThread(&thread->Thread)) 	return 1;
	else										return 0;
};

void LiteFX_Start(LITE_FX_THREAD_T * thread, LITE_FX_T * fx, void(*onComplete)(void))
{
	 //check if FX has been initialized
	if(fx->LightFX.Pattern == 0) 	return;
	if(fx->LightFX.LoadFX == 0) 	return;
	if(fx->LightFX.p_LEDStrip == 0) return;
	if(fx->LightFX.p_Vars == 0) 	return;
	if(fx->LightFX.NumLEDs == 0) 	return;

	fx->p_ThreadHandle = thread; // Keep track of thread the fx is executing for speed changes later.

	fx->OnComplete = onComplete;

	if (fx->Ticks == 0)		// Ticks == 0 means it is a cycle FX, run repeatedly
	{
		if (fx->Freq == 0) 	// Freq == 0 means it is a solid pattern
		{
			LiteFXOS_SetThreadStop(&thread->Thread);
			if (fx->p_Color1Handle) LightFX_SetSolid(fx->LightFX.p_LEDStrip, 0, fx->LightFX.NumLEDs, *fx->p_Color1Handle);
			FastLED.show();
		}
		else
		{
			LiteFXOS_StartThreadCycleArgFreq(&thread->Thread, &fx->LightFX, fx->LightFX.LoadFX, fx->LightFX.Pattern, fx->Freq);
		}
	}
	else // Ticks > 0 means it is a Momentary FX, run for number of ticks
	{
		if (!fx->SameStartOptOut)
		{
			if (fx->p_IndexHandle) 		*fx->p_IndexHandle 		= fx->InitialIndex;			// Start LiteFX from initial settings
			if (fx->p_DirectionHandle) 	*fx->p_DirectionHandle 	= fx->InitialDirection;
		}
		LiteFXOS_StartThreadMomentaryArgTicksFreq(&thread->Thread, &fx->LightFX, fx->LightFX.LoadFX, fx->LightFX.Pattern, fx->Ticks, fx->Freq, fx->OnComplete);
	}
}

/*-----------------------------------------------------------------------------
  Patterns
  -----------------------------------------------------------------------------
  LiteFX_InitPattern 		- must be called before using.
  LiteFX_SetPattern  		- sets all variables used.
  LiteFX_SetPatternOption 	- sets parameters unique to pattern.
 *----------------------------------------------------------------------------*/
/******************************************************************************/
/*!
 * @name ColorFader
 * @brief
 */
/******************************************************************************/
void LiteFX_InitColorFader(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->p_IndexHandle 		= &fx->ColorFaderVars.Index;
	fx->p_DirectionHandle 	= &fx->ColorFaderVars.Direction;
	fx->p_Color1Handle		= &fx->ColorFaderVars.Color1;

	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_InitFXColorFader(&fx->LightFX, &fx->ColorFaderVars, ledStrip, ledStart, ledLength, startingIndex, startingDirection, transitionSteps, boundaryBehavior, color1, color2);
}

void LiteFX_SetColorFader(LITE_FX_T * fx, uint16_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_SetFXColorFader(&fx->LightFX, startingIndex, startingDirection, transitionSteps, boundaryBehavior, color1, color2);
}

void LiteFX_SetColorFaderCycleTime(LITE_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{
	LightFX_SetFXColorFaderCycleTime(&fx->LightFX, cycleTimeMs, ticksPerSecond);
	fx->Ticks 			= fx->ColorFaderVars.TransitionSteps*2 + 1;
}

/******************************************************************************/
/*!
 * @name  LightFX ColorFader one color
 * @brief
 */
/******************************************************************************/
void LiteFX_InitColorFaderStrip(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, CRGB * ledStripInitialColor, CRGB color1, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->p_IndexHandle 		= &fx->ColorFaderStripVars.Index;
	fx->p_DirectionHandle 	= &fx->ColorFaderStripVars.Direction;
	fx->p_Color1Handle		= &fx->ColorFaderStripVars.Color1;
	//lesstrip

	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_InitFXColorFaderStrip(&fx->LightFX, &fx->ColorFaderStripVars, ledStrip, ledStart, ledLength, startingIndex, startingDirection, boundarySteps, fullTransitionSteps, boundaryBehavior, ledStripInitialColor, color1);
}

void LiteFX_SetColorFaderStrip(LITE_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, CRGB * ledStripInitialColor, CRGB color1, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_SetFXColorFaderStrip(&fx->LightFX, startingIndex, startingDirection, boundarySteps, fullTransitionSteps, boundaryBehavior, ledStripInitialColor, color1);
}

void LiteFX_SetColorFaderStripCycleTime(LITE_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{

}

/******************************************************************************/
/*!
 * @name ColorWipe
 * @brief Wipes color to index position
 */
/******************************************************************************/
void LiteFX_InitColorWipe(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->p_IndexHandle 		= &fx->ColorWipeVars.Index;
	fx->p_DirectionHandle 	= &fx->ColorWipeVars.Direction;
	fx->p_Color1Handle		= &fx->ColorWipeVars.Color1;

	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_InitFXColorWipe(&fx->LightFX, &fx->ColorWipeVars, ledStrip, ledStart, ledLength, startingIndex, startingDirection, boundaryBehavior, color1, color2);
}

void LiteFX_SetColorWipe(LITE_FX_T * fx, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_SetFXColorWipe(&fx->LightFX, startingIndex, startingDirection, boundaryBehavior, color1, color2);
}

/******************************************************************************/
/*!
 * @name Blink
 * @brief
 */
/******************************************************************************/
void LiteFX_InitBlink(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool startingDirection, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->p_DirectionHandle 	= &fx->BlinkVars.Direction;
	fx->p_Color1Handle		= &fx->BlinkVars.Color1;

	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialDirection 	= startingDirection;

	LightFX_InitFXBlink(&fx->LightFX, &fx->BlinkVars, ledStrip, ledStart, ledLength, startingDirection, color1, color2);
}

void LiteFX_SetBlink(LITE_FX_T * fx, bool startingDirection, CRGB color1, CRGB color2, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialDirection 	= startingDirection;

	LightFX_SetFXBlink(&fx->LightFX, startingDirection, color1, color2);
}

/******************************************************************************/
/*!
 * @name Palette Wipe
 * @brief
 */
/******************************************************************************/
void LiteFX_InitPaletteWipe(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->p_IndexHandle 		= &fx->PaletteWipeVars.Index;
	fx->p_DirectionHandle 	= &fx->PaletteWipeVars.Direction;
	fx->p_PaletteHandle		= &fx->PaletteWipeVars.p_Palette;

	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_InitFXPaletteWipe(&fx->LightFX, &fx->PaletteWipeVars, ledStrip, ledStart, ledLength, startingIndex, startingDirection, stepsPerCycle, boundaryBehavior, palette, visibleSpectrum);
}

void LiteFX_SetPaletteWipe(LITE_FX_T * fx, uint16_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_SetFXPaletteWipe(&fx->LightFX, startingIndex, startingDirection, stepsPerCycle, boundaryBehavior, palette, visibleSpectrum);
}

/******************************************************************************/
/*!
 * @name Palette Fader
 * @brief
 */
/******************************************************************************/
void LiteFX_InitPaletteFader(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->p_IndexHandle 		= &fx->PaletteFaderVars.Index;
	fx->p_DirectionHandle 	= &fx->PaletteFaderVars.Direction;
	fx->p_PaletteHandle		= &fx->PaletteFaderVars.p_Palette;

	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_InitFXPaletteFader(&fx->LightFX, &fx->PaletteFaderVars, ledStrip, ledStart, ledLength, startingIndex, startingDirection, stepsPerCycle, boundaryBehavior, palette);
}

void LiteFX_SetPaletteFader(LITE_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= startingIndex;
	fx->InitialDirection 	= startingDirection;

	LightFX_SetFXPaletteFader(&fx->LightFX, startingIndex, startingDirection, stepsPerCycle, boundaryBehavior, palette);
}

/******************************************************************************/
/*!
 * @name Palette Solid
 * @brief Solid colors using palette. Switch between colors using interface.
 * 		Solid color is handled by LiteFX only, there is no LightFX counterpart
 */
/******************************************************************************/
inline static void NotRunPattern(){}
inline static void NotRunLoad(void *){}

void LiteFX_InitPaletteSolid(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * palette)
{
	fx->p_PaletteHandle	= &fx->SolidColorVars.p_Palette;
	fx->p_Color1Handle	= &fx->SolidColorVars.Color;
	fx->p_IndexHandle	= &fx->SolidColorVars.Index;

	fx->Ticks 			= 0;
	fx->Freq 			= 0;
	fx->OnComplete 		= 0;
	fx->InitialIndex 		= 0;
	fx->InitialDirection 	= 0;

	fx->SolidColorVars.Index = 0;
	fx->SolidColorVars.Color = ColorFromPalette(*palette, 0);
	fx->SolidColorVars.p_Palette = palette;

	LightFX_InitFXMap(&fx->LightFX, ledStrip, ledStart, ledLength);
	// Set so LiteFX_Start knows the FX has been initialized.
	fx->LightFX.Pattern = NotRunPattern;
	fx->LightFX.LoadFX = NotRunLoad;
	fx->LightFX.p_Vars =  &fx->SolidColorVars;

	LiteFX_AttachInterface(fx, &LIGHT_FX_INTERFACE_PALETTE_COLOR);
}

//void LiteFX_SetPaletteSolid(LITE_FX_T * fx, const CRGBPalette16 * palette)
//{
//	*fx->p_PaletteHandle		=  palette;
//}

void LiteFX_NextPaletteColor(LITE_FX_T * fx)
{
	//change index handle to PaletteIndexHandle
	if (fx->p_Color1Handle && fx->p_PaletteHandle && fx->p_IndexHandle)
	{
		*fx->p_IndexHandle = (*fx->p_IndexHandle + 256/16) % 256;
		*fx->p_Color1Handle = ColorFromPalette(**fx->p_PaletteHandle, *fx->p_IndexHandle);
	}

	if (fx->Freq == 0) // recolor LEDs for solid
	{
		//LiteFX_Start(LITE_FX_THREAD_T * thread, LITE_FX_T * fx)
		LightFX_SetSolid(fx->LightFX.p_LEDStrip, 0, fx->LightFX.NumLEDs, *fx->p_Color1Handle);
		FastLED.show();
	}
}

void LiteFX_PreviousPaletteColor(LITE_FX_T * fx)
{
	if (fx->p_Color1Handle && fx->p_PaletteHandle && fx->p_IndexHandle)
	{
		*fx->p_IndexHandle = (*fx->p_IndexHandle - 256/16) % 256;
		*fx->p_Color1Handle = ColorFromPalette(**fx->p_PaletteHandle, *fx->p_IndexHandle);
	}

	if (fx->Freq == 0) // recolor LEDs for solid
	{
		//LiteFX_Start(LITE_FX_THREAD_T * thread, LITE_FX_T * fx)
		LightFX_SetSolid(fx->LightFX.p_LEDStrip, 0, fx->LightFX.NumLEDs, *fx->p_Color1Handle);
		FastLED.show();
	}
}

/******************************************************************************/
/*!
 * @name Palette Table Solid
 * @brief Solid colors using palette table, can switch colors and palettes
 */
/******************************************************************************/
//void LiteFX_InitPaletteTableSolid(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, PALETTE_TABLE_T * table)
//{
//	fx->p_PaletteHandle	=
//	fx->p_Color1Handle	= &fx->SolidColor;
//
//	fx->Ticks 			= 0;
//	fx->Freq 			= 0;
//	fx->OnComplete 		= 0;
//	fx->BeginIndex 		= 0;
//	fx->BeginDirection 	= 0;
//
//	fx->SolidColor = ColorFromPalette(*palette, 0);
//
//	LightFX_InitFXMap(&fx->LightFX, ledStrip, ledStart, ledLength);
//	LiteFX_AttachInterface(fx, &LIGHT_FX_INTERFACE_PALETTE_COLOR);
//}
//
//void LiteFX_SetPaletteTableSolid(LITE_FX_T * fx, const PALETTE_TABLE_T * table)
//{
//	fx->p_PaletteHandle		=
//}

/******************************************************************************/
/*!
 * @name Colors Array Solid
 * @brief Solid colors using specified array of colors
 */
/******************************************************************************/
//void LiteFX_InitColorArraySolid(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGB * colorArray, uint8_t colorCount)
//{
//	fx->p_Color1Handle	= (CRGB *)colorArray;
//
//	fx->Ticks 			= 0;
//	fx->Freq 			= 0;
//	fx->OnComplete 		= 0;
//	fx->BeginIndex 		= 0;
//	fx->BeginDirection 	= 0;
//}
//
//void LiteFX_SetColorArraySolid(LITE_FX_T * fx, const CRGB * colorArray)
//{
//	fx->p_Color1Handle	= (CRGB *)colorArray;
//}


/******************************************************************************/
/*!
 * @name Fire2012
 * @brief
 */
/******************************************************************************/
void LiteFX_InitFire2012(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint8_t * ledsBuffer, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;

	LightFX_InitFXFire2012Default(&fx->LightFX, &fx->Fire2012Vars, ledStrip, ledStart, ledLength, ledsBuffer);
}

void LiteFX_SetFire2012(LITE_FX_T * fx, bool direction, uint8_t cooling, uint8_t sparkling, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;

	LightFX_SetFXFire2012(&fx->LightFX, direction, cooling, sparkling);
}

void LiteFX_Fire2012Reset(LITE_FX_T * fx)
{
	LightFX_SetFXFire2012Restart(&fx->LightFX);
}

/******************************************************************************/
/*!
 * @name Fire2012WithPalette
 * @brief
 */
/******************************************************************************/
void LiteFX_InitFire2012WithPalette(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t * ledsBuffer, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->p_DirectionHandle 	= &fx->Fire2012WithPaletteVars.Direction;
	fx->p_PaletteHandle		= &fx->Fire2012WithPaletteVars.p_Palette;

	fx->InitialDirection 	= 1;

	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;

	LightFX_InitFXFire2012WithPaletteDefault(&fx->LightFX, &fx->Fire2012WithPaletteVars, ledStrip, ledStart, ledLength, initialPalette, useDynamicPalette, ledsBuffer);
}

void LiteFX_SetFire2012WithPalette(LITE_FX_T * fx, bool direction, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t cooling, uint8_t sparkling, uint32_t ticks, uint8_t ticksPerSecond, void (*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq 			= ticksPerSecond;
	fx->OnComplete 		= onComplete;

	fx->InitialDirection 	= direction;

	LightFX_SetFXFire2012WithPalette(&fx->LightFX, direction, initialPalette, useDynamicPalette, cooling, sparkling);
}

/******************************************************************************/
/*!
 * @name Blade
 * @brief
 */
/******************************************************************************/
void LiteFX_InitBladeScroll(LITE_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, float pos, bool direction, uint16_t steps, CRGB color1, CRGB color2, uint16_t tipLength, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->p_IndexHandle 		= (uint32_t *)&fx->BladeScrollVars.Position;
	fx->p_DirectionHandle 	= &fx->BladeScrollVars.Direction;
	fx->p_Color1Handle		= &fx->BladeScrollVars.Color1;

	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= pos;
	fx->InitialDirection 	= direction;

	LightFX_InitBladeScroll(&fx->LightFX, &fx->BladeScrollVars, ledStrip, ledStart, ledLength, pos, direction, steps, 0, color1, color2, tipLength);
}

void LiteFX_SetBladeScroll(LITE_FX_T * fx, float pos, bool direction, uint16_t steps, CRGB color1, CRGB color2, uint16_t tipLength, uint32_t ticks, uint8_t ticksPerSecond, void(*onComplete)(void))
{
	fx->Ticks 			= ticks;
	fx->Freq	 		= ticksPerSecond;
	fx->OnComplete 		= onComplete;
	fx->InitialIndex 		= pos;
	fx->InitialDirection 	= direction;

	LightFX_SetBladeScroll(&fx->LightFX, pos, direction, steps, 0, color1, color2, tipLength);
}

void LiteFX_SetBladeScrollTime(LITE_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{
	LightFX_SetFXColorFaderCycleTime(&fx->LightFX,  cycleTimeMs,  ticksPerSecond);
	fx->Ticks 			= fx->BladeScrollVars.Steps + 1;
}

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
//void LiteFX_SetCycle(LITE_FX_T * fx, uint8_t ticksPerSecond)
//{
//	fx->Ticks = 0;
//	fx->Freq = ticksPerSecond;
//	fx->OnComplete = 0;
//}

//void LiteFX_SetMomentary(LITE_FX_T * fx, uint32_t ticks, uint32_t ticksPerSecond, void (*onComplete)(void))
//{
//	fx->Ticks = ticks;
//	fx->Freq = ticksPerSecond;
//	fx->OnComplete = onComplete;
//}

//void LiteFX_SetMomentaryTimeFreq(LITE_FX_T * fx, uint32_t time, uint32_t ticksPerSecond, void (*onComplete)(void))
//{
//}

void LiteFX_SetContinous(LITE_FX_T * fx)
{
	fx->SameStartOptOut		= true;
}

void LiteFX_SetIndex(LITE_FX_T * fx, uint32_t index)
{
	if (fx->p_IndexHandle) *fx->p_IndexHandle = index;
}

void LiteFX_SetPos(LITE_FX_T * fx, float pos)
{
	if (fx->p_IndexHandle) *fx->p_IndexHandle = pos;
}

void LiteFX_SetDirection(LITE_FX_T * fx, bool dir)
{
	if (fx->p_DirectionHandle) *fx->p_DirectionHandle = dir;
}

void LiteFX_SetColor(LITE_FX_T * fx, CRGB color)
{
	if (fx->p_Color1Handle) *fx->p_Color1Handle = color;
}

void LiteFX_SetPalette(LITE_FX_T * fx, const CRGBPalette16 * palette)
{
	*fx->p_PaletteHandle		= palette;
}


/*-----------------------------------------------------------------------------
  Palette Table
 *----------------------------------------------------------------------------*/
void LiteFX_AttachPaletteTable(LITE_FX_T * fx, PALETTE_TABLE_T * table)
{
	fx->p_PaletteTable = table;
}

void LiteFX_NextPalette(LITE_FX_T * fx)
{
	if (fx->p_PaletteHandle && fx->p_PaletteTable) *fx->p_PaletteHandle = PaletteTable_GetNextPalette(fx->p_PaletteTable);
}

void LiteFX_PreviousPalette(LITE_FX_T * fx)
{
	if (fx->p_PaletteHandle && fx->p_PaletteTable) *fx->p_PaletteHandle = PaletteTable_GetPreviousPalette(fx->p_PaletteTable);
}

void LiteFX_NextPaletteTableColor(LITE_FX_T * fx)
{
	if (fx->p_Color1Handle && fx->p_PaletteTable) *fx->p_Color1Handle = PaletteTable_GetNextColor(fx->p_PaletteTable);
}

void LiteFX_PreviousPaletteTableColor(LITE_FX_T * fx)
{
	if (fx->p_Color1Handle && fx->p_PaletteTable) *fx->p_Color1Handle = PaletteTable_GetPreviousColor(fx->p_PaletteTable);
}

/*-----------------------------------------------------------------------------
  LiteFX Tick Rate
 *----------------------------------------------------------------------------*/
void LiteFX_AttachTickRate(LITE_FX_T * fx, LITE_FX_TICK_RATE_T * rate)
{
	fx->p_TickRate = rate;
}

void LiteFX_SetTickRateDefaultFreq(LITE_FX_T * fx)
{
	if (fx->p_TickRate)
	{
		fx->Freq = fx->p_TickRate->Default;

		LiteFXOS_SetThreadFreq(&fx->p_ThreadHandle->Thread, fx->Freq);
	}
}

void LiteFX_SetTickRateFaster(LITE_FX_T * fx)
{
	if (fx->p_TickRate)
	{
		// freq will not take on every value cleanly this way. come back to this later
		if (fx->Freq * fx->p_TickRate->StepMultiplier < fx->p_TickRate->Max)
			fx->Freq = fx->Freq * fx->p_TickRate->StepMultiplier;
		else
			fx->Freq = fx->p_TickRate->Min;

		LiteFXOS_SetThreadFreq(&fx->p_ThreadHandle->Thread, fx->Freq);
	}
}

void LiteFX_SetTickRateSlower(LITE_FX_T * fx)
{
	if (fx->p_TickRate)
	{
		if (fx->Freq / fx->p_TickRate->StepMultiplier > fx->p_TickRate->Min)
			fx->Freq = fx->Freq / fx->p_TickRate->StepMultiplier;
		else
			fx->Freq = fx->p_TickRate->Max;

		LiteFXOS_SetThreadFreq(&fx->p_ThreadHandle->Thread, fx->Freq);
	}
}

/*-----------------------------------------------------------------------------
  LiteFX Interface/Interactions
 *----------------------------------------------------------------------------*/
void LiteFX_AttachInterface(LITE_FX_T * fx, LITE_FX_INTERFACE_T * interface)
{
	fx->p_Interface = interface;
}

void LiteFX_DoInterfaceUp(LITE_FX_T * fx)
{
	if (fx->p_Interface)
		if (fx->p_Interface->Up)
			fx->p_Interface->Up(fx);
}

void LiteFX_DoInterfaceDown(LITE_FX_T * fx)
{
	if (fx->p_Interface)
		if (fx->p_Interface->Down)
			fx->p_Interface->Down(fx);
}

/*-----------------------------------------------------------------------------
  LiteFX Table
 *----------------------------------------------------------------------------*/
void LiteFXTable_InitTable(LITE_FX_TABLE_T * table, LITE_FX_T ** fxArray, uint8_t fxCount)
{
	table->p_LiteFXArray = fxArray;
	table->TableSize = fxCount;
	table->TableIndex = 0;
}

LITE_FX_T * LiteFXTable_GetFX(LITE_FX_TABLE_T * table)
{
	return table->p_LiteFXArray[table->TableIndex];
}

LITE_FX_T * LiteFXTable_GetNext(LITE_FX_TABLE_T * table)
{
	if (table->TableIndex < table->TableSize - 1)	table->TableIndex++;
	else											table->TableIndex = 0;

	return table->p_LiteFXArray[table->TableIndex];
}

LITE_FX_T * LiteFXTable_GetPrevious(LITE_FX_TABLE_T * table)
{
	if (table->TableIndex > 0)	table->TableIndex--;
	else						table->TableIndex = table->TableSize - 1;

	return table->p_LiteFXArray[table->TableIndex];
}

/*-----------------------------------------------------------------------------
  Defaults
 *----------------------------------------------------------------------------*/
#ifndef NO_LITE_FX_DEFAULTS

//Call this function before using preset defaults, inits using some predefined values
//One buffer is shared between FX. Each FX using a buffer must be assigned a new buffer if they are to be used simultaneously.
void LiteFX_InitDefault(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB * ledStripBuffer)
{
	//Used by table
	LiteFX_InitPaletteWipe(&LiteFX_DefaultPaletteWipe, ledStrip, ledStart, ledLength, 0, true, 120, false, PaletteTable_GetPalette(&PaletteTable_Default), 255*2/3, 0, 60, 0);
	LiteFX_AttachInterface(&LiteFX_DefaultPaletteWipe, &LIGHT_FX_INTERFACE_PALETTE);
	LiteFX_AttachTickRate(&LiteFX_DefaultPaletteWipe, &LIGHT_FX_TICK_RATE_60);
	LiteFX_AttachPaletteTable(&LiteFX_DefaultPaletteWipe, &PaletteTable_Default);

	LiteFX_InitPaletteFader(&LiteFX_DefaultPaletteFader, ledStrip, ledStart, ledLength, 0, true, 120, false, PaletteTable_GetPalette(&PaletteTable_Default), 0, 60, 0);
	LiteFX_AttachInterface(&LiteFX_DefaultPaletteFader, &LIGHT_FX_INTERFACE_SPEED);
	LiteFX_AttachTickRate(&LiteFX_DefaultPaletteFader, &LIGHT_FX_TICK_RATE_60);
	LiteFX_AttachPaletteTable(&LiteFX_DefaultPaletteFader, &PaletteTable_Default);

	LiteFX_InitPaletteSolid(&LiteFX_DefaultPaletteSolid, ledStrip, ledStart, ledLength, PaletteTable_GetPalette(&PaletteTable_Default));
	LiteFX_AttachInterface(&LiteFX_DefaultPaletteSolid, &LIGHT_FX_INTERFACE_PALETTE_COLOR);

	LiteFX_InitFire2012(&LiteFX_DefaultFire, ledStrip, ledStart, ledLength, (uint8_t *)ledStripBuffer, 0, 60,  0);
	LiteFX_AttachInterface(&LiteFX_DefaultFire, &LIGHT_FX_INTERFACE_FIRE_RESET);

	LiteFX_InitFire2012WithPalette(&LiteFX_DefaultFireWithPalette, ledStrip, ledStart, ledLength, PaletteTable_GetPalette(&LiteFX_DefaultFirePaletteTable), false, (uint8_t *)ledStripBuffer, 0, 60, 0);
	LiteFX_AttachPaletteTable(&LiteFX_DefaultFireWithPalette, &LiteFX_DefaultFirePaletteTable);
	LiteFX_AttachInterface(&LiteFX_DefaultFireWithPalette, &LIGHT_FX_INTERFACE_PALETTE);

	LiteFX_InitFire2012WithPalette(&LiteFX_DefaultFireWithDynamicPalette, ledStrip, ledStart, ledLength, 0, true, (uint8_t *)ledStripBuffer, 0, 60, 0);
	LiteFX_AttachInterface(&LiteFX_DefaultFireWithDynamicPalette, &LIGHT_FX_INTERFACE_FIRE_RESET);

	//commonly used
	LiteFX_InitColorFaderStrip(&LiteFX_DefaultFlash, ledStrip, ledStart, ledLength, 0, true, 12, 12, true, ledStripBuffer, CRGB::White, 25, 240, 0);
}

/*-----------------------------------------------------------------------------
  LiteFX Preset Defaults
 *----------------------------------------------------------------------------*/
LITE_FX_T LiteFX_DefaultPaletteSolid;
LITE_FX_T LiteFX_DefaultPaletteWipe;
LITE_FX_T LiteFX_DefaultPaletteFader;

LITE_FX_T LiteFX_DefaultFire;
LITE_FX_T LiteFX_DefaultFireWithPalette;
LITE_FX_T LiteFX_DefaultFireWithDynamicPalette;

LITE_FX_T LiteFX_DefaultStrobe;
LITE_FX_T LiteFX_DefaultCylon;
LITE_FX_T LiteFX_DefaultSineWipe;

LITE_FX_T LiteFX_DefaultRainbowWithGlitter;
LITE_FX_T LiteFX_DefaultConfetti;
LITE_FX_T LiteFX_DefaultBPM;
LITE_FX_T LiteFX_DefaultJuggle;

//LITE_FX_T LiteFX_DefaultPaletteCylon;
//LITE_FX_T LiteFX_DefaultPaletteSineWipe;
//LITE_FX_T LiteFX_DefaultPaletteStrobe;
//
//LITE_FX_T LiteFX_DefaultTheaterChase;
//
LITE_FX_T LiteFX_DefaultFlash;
//LITE_FX_T LiteFX_DefaultFlicker;

/*-----------------------------------------------------------------------------
  Palette Table for Fire2012
 *----------------------------------------------------------------------------*/
const CRGBPalette16 FIRE_PALETTES_DEFAULT[] =
{
	//HeatColors_p,
	CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White),
	CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White),
	CRGBPalette16( CRGB::Black, CRGB::White),
};

PALETTE_TABLE_T LiteFX_DefaultFirePaletteTable =
{
	.p_Palettes = &FIRE_PALETTES_DEFAULT[0],
	.TableSize = sizeof(FIRE_PALETTES_DEFAULT)/sizeof(CRGBPalette16),
	.TableIndex = 0,
	.ColorIndex = 0,
};


/*-----------------------------------------------------------------------------
  LightFX Tick Rate Defaults
 *----------------------------------------------------------------------------*/
LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_60 =
{
	.Default = 60,
	.Min = 10,
	.Max = 240,
	.StepMultiplier = 1.5,
};

LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_90 =
{
	.Default = 90,
	.Min = 15,
	.Max = 240,
	.StepMultiplier = 1.5,
};

LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_120 =
{
	.Default = 120,
	.Min = 20,
	.Max = 240,
	.StepMultiplier = 1.5,
};

//LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_STROBE =
//{
//	.TicksPerSecondDefault = 20,
//	.TicksPerSecondMin = 1,
//	.TicksPerSecondMax = 60,
//	.TicksPerSecondStepSize = 5,
//};
//
//LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_1_STEP =
//{
//	.TicksPerSecondDefault = 3,
//	.TicksPerSecondMin = 1,
//	.TicksPerSecondMax = 10,
//	.TicksPerSecondStepSize = 1,
//};
//
//LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_SCANNER =
//{
//	.TicksPerSecondDefault = 20,
//	.TicksPerSecondMin = 1,
//	.TicksPerSecondMax = 150,
//	.TicksPerSecondStepSize = 10,
//};
//
//LITE_FX_TICK_RATE_T LIGHT_FX_TICK_RATE_SOLID =
//{
//	.Default = 1,
//	.Min = 1,
//	.Max = 1,
//	.StepMultiplier = 1,
//};

/*-----------------------------------------------------------------------------
  LightFX Interface Defaults
 *----------------------------------------------------------------------------*/
LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_PALETTE =
{
	.Up 	= LiteFX_NextPalette,
	.Down	= LiteFX_PreviousPalette,
};

LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_PALETTE_COLOR =
{
	.Up		= LiteFX_NextPaletteColor,
	.Down	= LiteFX_PreviousPaletteColor,
};

LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_SPEED =
{
	.Up 	= LiteFX_SetTickRateFaster,
	.Down 	= LiteFX_SetTickRateSlower,
};

LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_FIRE_RESET =
{
	.Up = LiteFX_Fire2012Reset,
	.Down = LiteFX_Fire2012Reset,
};

//LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_COLOR_STEP_SIZE =
//{
//	.Up = IncreaseColorStep,
//	.Down = DecreaseColorStep,
//};

//LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_MANUAL_TICK =
//{
//	.Up = ,
//	.Down = ,
//};

/*-----------------------------------------------------------------------------
  LiteFX Table
 *----------------------------------------------------------------------------*/
LITE_FX_T * const LITE_FX_ARRAY_DEFAULT[] =
{
	&LiteFX_DefaultPaletteWipe,
	&LiteFX_DefaultPaletteFader,
	&LiteFX_DefaultPaletteSolid,

	&LiteFX_DefaultFire,
	&LiteFX_DefaultFireWithPalette,
	&LiteFX_DefaultFireWithDynamicPalette,

//	&LiteFX_DefaultStrobe,
//	&LiteFX_DefaultCylon,
//	&LiteFX_DefaultSineWipe,

//	&LiteFX_DefaultRainbowWithGlitter,
//	&LiteFX_DefaultConfetti,
//	&LiteFX_DefaultBPM,
//	&LiteFX_DefaultJuggle,

//	&LITE_FX_SCANNER,
//	&LITE_FX_SCANNER_INTENSITY,
//	&LITE_FX_THEATER_CHASE,
};

LITE_FX_TABLE_T LiteFXTable_Default =
{
	.p_LiteFXArray 	= (LITE_FX_T **)&LITE_FX_ARRAY_DEFAULT,
	.TableSize 		= sizeof(LITE_FX_ARRAY_DEFAULT)/sizeof(LITE_FX_T*),
	.TableIndex 	= 0,
};

#endif
