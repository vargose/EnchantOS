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
	void (*Pattern)(void);

	CRGB * p_LEDStrip;
	uint16_t NumLEDs;

	uint32_t	IndexPos;
	bool		Direction;
	uint32_t	Steps;
	bool		BoundaryBehavior;

	CRGB 	Color1;
	CRGB 	Color2;

	CRGBPalette16 *	p_Palette;

	uint32_t	Special1;
	uint32_t	Special2;
	uint32_t	Special3;

	CRGB * p_LEDStripBuffer;
} LIGHT_FX_T;

void LightFX_SetOff(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength);

void LightFX_ProcColorFader(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * index, bool * direction, uint16_t steps, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_ProcColorWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t * indexPos, bool * direction, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_ProcBlink(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool * direction, CRGB color1, CRGB color2);
void LightFX_ProcPaletteWipe(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum);

void LightFX_LoadMemory(void * fx);
void LightFX_MapFX(LIGHT_FX_T * fx);
void LightFX_ProcFX(LIGHT_FX_T * fx);


void LightFX_SetFXColorFader(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t patternIndex, bool direction, uint16_t transitionSteps, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_SetFXColorWipe(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t patternIndex, bool direction, bool boundaryBehavior, CRGB color1, CRGB color2);
void LightFX_SetFXBlink(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, bool direction, CRGB color1, CRGB color2);
void LightFX_SetFXPaletteWipe(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint16_t cycleSteps, CRGBPalette16 * palette, uint8_t visibleSpectrum);

void LightFX_SetFXColorFaderStrip(LIGHT_FX_T * fx, CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, uint32_t patternIndex, bool direction, uint16_t boundarySteps, uint16_t totalTransitionSteps, bool boundaryBehavior, CRGB * ledStripInitialColor, CRGB color);

#endif /* SOURCE_LITEFXOS_LIGHTFX_H_ */
