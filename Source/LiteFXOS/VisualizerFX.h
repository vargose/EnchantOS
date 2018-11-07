/*-----------------------------------------------------------------------------
  LightFX with Visualizer Input
 *----------------------------------------------------------------------------*/
void VisualizerFX_Update(float v);
void VisualizerFX_CalcVars();

void VisualizerFX_SetThresholdMic();
void VisualizerFX_SetThresholdWav();

void VisualizerFX_ResetVars();
void VisualizerFX_SetPalette(const CRGBPalette16 * p_palette);

void VisualizerFX_Init(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * startingPalette, CRGB * colorBuffer, uint8_t * valBuffer, const CRGBPalette16 ** paletteSync);

void (*VisualizerFXTableGet(void))(void);
void (*VisualizerFXTableGetNext(void))(void);
void (*VisualizerFXTableGetPrev(void))(void);
