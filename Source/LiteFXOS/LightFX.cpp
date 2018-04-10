/**************************************************************************/
/*!
    @file     LightFX.cpp
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

    @brief LED strip pattern algorithms
*/
/**************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "FastLED.h"
#include "LightFX.h"

#define FAST_LED_PALLETTE_MAX 	255
#define FAST_LED_COLOR_MAX		255
/*-----------------------------------------------------------------------------
  Common LED Strip Variables -
  Use these primitives because LED pattern algorithms are naturally written
  using these parameters
 *----------------------------------------------------------------------------*/
static CRGB * 	p_LEDStrip;
static uint16_t NumLEDs;

/*-----------------------------------------------------------------------------
  Common Pattern Variables - Shared by patterns
 *----------------------------------------------------------------------------*/
static uint32_t	* p_IndexPos;
static bool		* p_Direction;

static uint32_t	Steps;
static bool		BoundaryBehavior;

static CRGB 	Color1;
static CRGB 	Color2;
static CRGBPalette16 *	p_Palette;

static uint32_t Special1;
static uint32_t	Special2;
static uint32_t	Special3;

static CRGB 	* p_LEDStripBuffer;

static inline void MapLEDStrip(CRGB * ledStart, uint16_t numLEDs)
{
	p_LEDStrip = ledStart;
	NumLEDs = numLEDs;
}

static inline void MapLEDStripArgStartEnd(CRGB * ledStrip, uint16_t start, uint16_t end)
{
	p_LEDStrip = &ledStrip[start];
	NumLEDs = end - start + 1;
}

static inline void MapLEDStripArgStartLength(CRGB * ledStrip, uint16_t start, uint16_t length)
{
	p_LEDStrip = &ledStrip[start];
	NumLEDs = length;
}

static inline void MapVarsAll(uint32_t * indexPos, bool * direction, uint32_t steps, bool boundaryBehavior, CRGB color1, CRGB color2, CRGBPalette16 * palette, uint32_t special1, uint32_t special2, uint32_t special3, CRGB * ledStripBuffer)
{
	p_IndexPos = indexPos;
	p_Direction = direction;
	Steps = steps;
	BoundaryBehavior = boundaryBehavior;
	Color1 = color1;
	Color2 = color2;
	p_Palette = palette;
	Special1 = special1;
	Special2 = special2;
	Special3 = special3;
	p_LEDStripBuffer = ledStripBuffer;
}

/*-----------------------------------------------------------------------------
  Common Helper Functions
 *----------------------------------------------------------------------------*/
//inclusive of bounds, makes reaching 0 more natural
static inline void HelperIndexStepBound(uint32_t * index, bool * direction, uint32_t stepSize, uint32_t boundLeft, uint32_t boundRight)
{
	if(*direction)
	{
		if ((*index + stepSize) < (boundRight))
		{
			*index += stepSize;
		}
		else
		{
			*direction = 0;
			*index = boundRight;
		}
	}
	else
	{
		if ((*index - stepSize) > (boundLeft))
		{
			*index -= stepSize;
		}
		else
		{
			*direction = 1;
			*index = boundLeft;
		}
	}
}

static inline void HelperIndexStepWrap(uint32_t * index, bool * direction, uint32_t stepSize, uint32_t boundLeft, uint32_t boundRight)
{
	if(*direction)
	{
		if 	((*index + stepSize > boundRight) || (*index + stepSize < boundLeft))
			*index = (*index - boundLeft + stepSize) % (boundRight - boundLeft + 1) + boundLeft;
		else
			*index = *index + stepSize;
		}
	else
	{
		if 	((*index - stepSize < boundLeft) || (*index - stepSize > boundRight))
			*index = (*index - boundLeft - stepSize) % (boundRight - boundLeft + 1) + boundLeft;
		else
			*index = *index + stepSize;
	}
}

static void HelperIndexStep(uint32_t * index, bool * direction, uint32_t stepSize, uint32_t boundLeft, uint32_t boundRight, bool boundaryBehavior)
{
	if (boundaryBehavior)
		HelperIndexStepBound(index, direction, stepSize, boundLeft, boundRight);
	else
		HelperIndexStepWrap(index, direction, stepSize, boundLeft, boundRight);
}

/*-----------------------------------------------------------------------------
  Pattern Algorithms
 *----------------------------------------------------------------------------*/
/******************************************************************************/
/*!
 * @name  LightFX ColorFader
 * @brief Transitions between 2 colors, skips in between colors and can
 * 		include white unlike rainbow bounded.
 */
/******************************************************************************/
void ColorFader()
{
	for (uint16_t n = 0; n < NumLEDs; n++)
	{
		p_LEDStrip[n].r = ((Color1.r * (Steps - *p_IndexPos)) + (Color2.r * *p_IndexPos)) / Steps;
		p_LEDStrip[n].g = ((Color1.g * (Steps - *p_IndexPos)) + (Color2.g * *p_IndexPos)) / Steps;
		p_LEDStrip[n].b = ((Color1.b * (Steps - *p_IndexPos)) + (Color2.b * *p_IndexPos)) / Steps;
	}

	HelperIndexStep(p_IndexPos, p_Direction, 1, 0, Steps, BoundaryBehavior);
}

//void SetColorFaderCycleSteps(uint16_t cycleTime, uint16_t updateInterval)
//{
//	Steps = cycleTime/updateInterval/2;
//}

/******************************************************************************/
/*!
 * @name  LightFX ColorFader one color
 * @brief
 */
/******************************************************************************/
#define COLOR_FADER_STRIP_BOUNDARY 	Special1
void ColorFaderStrip()
{
	for (uint16_t n = 0; n < NumLEDs; n++)
	{
		p_LEDStrip[n].r = ((p_LEDStripBuffer[n].r * (Steps - *p_IndexPos)) + (Color1.r * *p_IndexPos)) / Steps;
		p_LEDStrip[n].g = ((p_LEDStripBuffer[n].g * (Steps - *p_IndexPos)) + (Color1.g * *p_IndexPos)) / Steps;
		p_LEDStrip[n].b = ((p_LEDStripBuffer[n].b * (Steps - *p_IndexPos)) + (Color1.b * *p_IndexPos)) / Steps;
	}

	HelperIndexStep(p_IndexPos, p_Direction, 1, 0, COLOR_FADER_STRIP_BOUNDARY, BoundaryBehavior);
}

/******************************************************************************/
/*!
 * @name ColorWipe
 * @brief Wipes color to index position
 */
/******************************************************************************/
void ColorWipe()
{
	uint16_t n = 0;

	for (; n < *p_IndexPos; n++)
		p_LEDStrip[n] = Color1;
	for (; n < NumLEDs; n++)
		p_LEDStrip[n] = Color2;

	HelperIndexStep(p_IndexPos, p_Direction, 1, 0, NumLEDs, BoundaryBehavior);
}

/******************************************************************************/
/*!
 * @name ColorWipe
 * @brief Wipes color to index position
 */
/******************************************************************************/
void Blink()
{
	if(*p_Direction)
	{
		*p_Direction = 0;
		for (uint16_t n = 0; n < NumLEDs; n++)
			p_LEDStrip[n] = Color1;
	}
	else
	{
		*p_Direction = 1;
		for (uint16_t n = 0; n < NumLEDs; n++)
			p_LEDStrip[n] = Color2;
	}
}

/******************************************************************************/
/*!
 * @name Palette Wipe
 * @brief
 */
/******************************************************************************/
//static uint8_t PaletteWipeSpectrum = 255*2/3; // 2/3 of full spectrum
#define PALETTE_WIPE_SPECTURM 	Special1

void PaletteWipe()
{
	static uint8_t index; // starting position in the palette is usually irrelevant, can be random

    for (uint16_t n = 0; n < NumLEDs; n++)
    	p_LEDStrip[n] = ColorFromPalette(*p_Palette, index + n*PALETTE_WIPE_SPECTURM/NumLEDs);

	index += FAST_LED_PALLETTE_MAX/Steps; //always forward and wrap at 255
	//HelperIndexStep(p_IndexPos, p_Direction, 1, 0, 255, 0);
}

//void SetPaletteWipeSpectrum(uint8_t visible) //255 is 100% of palette spectrum
//{
//	PALETTE_WIPE_SPECTURM = visible;
//}

/*-----------------------------------------------------------------------------
  Map and set common vars before calling pattern function
  Can also use MapVarsAll
 *----------------------------------------------------------------------------*/
void MapColorFaderVars(uint32_t * index, bool * direction, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	p_IndexPos = index;
	p_Direction = direction;
	Color1 = color1;
	Color2 = color2;
	Steps = transitionSteps;
	BoundaryBehavior = boundaryBehavior;
}

void MapColorWipeVars(uint32_t * indexPos, bool * direction, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	p_IndexPos = indexPos;
	p_Direction = direction;
	Color1 = color1;
	Color2 = color2;
	BoundaryBehavior = boundaryBehavior;
}

void MapBlinkVars(bool * direction, CRGB color1, CRGB color2)
{
	p_Direction = direction;
	Color1 = color1;
	Color2 = color2;
}

void MapPaletteWipeVars(uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum)
{
	Steps = cycleSteps;
	p_Palette = palette;
	PALETTE_WIPE_SPECTURM = visibleSpectrum; //255 is 100% of palette spectrum
}


/*-----------------------------------------------------------------------------
  Proc functions will map and call pattern update
 *----------------------------------------------------------------------------*/
/******************************************************************************/
/*!
 * @name LightFX_SetOff
 * @brief Quick off
 */
/******************************************************************************/
void LightFX_SetOff(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength)
{
	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
	for (uint16_t n = 0; n < NumLEDs; n++)
		p_LEDStrip[n] = 0;
}

void LightFX_ProcColorFader(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * index, bool * direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
	MapColorFaderVars(index, direction, steps, boundaryBehavior, color1, color2);
	ColorFader();
}

void LightFX_ProcColorWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * indexPos, bool * direction, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
	MapColorWipeVars(indexPos, direction, boundaryBehavior, color1, color2);
	ColorWipe();
}

void LightFX_ProcBlink(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool * direction, CRGB color1, CRGB color2)
{
	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
	MapBlinkVars(direction, color1, color2);
	Blink();
}

void LightFX_ProcPaletteWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum)
{
	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
	MapPaletteWipeVars(cycleSteps, palette, visibleSpectrum);
	PaletteWipe();
}

/*-----------------------------------------------------------------------------
  Load vars from a FX - loads all vars including unused
 *----------------------------------------------------------------------------*/
void LightFX_LoadMemory(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);
	MapVarsAll
			(
			&((LIGHT_FX_T *)fx)->IndexPos,
			&((LIGHT_FX_T *)fx)->Direction,
			((LIGHT_FX_T *)fx)->Steps,
			((LIGHT_FX_T *)fx)->BoundaryBehavior,
			((LIGHT_FX_T *)fx)->Color1,
			((LIGHT_FX_T *)fx)->Color2,
			((LIGHT_FX_T *)fx)->p_Palette,
			((LIGHT_FX_T *)fx)->Special1,
			((LIGHT_FX_T *)fx)->Special2,
			((LIGHT_FX_T *)fx)->Special3,
			((LIGHT_FX_T *)fx)->p_LEDStripBuffer
			);
}

void LightFX_MapFX(LIGHT_FX_T * fx)
{
	MapLEDStrip(fx->p_LEDStrip, fx->NumLEDs);
	MapVarsAll(&fx->IndexPos, &fx->Direction, fx->Steps, fx->BoundaryBehavior, fx->Color1, fx->Color2, fx->p_Palette, fx->Special1, fx->Special2, fx->Special3, fx->p_LEDStripBuffer);
}

void LightFX_ProcFX(LIGHT_FX_T * fx)
{
	LightFX_MapFX(fx);
	fx->Pattern();
}

void LightFX_InitFX(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength)
{
	fx->Pattern = ColorFader;
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
}

/*-----------------------------------------------------------------------------
  Set a FX - using a particular function with only the relevant vars
  avoids confusion for the user
 *----------------------------------------------------------------------------*/
void LightFX_SetFXColorFader(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t patternIndex, bool direction, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	fx->Pattern = ColorFader;
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
	fx->IndexPos = patternIndex;
	fx->Direction = direction;
	fx->Steps = transitionSteps;
	fx->BoundaryBehavior = boundaryBehavior;
	fx->Color1 = color1;
	fx->Color2 = color2;
}

void LightFX_SetFXColorFaderStrip(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t patternIndex, bool direction, uint16_t boundarySteps, uint16_t totalTransitionSteps, bool boundaryBehavior, CRGB * ledStripInitialColor, CRGB color)
{
	fx->Pattern = ColorFaderStrip;
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
	fx->IndexPos = patternIndex;
	fx->Direction = direction;
	fx->Steps = totalTransitionSteps;
	fx->BoundaryBehavior = boundaryBehavior;
	fx->p_LEDStripBuffer = ledStripInitialColor;
	fx->Color1 = color;
	fx->COLOR_FADER_STRIP_BOUNDARY = boundarySteps;
}

void LightFX_SetFXColorWipe(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t patternIndex, bool direction, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	fx->Pattern = ColorWipe;
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
	fx->IndexPos = patternIndex;
	fx->Direction = direction;
	fx->BoundaryBehavior = boundaryBehavior;
	fx->Color1 = color1;
	fx->Color2 = color2;
}

void LightFX_SetFXBlink(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool direction, CRGB color1, CRGB color2)
{
	fx->Pattern = Blink;
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
	fx->Direction = direction;
	fx->Color1 = color1;
	fx->Color2 = color2;
}

void LightFX_SetFXPaletteWipe(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum)
{
	fx->Pattern = PaletteWipe;
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
	fx->Steps = cycleSteps;
	fx->p_Palette = palette;
	fx->PALETTE_WIPE_SPECTURM = visibleSpectrum; //255 is 100% of palette spectrum
}
