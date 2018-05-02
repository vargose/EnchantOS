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

    @brief LED strip pattern primary algorithms.
*/
/**************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "FastLED.h"
#include "LightFX.h"

#define FASTLED_PALLETTE_MAX 	255
#define FASTLED_COLOR_MAX		255
/*-----------------------------------------------------------------------------
  Common LED Strip Variables -
  Use these primitives because LED pattern algorithms are naturally written
  using these parameters
 *----------------------------------------------------------------------------*/
static CRGB * 	p_LEDStrip;
static uint16_t NumLEDs;

static inline void MapLEDStrip(CRGB * ledStart, uint16_t numLEDs)
{
	p_LEDStrip = ledStart;
	NumLEDs = numLEDs;
}

//static inline void MapLEDStripArgStartEnd(CRGB * ledStrip, uint16_t start, uint16_t end)
//{
//	p_LEDStrip = &ledStrip[start];
//	NumLEDs = end - start + 1;
//}
//
//static inline void MapLEDStripArgStartLength(CRGB * ledStrip, uint16_t start, uint16_t length)
//{
//	p_LEDStrip = &ledStrip[start];
//	NumLEDs = length;
//}

//static inline void MapVarsAll(uint32_t * indexPos, bool * direction, uint32_t steps, bool boundaryBehavior, CRGB color1, CRGB color2, CRGBPalette16 * palette, uint32_t special1, uint32_t special2, uint32_t special3, CRGB * ledStripBuffer)
//{
//	p_IndexPos = indexPos;
//	p_Direction = direction;
//	Steps = steps;
//	BoundaryBehavior = boundaryBehavior;
//	Color1 = color1;
//	Color2 = color2;
//	p_Palette = palette;
//	Special1 = special1;
//	Special2 = special2;
//	Special3 = special3;
//	p_LEDStripBuffer = ledStripBuffer;
//}

/*-----------------------------------------------------------------------------
  Common Pattern Variables - Shared by patterns
 *----------------------------------------------------------------------------*/
//Callee Save/Modify
static uint32_t			* p_IndexPos;
static bool				* p_Direction;
static void 			* p_Special;
static CRGBPalette16 	* p_PaletteSpecial; // non constant Palette
static float			* p_FloatPos;

//Caller Save
static uint32_t	Steps;
static bool		BoundaryBehavior;
static CRGB 	Color1;
static CRGB 	Color2;
static const CRGBPalette16 * p_Palette;
static const CRGB * p_LEDStripState;
static uint32_t Special1;
static uint32_t	Special2;
static uint32_t	Special3;
static bool		BoolSpecial;

/*-----------------------------------------------------------------------------
  Misc Set Functions
 *----------------------------------------------------------------------------*/
/******************************************************************************/
/*!
 * @name LightFX_SetOff
 * @brief Quick off
 */
/******************************************************************************/
void LightFX_SetOff(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength)
{
	MapLEDStrip(&ledStrip[ledStart], ledLength);
	for (uint16_t n = 0; n < NumLEDs; n++)
		p_LEDStrip[n] = 0;
}

void LightFX_SetContextOff()
{
	for (uint16_t n = 0; n < NumLEDs; n++)
		p_LEDStrip[n] = 0;
}

void LightFX_SetSolid(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB color)
{
	MapLEDStrip(&ledStrip[ledStart], ledLength);
	for (uint16_t n = 0; n < NumLEDs; n++)
		p_LEDStrip[n] = color;
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
			*index = (*index - boundLeft + stepSize) % (boundRight - boundLeft + 1) + boundLeft; //(boundRight - boundLeft + 1) to include bounds
		else
			*index = *index + stepSize;
	}
	else
	{
		if 	((*index - stepSize < boundLeft) || (*index - stepSize > boundRight))
			*index = (*index - boundLeft - stepSize) % (boundRight - boundLeft + 1) + boundLeft;
		else
			*index = *index - stepSize;
	}
}

static inline void HelperIndexStep(uint32_t * index, bool * direction, uint32_t stepSize, uint32_t boundLeft, uint32_t boundRight, bool boundaryBehavior)
{
	if (boundaryBehavior)
		HelperIndexStepBound(index, direction, stepSize, boundLeft, boundRight);
	else
		HelperIndexStepWrap(index, direction, stepSize, boundLeft, boundRight);
}

static inline void HelperPosStepBound(float * pos, bool * direction, float stepSize, float boundLeft, float boundRight)
{
	if(*direction)
	{
		if ((*pos + stepSize) < (boundRight))
		{
			*pos += stepSize;
		}
		else
		{
			*direction = 0;
			*pos = boundRight;
		}
	}
	else
	{
		if ((*pos - stepSize) > (boundLeft))
		{
			*pos -= stepSize;
		}
		else
		{
			*direction = 1;
			*pos = boundLeft;
		}
	}
}

static inline void HelperPosStepWrap(float * pos, bool * direction, float stepSize, float boundLeft, float boundRight)
{
	if(*direction)
	{
		if 	((*pos + stepSize > boundRight) || (*pos + stepSize < boundLeft))
			*pos = fmod((*pos - boundLeft + stepSize), (boundRight - boundLeft + 1)) + boundLeft;
		else
			*pos = *pos + stepSize;
	}
	else
	{
		if 	((*pos - stepSize < boundLeft) || (*pos - stepSize > boundRight))
			*pos = fmod((*pos - boundLeft - stepSize), (boundRight - boundLeft + 1)) + boundLeft;
		else
			*pos = *pos - stepSize;
	}
}

static inline void HelperPosStepHardBound(float * pos, bool * direction, float stepSize, float boundLeft, float boundRight)
{
	if(*direction)
	{
		if (*pos + stepSize < boundRight)	*pos += stepSize;
		else								*pos = boundRight;
	}
	else
	{
		if (*pos - stepSize > boundLeft) 	*pos -= stepSize;
		else								*pos = boundLeft;
	}
}

static inline void HelperPosStep(float * pos, bool * direction, float stepSize, float boundLeft, float boundRight, bool boundaryBehavior)
{
	if (boundaryBehavior)
		HelperPosStepBound(pos, direction, stepSize, boundLeft, boundRight);
	else
		HelperPosStepWrap(pos, direction, stepSize, boundLeft, boundRight);
}


//inline static void HelperPosStep(float * pos, bool * direction, float stepSize, float boundLeft, float boundRight, bool hardBound, bool boundaryBehavior)
//{
//	if(*direction)
//	{
//		if (*pos + stepSize > boundRight && *pos + stepSize < boundLeft)
//		{
//			if (hardBound)
//				*pos = boundRight;
//			else
//			{
//				if (boundaryBehavior) //rebound // can take on value of boundary twice this way
//				{
//					*pos = boundRight;
//					*direction = !*direction;
//				}
//				else //wrap
//					*pos = fmod((*pos - boundLeft + stepSize), (boundRight - boundLeft + 1)) + boundLeft;
//			}
//		}
//		else
//			*pos += stepSize;
//
//	}
//}


/*-----------------------------------------------------------------------------
  Pattern Algorithms
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
	The pattern algorithm -
		void Pattern()
	Load the FX variables into this module, so it can be called in the void-void format above
		void LoadPattern(void * fx)
	Store the configuration in to the LIGHT_FX_T
		void LightFX_InitFXPattern(LIGHT_FX_T * fx, FX_PATTERN_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, unique vars)
	Reconfigure options for the pattern. LightFX_InitFXPattern must be called before this can be used.
		void LightFX_SetFXPattern(LIGHT_FX_T * fx, unique vars)
	Functions for setting any unique options
		void LightFX_SetFXPatternUnqiueOption(LIGHT_FX_T * fx, option);
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

//void inline MapColorFaderVars(uint32_t * index, bool * direction, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2)
//{
//	p_IndexPos = index;
//	p_Direction = direction;
//	Color1 = color1;
//	Color2 = color2;
//	Steps = transitionSteps;
//	BoundaryBehavior = boundaryBehavior;
//}

//void LightFX_ProcColorFader(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * index, bool * direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2)
//{
//	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
//	MapColorFaderVars(index, direction, steps, boundaryBehavior, color1, color2);
//	p_IndexPos = index;
//	p_Direction = direction;
//	Color1 = color1;
//	Color2 = color2;
//	Steps = transitionSteps;
//	BoundaryBehavior = boundaryBehavior;
//	ColorFader();
//}

void LoadColorFader(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_IndexPos 			= &((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Index;
	p_Direction 		= &((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	Color1 				= ((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color1;
	Color2 				= ((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color2;
	Steps 				= ((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->TransitionSteps;
	BoundaryBehavior 	= ((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->BoundaryBehavior;
}

void LightFX_InitFXColorFader(LIGHT_FX_T * fx, FX_COLOR_FADER_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	fx->Pattern 	= ColorFader;
	fx->LoadFX 		= LoadColorFader;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars	 	= vars;

	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Index				= startingIndex;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Direction 			= startingDirection;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->TransitionSteps  	= transitionSteps;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->BoundaryBehavior 	= boundaryBehavior;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Color1 			= color1;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Color2 			= color2;
}

void LightFX_SetFXColorFader(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Index 				= startingIndex;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Direction 			= startingDirection;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->TransitionSteps 	= transitionSteps;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->BoundaryBehavior 	= boundaryBehavior;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Color1 			= color1;
	((FX_COLOR_FADER_VARS_T *)(fx->p_Vars))->Color2 			= color2;
}

void LightFX_SetFXColorFaderCycleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{
	((FX_COLOR_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->TransitionSteps = ticksPerSecond*cycleTimeMs/1000/2;
}

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
		p_LEDStrip[n].r = ((p_LEDStripState[n].r * (Steps - *p_IndexPos)) + (Color1.r * *p_IndexPos)) / Steps;
		p_LEDStrip[n].g = ((p_LEDStripState[n].g * (Steps - *p_IndexPos)) + (Color1.g * *p_IndexPos)) / Steps;
		p_LEDStrip[n].b = ((p_LEDStripState[n].b * (Steps - *p_IndexPos)) + (Color1.b * *p_IndexPos)) / Steps;
	}

	HelperIndexStep(p_IndexPos, p_Direction, 1, 0, COLOR_FADER_STRIP_BOUNDARY, BoundaryBehavior);
}

void LoadColorFaderStrip(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_IndexPos 					= &((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Index;
	p_Direction 				= &((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	Color1 						= ((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color1;
	Steps 						= ((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->TransitionSteps;
	BoundaryBehavior 			= ((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->BoundaryBehavior;
	p_LEDStripState 			= ((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_LEDStripColor;
	COLOR_FADER_STRIP_BOUNDARY 	= ((FX_COLOR_FADER_STRIP_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Boundary;
}

void LightFX_InitFXColorFaderStrip(LIGHT_FX_T * fx, FX_COLOR_FADER_STRIP_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, const CRGB * ledStripInitialColor, CRGB color1)
{
	fx->Pattern 	= ColorFaderStrip;
	fx->LoadFX 		= LoadColorFaderStrip;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars	 	= vars;

	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Index					= startingIndex;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Direction 				= startingDirection;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->TransitionSteps  		= fullTransitionSteps;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->BoundaryBehavior 		= boundaryBehavior;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->p_LEDStripColor			= ledStripInitialColor;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Color1 					= color1;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Boundary					= boundarySteps;
}

void LightFX_SetFXColorFaderStrip(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, const CRGB * ledStripInitialColor, CRGB color1)
{
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Index					= startingIndex;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Direction 				= startingDirection;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->TransitionSteps  		= fullTransitionSteps;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->BoundaryBehavior 		= boundaryBehavior;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->p_LEDStripColor			= ledStripInitialColor;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Color1 					= color1;
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->Boundary					= boundarySteps;
}

void LightFX_SetFXColorFaderStripCycleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{
	((FX_COLOR_FADER_STRIP_VARS_T *)(fx->p_Vars))->TransitionSteps = ticksPerSecond*cycleTimeMs/1000/2;
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

//void inline MapColorWipeVars(uint32_t * indexPos, bool * direction, bool boundaryBehavior, CRGB color1, CRGB color2)
//{
//	p_IndexPos = indexPos;
//	p_Direction = direction;
//	Color1 = color1;
//	Color2 = color2;
//	BoundaryBehavior = boundaryBehavior;
//}

//void LightFX_ProcColorWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * indexPos, bool * direction, bool boundaryBehavior, CRGB color1, CRGB color2)
//{
//	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
//	MapColorWipeVars(indexPos, direction, boundaryBehavior, color1, color2);
//	ColorWipe();
//}
//

void LoadColorWipe(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_IndexPos 					= &((FX_COLOR_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Index;
	p_Direction 				= &((FX_COLOR_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	BoundaryBehavior 			= ((FX_COLOR_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->BoundaryBehavior;
	Color1 						= ((FX_COLOR_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color1;
	Color2 						= ((FX_COLOR_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color2;
}

void LightFX_InitFXColorWipe(LIGHT_FX_T * fx, FX_COLOR_WIPE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	fx->Pattern 	= ColorWipe;
	fx->LoadFX 	= LoadColorWipe;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars	 	= vars;

	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Index				= startingIndex;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Direction 			= startingDirection;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->BoundaryBehavior 	= boundaryBehavior;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Color1 				= color1;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Color2 				= color2;
}

void LightFX_SetFXColorWipe(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2)
{
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Index				= startingIndex;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Direction 			= startingDirection;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->BoundaryBehavior 	= boundaryBehavior;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Color1 				= color1;
	((FX_COLOR_WIPE_VARS_T *)(fx->p_Vars))->Color2 				= color2;
}


/******************************************************************************/
/*!
 * @name Blink
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

//void inline MapBlinkVars(bool * direction, CRGB color1, CRGB color2)
//{
//	p_Direction = direction;
//	Color1 = color1;
//	Color2 = color2;
//}

//void LightFX_ProcBlink(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool * direction, CRGB color1, CRGB color2)
//{
//	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
//	MapBlinkVars(direction, color1, color2);
//	Blink();
//}

void LoadBlink(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_Direction 	= &((FX_BLINK_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	Color1 			= ((FX_BLINK_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color1;
	Color2 			= ((FX_BLINK_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Color2;
}

void LightFX_InitFXBlink(LIGHT_FX_T * fx, FX_BLINK_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool startingDirection, CRGB color1, CRGB color2)
{
	fx->Pattern 	= Blink;
	fx->LoadFX 	= LoadBlink;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars	 	= vars;

	((FX_BLINK_VARS_T *)(fx->p_Vars))->Direction 	= startingDirection;
	((FX_BLINK_VARS_T *)(fx->p_Vars))->Color1 		= color1;
	((FX_BLINK_VARS_T *)(fx->p_Vars))->Color2		= color2;
}

void LightFX_SetFXBlink(LIGHT_FX_T * fx, bool startingDirection, CRGB color1, CRGB color2)
{
	((FX_BLINK_VARS_T *)(fx->p_Vars))->Direction 	= startingDirection;
	((FX_BLINK_VARS_T *)(fx->p_Vars))->Color1 		= color1;
	((FX_BLINK_VARS_T *)(fx->p_Vars))->Color2		= color2;
}


/******************************************************************************/
/*!
 * @name Palette Wipe
 * @brief
 */
/******************************************************************************/
#define PALETTE_WIPE_SPECTURM 	Special1	// 255 is 100% of palette spectrum

void PaletteWipe()
{
    for (uint16_t n = 0; n < NumLEDs; n++)
    	p_LEDStrip[n] = ColorFromPalette(*p_Palette, *p_IndexPos + n*PALETTE_WIPE_SPECTURM/NumLEDs);

	HelperIndexStep(p_IndexPos, p_Direction, (FASTLED_PALLETTE_MAX + 1)/Steps, 0, FASTLED_PALLETTE_MAX, BoundaryBehavior);
}

//void inline MapPaletteWipeVars(uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum)
//{
//	Steps = cycleSteps;
//	p_Palette = palette;
//	PALETTE_WIPE_SPECTURM = visibleSpectrum; //255 is 100% of palette spectrum
//}

//void LightFX_ProcPaletteWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum)
//{
//	MapLEDStripArgStartLength(ledStrip, ledStart, ledLength);
//	MapPaletteWipeVars(cycleSteps, palette, visibleSpectrum);
//	PaletteWipe();
//}

void LoadPaletteWipe(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_IndexPos 				= &((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Index;
	p_Direction 			= &((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	Steps 					= ((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Steps;
	BoundaryBehavior 		= ((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->BoundaryBehavior;
	p_Palette 				= ((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_Palette;
	PALETTE_WIPE_SPECTURM 	= ((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->VisibleSpectrum;
}

void LightFX_InitFXPaletteWipe(LIGHT_FX_T * fx, FX_PALETTE_WIPE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum)
{
	fx->Pattern 	= PaletteWipe;
	fx->LoadFX 	= LoadPaletteWipe;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars 		= vars;

	vars->Index 				= startingIndex;
	vars->Direction 			= startingDirection;
	vars->Steps					= stepsPerCycle;
	vars->BoundaryBehavior		= boundaryBehavior;
	vars->p_Palette				= palette;
	vars->VisibleSpectrum		= visibleSpectrum;
}

void LightFX_SetFXPaletteWipe(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum)
{
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Index 				= startingIndex;
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction			= startingDirection;
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Steps				= stepsPerCycle;
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->BoundaryBehavior 	= boundaryBehavior;
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_Palette			= palette;
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->VisibleSpectrum		= visibleSpectrum;
}

void LightFX_SetFXPaletteWipeSpectrum(LIGHT_FX_T * fx, uint8_t visibleSpectrum)
{
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->VisibleSpectrum	 = visibleSpectrum;
}

void LightFX_SetFXPaletteWipeCyleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{
	((FX_PALETTE_WIPE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Steps = ticksPerSecond*cycleTimeMs/1000;
}

/******************************************************************************/
/*!
 * @name Palette Fader
 * @brief
 */
/******************************************************************************/
void PaletteFader()
{
    for (uint16_t n = 0; n < NumLEDs; n++)
    	p_LEDStrip[n] =  ColorFromPalette(*p_Palette, *p_IndexPos);

	HelperIndexStep(p_IndexPos, p_Direction, FASTLED_PALLETTE_MAX/Steps, 0, FASTLED_PALLETTE_MAX, BoundaryBehavior);
}

void LoadPaletteFader(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_IndexPos 				= &((FX_PALETTE_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Index;
	p_Direction 			= &((FX_PALETTE_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	Steps 					= ((FX_PALETTE_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Steps;
	BoundaryBehavior 		= ((FX_PALETTE_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->BoundaryBehavior;
	p_Palette 				= ((FX_PALETTE_FADER_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_Palette;
}

void LightFX_InitFXPaletteFader(LIGHT_FX_T * fx, FX_PALETTE_FADER_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette)
{
	fx->Pattern 	= PaletteFader;
	fx->LoadFX 		= LoadPaletteFader;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars 		= vars;

	vars->Index 				= startingIndex;
	vars->Direction 			= startingDirection;
	vars->Steps					= stepsPerCycle;
	vars->BoundaryBehavior		= boundaryBehavior;
	vars->p_Palette				= palette;
}

void LightFX_SetFXPaletteFader(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette)
{
	((FX_PALETTE_FADER_VARS_T *)fx->p_Vars)->Index 				= startingIndex;
	((FX_PALETTE_FADER_VARS_T *)fx->p_Vars)->Direction			= startingDirection;
	((FX_PALETTE_FADER_VARS_T *)fx->p_Vars)->Steps				= stepsPerCycle;
	((FX_PALETTE_FADER_VARS_T *)fx->p_Vars)->BoundaryBehavior 	= boundaryBehavior;
	((FX_PALETTE_FADER_VARS_T *)fx->p_Vars)->p_Palette			= palette;
}

void LightFX_SetFXPaletteFaderCyleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond)
{
	((FX_PALETTE_FADER_VARS_T *)fx->p_Vars)->Steps = ticksPerSecond*cycleTimeMs/1000;
}


/******************************************************************************/
/*!
 * @name Fire2012
 * @brief Adapted from Fire2012 by Mark Kriegsman
 */
/******************************************************************************/
// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
////
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
//#define COOLING  55
#define COOLING  Special1

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
//#define SPARKING 120
#define SPARKING Special2

void Fire2012()
{
// Array of temperature readings at each simulation cell
  //static byte heat[NUM_LEDs];
  uint8_t * heat = (uint8_t *)p_Special;

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NumLEDs; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NumLEDs) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NumLEDs - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NumLEDs; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( !(*p_Direction) ) {
        pixelnumber = (NumLEDs-1) - j;
      } else {
        pixelnumber = j;
      }
      p_LEDStrip[pixelnumber] = color;
    }
}

void LoadFire2012(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_Direction 			= &((FX_FIRE2012_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	COOLING 				= ((FX_FIRE2012_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Cooling;
	SPARKING 				= ((FX_FIRE2012_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Sparking;
	p_Special				= ((FX_FIRE2012_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_Heat;
}

void LightFX_InitFXFire2012(LIGHT_FX_T * fx, FX_FIRE2012_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool direction, uint8_t cooling, uint8_t sparkling, uint8_t * ledsBuffer)
{
	fx->Pattern 	= Fire2012;
	fx->LoadFX 		= LoadFire2012;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars 		= vars;

	vars->Direction 		= direction;
	vars->Cooling 			= cooling;		// default is 55
	vars->Sparking			= sparkling;	// default is 120

	vars->p_Heat 			= ledsBuffer; 	// array must be same length as LED strip
}

void LightFX_InitFXFire2012Default(LIGHT_FX_T * fx, FX_FIRE2012_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint8_t * ledsBuffer)
{
	fx->Pattern 	= Fire2012;
	fx->LoadFX 		= LoadFire2012;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars 		= vars;

	vars->Direction 		= 1;
	vars->Cooling 			= 55;
	vars->Sparking			= 120;

	vars->p_Heat 			= ledsBuffer; 	// array must be same length as LED strip
}

void LightFX_SetFXFire2012(LIGHT_FX_T * fx, bool direction, uint8_t cooling, uint8_t sparkling)
{
	((FX_FIRE2012_VARS_T *)fx->p_Vars)->Direction	= direction;
	((FX_FIRE2012_VARS_T *)fx->p_Vars)->Cooling		= cooling;
	((FX_FIRE2012_VARS_T *)fx->p_Vars)->Sparking 	= sparkling;
}

void LightFX_SetFXFire2012Restart(LIGHT_FX_T * fx)
{
    for( int i = 0; i < NumLEDs; i++) {
    	((FX_FIRE2012_VARS_T *)fx->p_Vars)->p_Heat[i] = 0;
    }
}

/******************************************************************************/
/*!
 * @name Fire2012WithPalette
 * @brief Adapted from Fire2012WithPalette by Mark Kriegsman
 */
/******************************************************************************/
// Fire2012 with programmable Color Palette
//
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".

#define USE_DYNAMIC_PALETTE BoolSpecial

inline void Fire2012DynamicPalette()
{
	// Fourth, the most sophisticated: this one sets up a new palette every
	// time through the loop, based on a hue that changes every time.
	// The palette is a gradient from black, to a dark color based on the hue,
	// to a light color based on the hue, to white.
	//
	static uint8_t hue = 0;
	hue++;
	CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
	CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
	*p_PaletteSpecial = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
////
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
//#define COOLING  55
#define COOLING  Special1

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
//#define SPARKING 120
#define SPARKING  Special2

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
	  //static byte heat[NUM_LEDS];
	  uint8_t * heat = (uint8_t *)p_Special;

	  if (USE_DYNAMIC_PALETTE)
	  {
		  p_Palette = p_PaletteSpecial;
		  Fire2012DynamicPalette();
	  }

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NumLEDs; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NumLEDs) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NumLEDs - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NumLEDs; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( *p_Palette, colorindex);
      int pixelnumber;
      if( !(*p_Direction) ) {
        pixelnumber = (NumLEDs-1) - j;
      } else {
        pixelnumber = j;
      }
      p_LEDStrip[pixelnumber] = color;
    }
}

void LoadFire2012WithPalette(void * fx)
{
	MapLEDStrip(((LIGHT_FX_T *)fx)->p_LEDStrip, ((LIGHT_FX_T *)fx)->NumLEDs);

	p_Direction 			= &((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Direction;
	COOLING 				= ((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Cooling;
	SPARKING 				= ((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->Sparking;
	p_Special				= ((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_Heat;
	p_Palette				= ((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->p_Palette;			//Use this const palette unless dynamic palette is selected
	USE_DYNAMIC_PALETTE		= ((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->UseDynamicPalette;
	p_PaletteSpecial		= &((FX_FIRE2012_WITH_PALETTE_VARS_T *)((LIGHT_FX_T *)fx)->p_Vars)->DynamicPalette;
}

void LightFX_InitFXFire2012WithPalette(LIGHT_FX_T * fx, FX_FIRE2012_WITH_PALETTE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool direction, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t cooling, uint8_t sparkling, uint8_t * ledsBuffer)
{
	fx->Pattern 	= Fire2012WithPalette;
	fx->LoadFX 		= LoadFire2012WithPalette;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars 		= vars;

	vars->p_Heat 			= ledsBuffer; 	// array must be same length as LED strip

	vars->Direction 		= direction;
	vars->Cooling 			= cooling;		// default is 55
	vars->Sparking			= sparkling;	// default is 120
	vars->p_Palette			= initialPalette; // make a copy of palette, the palette might change
	vars->UseDynamicPalette = useDynamicPalette;
}

void LightFX_InitFXFire2012WithPaletteDefault(LIGHT_FX_T * fx, FX_FIRE2012_WITH_PALETTE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t * ledsBuffer)
{
	fx->Pattern 	= Fire2012WithPalette;
	fx->LoadFX 		= LoadFire2012WithPalette;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars 		= vars;

	vars->p_Heat 			= ledsBuffer; 			// array must be same length as LED strip

	vars->Direction 		= 1;
	vars->Cooling 			= 55;
	vars->Sparking			= 120;
	vars->p_Palette			= initialPalette;		 // make a copy of palette, the palette might change
	vars->UseDynamicPalette = useDynamicPalette;
}

void LightFX_SetFXFire2012WithPalette(LIGHT_FX_T * fx, bool direction, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t cooling, uint8_t sparkling)
{
	((FX_FIRE2012_WITH_PALETTE_VARS_T *)fx->p_Vars)->Direction			= direction;
	((FX_FIRE2012_WITH_PALETTE_VARS_T *)fx->p_Vars)->Cooling			= cooling;
	((FX_FIRE2012_WITH_PALETTE_VARS_T *)fx->p_Vars)->Sparking 			= sparkling;
	((FX_FIRE2012_WITH_PALETTE_VARS_T *)fx->p_Vars)->p_Palette			= initialPalette; // make a copy of palette, the palette might change
	((FX_FIRE2012_WITH_PALETTE_VARS_T *)fx->p_Vars)->UseDynamicPalette 	= useDynamicPalette;
}

void LightFX_SetFXFire2012WithPaletteRestart(LIGHT_FX_T * fx)
{
    for( int i = 0; i < NumLEDs; i++) {
    	((FX_FIRE2012_WITH_PALETTE_VARS_T *)fx->p_Vars)->p_Heat[i] = 0;
    }
}

/******************************************************************************/
/*!
 * @name Blade Scroll
 * @brief Similar to color wipe, with fading intensity tip section
 */
/******************************************************************************/
#define BLADE_SCROLL_TIP_LENGTH 	Special1

void BladeScroll()
{
	uint16_t n = 0;

	for (; n < (*p_FloatPos - BLADE_SCROLL_TIP_LENGTH); n++) // before tip start is full brightness or color1
		p_LEDStrip[n] = Color1;
	for (; n < *p_FloatPos && n < NumLEDs; n++) // tip section is linear fade. try non linear later
	{
		//p_LEDStrip[n] =  FASTLED_COLOR_MAX - FASTLED_COLOR_MAX*(n - *p_FloatPos)/BLADE_SCROLL_TIP_LENGTH;
		p_LEDStrip[n].r = ((Color1.r * (BLADE_SCROLL_TIP_LENGTH - (n -*p_FloatPos))) + (Color2.r * (n - *p_FloatPos))) / BLADE_SCROLL_TIP_LENGTH;
		p_LEDStrip[n].g = ((Color1.g * (BLADE_SCROLL_TIP_LENGTH - (n -*p_FloatPos))) + (Color2.g * (n - *p_FloatPos))) / BLADE_SCROLL_TIP_LENGTH;
		p_LEDStrip[n].b = ((Color1.b * (BLADE_SCROLL_TIP_LENGTH - (n -*p_FloatPos))) + (Color2.b * (n - *p_FloatPos))) / BLADE_SCROLL_TIP_LENGTH;
	}
	for (; n < NumLEDs; n++) // past tip is off or color 2
		p_LEDStrip[n] = Color2;

	HelperPosStepHardBound(p_FloatPos, p_Direction, (float)(NumLEDs + BLADE_SCROLL_TIP_LENGTH) / Steps, 0.0f, NumLEDs + BLADE_SCROLL_TIP_LENGTH);
}

void LoadBladeScroll(LIGHT_FX_T * fx)
{
	MapLEDStrip(fx->p_LEDStrip, fx->NumLEDs);

	p_FloatPos 					= &((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->Position;
	p_Direction 				= &((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->Direction;
	Steps						= ((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->Steps;
	BoundaryBehavior 			= ((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->BoundaryBehavior;
	Color1 						= ((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->Color1;
	Color2 						= ((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->Color2;
	BLADE_SCROLL_TIP_LENGTH		= ((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->TipLength;
}

void LightFX_InitBladeScroll(LIGHT_FX_T * fx, FX_BLADE_SCROLL_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, float pos, bool direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2, uint16_t tipLength)
{
	fx->Pattern 	= BladeScroll;
	fx->LoadFX 		= (void (*)(void *))LoadBladeScroll;
	fx->p_LEDStrip 	= &ledStrip[ledStart];
	fx->NumLEDs 	= ledLength;
	fx->p_Vars	 	= vars;

	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Position				= pos;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Direction 			= direction;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->BoundaryBehavior 		= boundaryBehavior; //not used
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Color1 				= color1;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Color2 				= color2;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Steps					= steps;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->TipLength 			= tipLength;
}

void LightFX_SetBladeScroll(LIGHT_FX_T * fx, float pos, bool direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2, uint16_t tipLength)
{
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Position				= pos;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Direction 			= direction;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->BoundaryBehavior 		= boundaryBehavior;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Color1 				= color1;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Color2 				= color2;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->Steps					= steps;
	((FX_BLADE_SCROLL_VARS_T *)(fx->p_Vars))->TipLength 			= tipLength;
}

void LightFX_SetBladeScrollTipLength(LIGHT_FX_T * fx, uint16_t len)
{
	((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->TipLength = len;
}

void LightFX_SetBladeScrollTime(LIGHT_FX_T * fx, uint32_t scrollTimeMs, uint16_t tickPerSecond)
{
	((FX_BLADE_SCROLL_VARS_T *)fx->p_Vars)->Steps = scrollTimeMs*tickPerSecond/1000;
}


/*-----------------------------------------------------------------------------
  Common LightFX Init
 *----------------------------------------------------------------------------*/
// Usually the FX position on the LED strip doesnt need to change
void LightFX_InitFXMap(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength)
{
	fx->p_LEDStrip = &ledStrip[ledStart];
	fx->NumLEDs = ledLength;
}
