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
*/
/**************************************************************************/
#ifndef SOURCE_LITEFXOS_PALETTETABLE_H_
#define SOURCE_LITEFXOS_PALETTETABLE_H_

typedef struct
{
	const CRGBPalette16 * p_Palettes;
	uint8_t TableSize;
	uint8_t TableIndex;
	uint8_t ColorIndex;
}
PALETTE_TABLE_T;

const CRGBPalette16 * PaletteTable_GetPalette(PALETTE_TABLE_T * paletteTable);
const CRGBPalette16 * PaletteTable_GetNextPalette(PALETTE_TABLE_T * paletteTable);
const CRGBPalette16 * PaletteTable_GetPreviousPalette(PALETTE_TABLE_T * paletteTable);

CRGB PaletteTable_GetColor(PALETTE_TABLE_T * paletteTable);
CRGB PaletteTable_GetNextColor(PALETTE_TABLE_T * paletteTable);
CRGB PaletteTable_GetPreviousColor(PALETTE_TABLE_T * paletteTable);
CRGB PaletteTable_GetSelectColor(PALETTE_TABLE_T * paletteTable, uint8_t paletteColorIndex);

//void PaletteTable_GetPaletteArg(PALETTE_TABLE_T * paletteTable, CRGBPalette16 * palette);
void PaletteTable_Reset(PALETTE_TABLE_T * paletteTable);

void PaletteTable_SetNext(PALETTE_TABLE_T * paletteTable);
void PaletteTable_SetPrevious(PALETTE_TABLE_T * paletteTable);

void PaletteTable_InitPaletteTable(PALETTE_TABLE_T * paletteTable, CRGBPalette16 * paletteTableArray, uint16_t paletteCount);

extern PALETTE_TABLE_T PaletteTable_Default;

#endif /* SOURCE_LITEFXOS_PALETTETABLE_H_ */
