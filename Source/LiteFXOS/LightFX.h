/**************************************************************************/
/*!
    @file     LightFX.h
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
#ifndef SOURCE_LITEFXOS_LIGHTFX_H_
#define SOURCE_LITEFXOS_LIGHTFX_H_

typedef struct
{
	void (*Pattern)(void);	//Pattern
	//refactor this to LightFX type later
	void (*LoadFX)(void * LightFX); // Function to load itself/context

	CRGB * p_LEDStrip;	//Mapping
	uint16_t NumLEDs;	//Mapping

	void * p_Vars; // Pattern Vars	//Using an aggregation model, rather than inheritance, to allows us to make an uniform array of LightFX
} LIGHT_FX_T;

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
void LightFX_InitFXMap(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength);
void LightFX_SetOff(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength);
void LightFX_SetContextOff();
void LightFX_SetSolid(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB color);

/******************************************************************************/
/*!
 * @name ColorFader
 * @brief
 */
/******************************************************************************/
typedef struct
{
	uint32_t	Index;
	bool		Direction;
	uint32_t	TransitionSteps;
	bool		BoundaryBehavior;
	CRGB 		Color1;
	CRGB 		Color2;
} FX_COLOR_FADER_VARS_T;

//void LightFX_ProcColorFader(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * index, bool * direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_InitFXColorFader(LIGHT_FX_T * fx, FX_COLOR_FADER_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_SetFXColorFader(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_SetFXColorFaderCycleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	uint32_t	Index;
	bool		Direction;
	uint32_t	TransitionSteps;
	bool		BoundaryBehavior;
	CRGB 		Color1;
	const CRGB * p_LEDStripColor;
	uint32_t	Boundary;
} FX_COLOR_FADER_STRIP_VARS_T;

void LightFX_InitFXColorFaderStrip(LIGHT_FX_T * fx, FX_COLOR_FADER_STRIP_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, const CRGB * ledStripInitialColor, CRGB color1);
void LightFX_SetFXColorFaderStrip(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t boundarySteps, uint16_t fullTransitionSteps, bool boundaryBehavior, const CRGB * ledStripInitialColor, CRGB color1);
void LightFX_SetFXColorFaderStripCycleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	uint32_t	Index;
	bool		Direction;
	bool		BoundaryBehavior;
	CRGB 		Color1;
	CRGB 		Color2;
} FX_COLOR_WIPE_VARS_T;

//void LightFX_ProcColorWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * indexPos, bool * direction, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_InitFXColorWipe(LIGHT_FX_T * fx, FX_COLOR_WIPE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_SetFXColorWipe(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, bool boundaryBehavior, CRGB color1, CRGB color2);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	bool Direction;
	CRGB Color1;
	CRGB Color2;
} FX_BLINK_VARS_T;

//void LightFX_ProcBlink(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool * direction, CRGB color1, CRGB color2);
void LightFX_InitFXBlink(LIGHT_FX_T * fx, FX_BLINK_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool startingDirection, CRGB color1, CRGB color2);
void LightFX_SetFXBlink(LIGHT_FX_T * fx, bool startingDirection, CRGB color1, CRGB color2);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	uint32_t		Index;
	bool			Direction;
	uint32_t		Steps;
	bool			BoundaryBehavior;
	const CRGBPalette16 * p_Palette;
	uint32_t		VisibleSpectrum;
} FX_PALETTE_WIPE_VARS_T;

void LightFX_InitFXPaletteWipe(LIGHT_FX_T * fx, FX_PALETTE_WIPE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum);
void LightFX_SetFXPaletteWipe(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette, uint8_t visibleSpectrum);
void LightFX_SetFXPaletteWipeSpectrum(LIGHT_FX_T * fx, uint8_t visibleSpectrum);
void LightFX_SetFXPaletteWipeCyleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	uint32_t	Index;
	bool		Direction;
	uint32_t	Steps;
	bool		BoundaryBehavior;
	const CRGBPalette16 * p_Palette;
} FX_PALETTE_FADER_VARS_T;

void LightFX_InitFXPaletteFader(LIGHT_FX_T * fx, FX_PALETTE_FADER_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette);
void LightFX_SetFXPaletteFader(LIGHT_FX_T * fx, uint32_t startingIndex, bool startingDirection, uint16_t stepsPerCycle, bool boundaryBehavior, const CRGBPalette16 * palette);
void LightFX_SetFXPaletteFaderCyleTime(LIGHT_FX_T * fx, uint32_t cycleTimeMs, uint16_t ticksPerSecond);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	bool	Direction;
	uint8_t Cooling;
	uint8_t Sparking;
	uint8_t * p_Heat;
} FX_FIRE2012_VARS_T;

void LightFX_InitFXFire2012(LIGHT_FX_T * fx, FX_FIRE2012_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool direction, uint8_t cooling, uint8_t sparkling, uint8_t * ledsBuffer);
void LightFX_InitFXFire2012Default(LIGHT_FX_T * fx, FX_FIRE2012_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint8_t * ledsBuffer);
void LightFX_SetFXFire2012(LIGHT_FX_T * fx, bool direction, uint8_t cooling, uint8_t sparkling);
void LightFX_SetFXFire2012Restart(LIGHT_FX_T * fx);

/******************************************************************************/
/*!
 * @name
 * @brief
 */
/******************************************************************************/
typedef struct
{
	bool	Direction;
	uint8_t Cooling;
	uint8_t Sparking;
	uint8_t * p_Heat;
	const CRGBPalette16 * p_Palette;
	CRGBPalette16 DynamicPalette;
	bool UseDynamicPalette;
	//uint8_t DynamicPaletteHue;
} FX_FIRE2012_WITH_PALETTE_VARS_T;

void LightFX_InitFXFire2012WithPalette(LIGHT_FX_T * fx, FX_FIRE2012_WITH_PALETTE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool direction, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t cooling, uint8_t sparkling, uint8_t * ledsBuffer);
void LightFX_InitFXFire2012WithPaletteDefault(LIGHT_FX_T * fx, FX_FIRE2012_WITH_PALETTE_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t * ledsBuffer);
void LightFX_SetFXFire2012WithPalette(LIGHT_FX_T * fx, bool direction, const CRGBPalette16 * initialPalette, bool useDynamicPalette, uint8_t cooling, uint8_t sparkling);
void LightFX_SetFXFire2012WithPaletteRestart(LIGHT_FX_T * fx);

typedef struct
{
	float		Position;
	bool		Direction;
	uint16_t	Steps;
	bool		BoundaryBehavior;
	CRGB 		Color1;
	CRGB 		Color2;
	uint16_t	TipLength;
} FX_BLADE_SCROLL_VARS_T;

void LightFX_InitBladeScroll(LIGHT_FX_T * fx, FX_BLADE_SCROLL_VARS_T * vars, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, float pos, bool direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2, uint16_t tipLength);
void LightFX_SetBladeScroll(LIGHT_FX_T * fx, float pos, bool direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2, uint16_t tipLength);
void LightFX_SetBladeScrollTipLength(LIGHT_FX_T * fx, uint16_t len);
void LightFX_SetBladeScrollTime(LIGHT_FX_T * fx, uint32_t scrollTimeMs, uint16_t tickPerSecond);


#endif /* SOURCE_LITEFXOS_LIGHTFX_H_ */
