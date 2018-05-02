/**************************************************************************/
/*!
    @file     PaletteTable.cpp
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

    @brief Palette selection table.
*/
/**************************************************************************/
#include "FastLED.h"

#include "PaletteTable.h"

/*-----------------------------------------------------------------------------
 Palette Table Functions
 *----------------------------------------------------------------------------*/
const CRGBPalette16 * PaletteTable_GetPalette(PALETTE_TABLE_T * paletteTable)
{
	if(!paletteTable->p_Palettes) return 0;

	return &paletteTable->p_Palettes[paletteTable->TableIndex];
}

const CRGBPalette16 * PaletteTable_GetSelectPalette(PALETTE_TABLE_T * paletteTable, uint8_t index)
{
	if(!paletteTable->p_Palettes) return 0;

	if(index < paletteTable->TableSize)
		return &paletteTable->p_Palettes[index];
	else
		return &paletteTable->p_Palettes[paletteTable->TableIndex];
}

const CRGBPalette16 * PaletteTable_GetNextPalette(PALETTE_TABLE_T * paletteTable)
{
	if(!paletteTable->p_Palettes) return 0;

	if (paletteTable->TableIndex < paletteTable->TableSize - 1)
		paletteTable->TableIndex++;
	else
		paletteTable->TableIndex = 0;

	return &paletteTable->p_Palettes[paletteTable->TableIndex];
}

const CRGBPalette16 * PaletteTable_GetPreviousPalette(PALETTE_TABLE_T * paletteTable)
{
	if(!paletteTable->p_Palettes) return 0;

	if (paletteTable->TableIndex > 0)
		paletteTable->TableIndex--;
	else
		paletteTable->TableIndex = paletteTable->TableSize - 1;

	return &paletteTable->p_Palettes[paletteTable->TableIndex];
}

CRGB PaletteTable_GetColor(PALETTE_TABLE_T * paletteTable)
{
	if(!paletteTable->p_Palettes) return 0;

	return ColorFromPalette(paletteTable->p_Palettes[paletteTable->TableIndex], paletteTable->ColorIndex);
}

CRGB PaletteTable_GetNextColor(PALETTE_TABLE_T * paletteTable)
{
	if(!paletteTable->p_Palettes) return 0;

	paletteTable->ColorIndex += 256/16;
	return ColorFromPalette(paletteTable->p_Palettes[paletteTable->TableIndex], paletteTable->ColorIndex);
}

CRGB PaletteTable_GetPreviousColor(PALETTE_TABLE_T * paletteTable)
{
	if(!paletteTable->p_Palettes) return 0;

	paletteTable->ColorIndex -= 256/16;
	return ColorFromPalette(paletteTable->p_Palettes[paletteTable->TableIndex], paletteTable->ColorIndex);
}

CRGB PaletteTable_GetSelectColor(PALETTE_TABLE_T * paletteTable, uint8_t paletteColorIndex)
{
	if(!paletteTable->p_Palettes) return 0;

	return ColorFromPalette(paletteTable->p_Palettes[paletteTable->TableIndex], paletteColorIndex);
}

//void PaletteTable_GetPaletteArg(PALETTE_TABLE_T * paletteTable, const CRGBPalette16 * palette)
//{
//	palette = &paletteTable->p_Palettes[paletteTable->TableIndex];
//}

void PaletteTable_Reset(PALETTE_TABLE_T * paletteTable)
{
	paletteTable->TableIndex = 0;
}

void PaletteTable_SetNext(PALETTE_TABLE_T * paletteTable)
{
	if (paletteTable->TableIndex < paletteTable->TableSize - 1)
		paletteTable->TableIndex++;
	else
		paletteTable->TableIndex = 0;
}

void PaletteTable_SetPrevious(PALETTE_TABLE_T * paletteTable)
{
	if (paletteTable->TableIndex > 0)
		paletteTable->TableIndex--;
	else
		paletteTable->TableIndex = paletteTable->TableSize - 1;
}

void PaletteTable_InitPaletteTable(PALETTE_TABLE_T * paletteTable, CRGBPalette16 * paletteArray, uint16_t paletteCount)
{
	paletteTable->TableIndex = 0;
	paletteTable->TableSize = paletteCount;
	paletteTable->p_Palettes = paletteArray;
}

/*-----------------------------------------------------------------------------
  Default Palette Table
 *----------------------------------------------------------------------------*/
const CRGBPalette16 PALETTES_DEFAULT[] =
{
	RainbowColors_p,
	RainbowStripeColors_p,
	PartyColors_p,
	HeatColors_p,
	CloudColors_p,
	LavaColors_p,
	OceanColors_p,
	ForestColors_p,
};

PALETTE_TABLE_T PaletteTable_Default =
{
	.p_Palettes = &PALETTES_DEFAULT[0],
	.TableSize = sizeof(PALETTES_DEFAULT)/sizeof(CRGBPalette16),
	.TableIndex = 0,
	.ColorIndex = 0,
};
