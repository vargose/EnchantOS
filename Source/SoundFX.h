/**************************************************************************/
/*!
    @file     SoundFX.h
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
#ifndef SOUND_FX_H
#define SOUND_FX_H

typedef struct
{
	char ** Filenames;
	uint8_t FilenameLenMax;
	uint8_t FileCountMax;
	uint8_t WavCount = 0;
	uint8_t	WavIndex = 0;
}
SOUND_FX_T;

// A SoundFX Font is a group of SoundFX
// All files for a font will be under the same directory
//typedef struct
//{
//  char * DirName;
//	SOUND_FX_T * SoundFX;
//	uint8_t SoundFXCount = 0;
//  bool Style;
//}
//SOUND_FX_FONT_T;

void SoundFX_Init(uint8_t pin);

void SoundFX_InitFX(SOUND_FX_T * soundFX, char ** filenamesBuffer, uint8_t filenameLenMax, uint8_t fileCountMax);
void SoundFX_AddFile(SOUND_FX_T * soundFX, const char * fileName);
void SoundFX_AddDirectory(SOUND_FX_T * soundFX, const char * dirName);
void SoundFX_InitFXWithDirectory(SOUND_FX_T * soundFX, char ** filenamesBuffer, uint8_t filenameLenMax, uint8_t fileCountMax, const char * dirName);

void SoundFX_Restart(void);
void SoundFX_VolumeUp(void);
void SoundFX_VolumeDown(void);
void SoundFX_Pause(void);

bool SoundFX_IsPlaying(void);
void SoundFX_Stop(void);
void SoundFX_PollAmpDisable(void);

void SoundFX_PlayFile(const char * filename);
void SoundFX_PlayFileLayered(const char * filename, bool backgroundForeground);
void SoundFX_PlayFileBackgroudLoop(const char * filename);

void SoundFX_PlayFX(SOUND_FX_T * soundFX);

void SoundFX_PlayFXIndex(SOUND_FX_T * soundFX, uint8_t index);
void SoundFX_PlayFXRandom(SOUND_FX_T * soundFX);

void SoundFX_PlayFXLayered(SOUND_FX_T * soundFX, bool backgroundForeground);
void SoundFX_PlayFXLayeredBackgroudLoop(SOUND_FX_T * soundFX);

void SoundFX_PlayNext(SOUND_FX_T * soundFXDir);
void SoundFX_PlayPrevious(SOUND_FX_T * soundFXDir);

uint8_t SoundFX_GetRMS();
uint8_t SoundFX_GetPeak();

void debug_SoundFX_PrintPeak();
void debug_SoundFX_PrintFilenames(SOUND_FX_T * soundFXDir);
void debug_SoundFX_PrintFilenames(char ** filenameBuffer, uint8_t fileCount, uint8_t filenameLen);

#endif
