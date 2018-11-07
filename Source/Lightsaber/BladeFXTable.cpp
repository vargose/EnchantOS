#include "LiteFXOS.h"

/*-----------------------------------------------------------------------------
  Blade LiteFX
 *----------------------------------------------------------------------------*/
LITE_FX_T BladeFXPaletteSolid;
LITE_FX_T BladeFXPaletteWipe;
LITE_FX_T BladeFXPaletteFader;

LITE_FX_T BladeFXScannerWithPalette;

LITE_FX_T BladeFXFire;
LITE_FX_T BladeFXFireWithPalette;
LITE_FX_T BladeFXFireWithDynamicPalette;

//LITE_FX_T BladeFXStrobe;
//LITE_FX_T BladeFXCylon;
//LITE_FX_T BladeFXSineWipe;
//
//LITE_FX_T BladeFXRainbowWithGlitter;
//LITE_FX_T BladeFXConfetti;
//LITE_FX_T BladeFXBPM;
//LITE_FX_T BladeFXJuggle;


//LITE_FX_T BladeFXPaletteSineWipe;
//LITE_FX_T BladeFXPaletteStrobe;
//
//LITE_FX_T BladeFXTheaterChase;
//
//LITE_FX_T BladeFXFlash;
//LITE_FX_T BladeFXFlicker;

/*-----------------------------------------------------------------------------
  Palette Table for Fire2012
 *----------------------------------------------------------------------------*/
const CRGBPalette16 FIRE_PALETTES[] =
{
	//HeatColors_p,
	CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White),
	CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White),
	CRGBPalette16( CRGB::Black, CRGB::White),
};

PALETTE_TABLE_T BladeFXFirePaletteTable =
{
	.p_Palettes = &FIRE_PALETTES[0],
	.TableSize = sizeof(FIRE_PALETTES)/sizeof(CRGBPalette16),
	.TableIndex = 0,
	.ColorIndex = 0,
};

/*-----------------------------------------------------------------------------
  Blade LiteFX Table
 *----------------------------------------------------------------------------*/
LITE_FX_T * const BLADE_FX_ARRAY[] =
{
	&BladeFXPaletteWipe,
	&BladeFXPaletteFader,

	&BladeFXScannerWithPalette,

	&BladeFXPaletteSolid,

	&BladeFXFire,
	&BladeFXFireWithPalette,
	&BladeFXFireWithDynamicPalette,

//	&BladeFXStrobe,
//	&BladeFXCylon,
//	&BladeFXSineWipe,

//	&BladeFXRainbowWithGlitter,
//	&BladeFXConfetti,
//	&BladeFXBPM,
//	&BladeFXJuggle,

//	&LITE_FX_SCANNER,
//	&LITE_FX_SCANNER_INTENSITY,
//	&LITE_FX_THEATER_CHASE,
};

LITE_FX_TABLE_T BladeFXTable =
{
	.p_LiteFXArray 	= (LITE_FX_T **)&BLADE_FX_ARRAY,
	.TableSize 		= sizeof(BLADE_FX_ARRAY)/sizeof(LITE_FX_T*),
	.TableIndex 	= 0,
};

/*-----------------------------------------------------------------------------
  Init
 *----------------------------------------------------------------------------*/
void BladeFXTableInit(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB * ledStripBuffer)
{
	//Used by table
	LiteFX_InitPaletteWipe(&BladeFXPaletteWipe, ledStrip, ledStart, ledLength, 0, true, 120, false, PaletteTable_GetPalette(&PaletteTable_Default), 255*2/3, 0, 60, 0);
	LiteFX_AttachInterface(&BladeFXPaletteWipe, &LIGHT_FX_INTERFACE_PALETTE);
	LiteFX_AttachTickRate(&BladeFXPaletteWipe, &LIGHT_FX_TICK_RATE_60);
	LiteFX_AttachPaletteTable(&BladeFXPaletteWipe, &PaletteTable_Default);

	LiteFX_InitPaletteFader(&BladeFXPaletteFader, ledStrip, ledStart, ledLength, 0, true, 120, false, PaletteTable_GetPalette(&PaletteTable_Default), 0, 60, 0);
	LiteFX_AttachInterface(&BladeFXPaletteFader, &LIGHT_FX_INTERFACE_SPEED);
	LiteFX_AttachTickRate(&BladeFXPaletteFader, &LIGHT_FX_TICK_RATE_60);
	LiteFX_AttachPaletteTable(&BladeFXPaletteFader, &PaletteTable_Default);

	LiteFX_InitPaletteSolid(&BladeFXPaletteSolid, ledStrip, ledStart, ledLength, PaletteTable_GetPalette(&PaletteTable_Default));
	LiteFX_AttachInterface(&BladeFXPaletteSolid, &LIGHT_FX_INTERFACE_PALETTE_COLOR);

	LiteFX_InitFire2012(&BladeFXFire, ledStrip, ledStart, ledLength, (uint8_t *)ledStripBuffer, 0, 60,  0);
	LiteFX_SetFire2012(&BladeFXFire, 1, 65, 120, 0, 60, 0); //65 is cooling
	LiteFX_AttachInterface(&BladeFXFire, &LIGHT_FX_INTERFACE_FIRE_RESET);

	LiteFX_InitFire2012WithPalette(&BladeFXFireWithPalette, ledStrip, ledStart, ledLength, PaletteTable_GetPalette(&BladeFXFirePaletteTable), false, (uint8_t *)ledStripBuffer, 0, 60, 0);
	LiteFX_AttachPaletteTable(&BladeFXFireWithPalette, &BladeFXFirePaletteTable);
	LiteFX_AttachInterface(&BladeFXFireWithPalette, &LIGHT_FX_INTERFACE_PALETTE);

	LiteFX_InitFire2012WithPalette(&BladeFXFireWithDynamicPalette, ledStrip, ledStart, ledLength, 0, true, (uint8_t *)ledStripBuffer, 0, 60, 0);
	LiteFX_AttachInterface(&BladeFXFireWithDynamicPalette, &LIGHT_FX_INTERFACE_FIRE_RESET);

	LiteFX_InitScannerWithPalette(&BladeFXScannerWithPalette, ledStrip, ledStart, ledLength, 0, 1, true, PaletteTable_GetPalette(&PaletteTable_Default), 48, 0, 60, 0);


}

//static uint8_t SaberColorsIndex;
//static CRGB SaberColors[] = {CRGB::Blue, 0x00FF00, CRGB::Red, CRGB::Purple, CRGB::DeepPink, CRGB::Orange, CRGB::Gold};
//
//static void SaberColorsNext()
//{
//	if (SaberColorsIndex < sizeof(SaberColors)/sizeof(CRGB) - 1)
//		SaberColorsIndex++;
//	else
//		SaberColorsIndex = 0;
//
//	fill_solid(LEDStripColor, NumLEDs, SaberColors[SaberColorsIndex]); //do outside?
//
//}
//
//static void SaberColorsPrevious()
//{
//	if (SaberColorsIndex > 0)
//		SaberColorsIndex--;
//	else
//		SaberColorsIndex = sizeof(SaberColors)/sizeof(CRGB) - 1;
//
//	fill_solid(LEDStripColor, NumLEDs, SaberColors[SaberColorsIndex]); //do outside?
//}

//LITE_FX_INTERFACE_T LIGHT_FX_INTERFACE_SABER_COLORS =
//{
//	.Up = SaberColorsNext,
//	.Down = SaberColorsPrevious,
//};
