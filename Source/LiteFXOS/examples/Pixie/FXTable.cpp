#include "LiteFXOS.h"

/*-----------------------------------------------------------------------------
  LiteFX
 *----------------------------------------------------------------------------*/
LITE_FX_T FXPaletteSolid;
LITE_FX_T FXPaletteWipe;
LITE_FX_T FXPaletteFader;

LITE_FX_T FXFire;
LITE_FX_T FXFireWithPalette;
LITE_FX_T FXFireWithDynamicPalette;

LITE_FX_T FXStrobe;
LITE_FX_T FXCylon;
LITE_FX_T FXSineWipe;

LITE_FX_T FXRainbowWithGlitter;
LITE_FX_T FXConfetti;
LITE_FX_T FXBPM;
LITE_FX_T FXJuggle;

//LITE_FX_T FXPaletteCylon;
//LITE_FX_T FXPaletteSineWipe;
//LITE_FX_T FXPaletteStrobe;
//
//LITE_FX_T FXTheaterChase;
//
//LITE_FX_T FXFlash;
//LITE_FX_T FXFlicker;

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

PALETTE_TABLE_T FXFirePaletteTable =
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
	&FXPaletteWipe,
	&FXPaletteFader,
	&FXPaletteSolid,

	&FXFire,
	&FXFireWithPalette,
	&FXFireWithDynamicPalette,

//	&FXStrobe,
//	&FXCylon,
//	&FXSineWipe,

//	&FXRainbowWithGlitter,
//	&FXConfetti,
//	&FXBPM,
//	&FXJuggle,

//	&LITE_FX_SCANNER,
//	&LITE_FX_SCANNER_INTENSITY,
//	&LITE_FX_THEATER_CHASE,
};

LITE_FX_TABLE_T FXTable =
{
	.p_LiteFXArray 	= (LITE_FX_T **)&BLADE_FX_ARRAY,
	.TableSize 		= sizeof(BLADE_FX_ARRAY)/sizeof(LITE_FX_T*),
	.TableIndex 	= 0,
};

//LITE_FX_TABLE_T LightFXTableVisualizer;

/*-----------------------------------------------------------------------------
  Init
 *----------------------------------------------------------------------------*/
void FXTableInit(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, CRGB * ledStripBuffer)
{
	//Used by table
	LiteFX_InitPaletteWipe(&FXPaletteWipe, ledStrip, ledStart, ledLength, 0, true, 120, false, PaletteTable_GetPalette(&PaletteTable_Default), 255*2/3, 0, 60, 0);
	LiteFX_AttachInterface(&FXPaletteWipe, &LIGHT_FX_INTERFACE_PALETTE);
	LiteFX_AttachTickRate(&FXPaletteWipe, &LIGHT_FX_TICK_RATE_60);
	LiteFX_AttachPaletteTable(&FXPaletteWipe, &PaletteTable_Default);

	LiteFX_InitPaletteFader(&FXPaletteFader, ledStrip, ledStart, ledLength, 0, true, 120, false, PaletteTable_GetPalette(&PaletteTable_Default), 0, 60, 0);
	LiteFX_AttachInterface(&FXPaletteFader, &LIGHT_FX_INTERFACE_SPEED);
	LiteFX_AttachTickRate(&FXPaletteFader, &LIGHT_FX_TICK_RATE_60);
	LiteFX_AttachPaletteTable(&FXPaletteFader, &PaletteTable_Default);

	LiteFX_InitPaletteSolid(&FXPaletteSolid, ledStrip, ledStart, ledLength, PaletteTable_GetPalette(&PaletteTable_Default));
	LiteFX_AttachInterface(&FXPaletteSolid, &LIGHT_FX_INTERFACE_PALETTE_COLOR);

	LiteFX_InitFire2012(&FXFire, ledStrip, ledStart, ledLength, (uint8_t *)ledStripBuffer, 0, 60,  0);
	LiteFX_AttachInterface(&FXFire, &LIGHT_FX_INTERFACE_FIRE_RESET);

	LiteFX_InitFire2012WithPalette(&FXFireWithPalette, ledStrip, ledStart, ledLength, PaletteTable_GetPalette(&FXFirePaletteTable), false, (uint8_t *)ledStripBuffer, 0, 60, 0);
	LiteFX_AttachPaletteTable(&FXFireWithPalette, &FXFirePaletteTable);
	LiteFX_AttachInterface(&FXFireWithPalette, &LIGHT_FX_INTERFACE_PALETTE);

	LiteFX_InitFire2012WithPalette(&FXFireWithDynamicPalette, ledStrip, ledStart, ledLength, 0, true, (uint8_t *)ledStripBuffer, 0, 60, 0);
	LiteFX_AttachInterface(&FXFireWithDynamicPalette, &LIGHT_FX_INTERFACE_FIRE_RESET);
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
