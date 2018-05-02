/**************************************************************************/
/*!
    @file     SoundFX.cpp
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

    @brief For binding groups of sound files. Uses the Teensy Audio Library
*/
/**************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "Audio.h"
#include "SD.h"

#include "SoundFX.h"

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
typedef enum
{
		WAV1_INPUT_L = 0,
		WAV1_INPUT_R = 1,
		WAV2_INPUT_L = 2,
		WAV2_INPUT_R = 3,
} SOUND_FX_OUT_CHANNEL_T;

//AudioSynthWaveform	Waveform1;

AudioPlaySdWav		PlayWav1;
AudioPlaySdWav		PlayWav2;

AudioMixer4			SoundFXOut;
AudioConnection		patchCord1(PlayWav1, 0, SoundFXOut, WAV1_INPUT_L);
AudioConnection		patchCord2(PlayWav1, 1, SoundFXOut, WAV1_INPUT_R);
AudioConnection		patchCord3(PlayWav2, 0, SoundFXOut, WAV2_INPUT_L);
AudioConnection		patchCord4(PlayWav2, 1, SoundFXOut, WAV2_INPUT_R);

#ifdef __MK64FX512__
	AudioOutputAnalogStereo		DAC;
	AudioConnection				patchCord5(SoundFXOut, 0, DAC, 1);
#endif
#ifdef __MK20DX256__
	AudioOutputAnalog			DAC;
	AudioConnection				patchCord5(SoundFXOut, 0, DAC, 0);
#endif

AudioAnalyzePeak	peak_L;
AudioAnalyzePeak    peak_R;
AudioConnection		c1(PlayWav1,0,peak_L,0);
AudioConnection		c2(PlayWav1,1,peak_R,0);

AudioAnalyzeRMS		RMS_L;
AudioAnalyzeRMS		RMS_R;
AudioConnection		c3(PlayWav2,0,RMS_L,0);
AudioConnection		c4(PlayWav2,1,RMS_R,0);

static uint8_t Peak;
static uint8_t RMS;

static uint8_t AmpPin;

bool Wav2IsPlaying = 0;
bool Wav2Changed = 0;

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
static uint32_t Volume = 100;

static inline void AmpOn(){digitalWrite(AmpPin, HIGH);}
static inline void AmpOff(){digitalWrite(AmpPin, LOW);}

void SoundFX_Init(uint8_t pin)
{
	AmpPin = pin;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH); // turn on the amplifier

	delay(10);             // allow time to wake up

	AudioMemory(30);

	DAC.analogReference(EXTERNAL); // much louder!

	delay(50);             // time for DAC voltage stable

	SoundFXOut.gain(WAV1_INPUT_L, 0.5);
	SoundFXOut.gain(WAV1_INPUT_R, 0.5);
	SoundFXOut.gain(WAV2_INPUT_L, 0);
	SoundFXOut.gain(WAV2_INPUT_R, 0);
}

// A SoundFX is a group of sound files binded to one action/event
// Playing the SoundFX will cycle through the files when the action/event occurs
// Sound files from different directories may be added to the same FX group
// Must provide module with a buffer for file names
// Sets buffer and boundaries to check against
void SoundFX_InitFX(SOUND_FX_T * soundFX, char ** filenamesBuffer, uint8_t filenameLenMax, uint8_t fileCountMax)
{
	soundFX->Filenames = filenamesBuffer;
	soundFX->FilenameLenMax = filenameLenMax;
	soundFX->FileCountMax = fileCountMax;
	soundFX->WavCount = 0;

	memset(filenamesBuffer, 0, filenameLenMax*fileCountMax); //need?
}

void SoundFX_AddFile(SOUND_FX_T * soundFX, const char * fileName)
{
	if(!SD.exists(fileName)) return;

	if (strlen(fileName) + 1 > soundFX->FilenameLenMax) return;

	if (soundFX->WavCount < soundFX->FileCountMax)
	{
		snprintf(((char*)soundFX->Filenames + soundFX->WavCount * soundFX->FilenameLenMax), soundFX->FilenameLenMax, "%s", fileName);
		soundFX->WavCount++;
	}
}

void SoundFX_AddFilesWithPrefix(SOUND_FX_T * soundFX, const char * dirName, const char * filePrefix)
{

}

static uint8_t FindWavFilesInDirectory(char ** filenamesBuffer, uint8_t filenameLenMax, uint8_t fileCountMax, const char * dirName)
{
	File Dir;
	File Fil;

    uint8_t count = 0;

    if(SD.exists(dirName))
    	Dir = SD.open(dirName);
	else
	{
		//SD.mkdir(dirName);
		return 0;
	}

    while(1)
    {
        Fil = Dir.openNextFile();

        if(!Fil) break;

        if((count < fileCountMax) && ((strstr(Fil.name(), ".wav")) || (strstr(Fil.name(), ".WAV"))) && (strlen(Fil.name()) + 1 < filenameLenMax))
        {
            if((Fil.isDirectory()) == 0)
            {
            	//snprintf(*(filenamesBuffer + count * filenameLenMax), filenameLenMax, "%s%s%s", dirName, "/", Fil.name());
            	snprintf(((char*)filenamesBuffer + count * filenameLenMax), filenameLenMax, "%s%s%s", dirName, "/", Fil.name());
            	count++;
            }
        }
    }

    return(count);
}

void SoundFX_AddDirectory(SOUND_FX_T * soundFX, const char * dirName)
{
	soundFX->WavCount += FindWavFilesInDirectory((char**)((char*)soundFX->Filenames + soundFX->WavCount * soundFX->FilenameLenMax), soundFX->FilenameLenMax, soundFX->FileCountMax - soundFX->WavCount, dirName);
}

void SoundFX_InitFXWithDirectory(SOUND_FX_T * soundFX, char ** filenamesBuffer, uint8_t filenameLenMax, uint8_t fileCountMax, const char * dirName)
{
	SoundFX_InitFX(soundFX, filenamesBuffer, filenameLenMax, fileCountMax);
	SoundFX_AddDirectory(soundFX, dirName);
}

void SoundFX_Reload(void)
{

}




void SoundFX_Reset(void)
{
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT1_L, 0.5);
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT1_R, 0.5);
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT2_L, 0);
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT2_R, 0);
}

void SoundFX_VolumeUp(void)
{

}

void SoundFX_VolumeDown(void)
{

}

void SoundFX_SetVolume(float volume)
{
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT1_L, volume);
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT1_R, volume);
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT2_L, volume);
//	SoundFXOut.gain(SOUND_FX_OUT_INPUT2_R, volume);
}


void SoundFX_Pause(void)
{

}

bool SoundFX_IsPlaying(void)
{
	if (PlayWav1.isPlaying() || PlayWav2.isPlaying())
		return true;
	else
		return false;
}

void SoundFX_Stop(void)
{
	PlayWav1.stop();
	PlayWav2.stop();
}

void SoundFX_PollAmpDisable(void)
{
	if (!PlayWav1.isPlaying() && !PlayWav2.isPlaying())
		AmpOff();
}

void SoundFX_PlayFile(const char * filename)
{
	if (filename == 0) return;

	//Serial.println(filename);

	SoundFXOut.gain(WAV1_INPUT_L, 0.5);
	SoundFXOut.gain(WAV1_INPUT_R, 0.5);
	SoundFXOut.gain(WAV2_INPUT_L, 0);
	SoundFXOut.gain(WAV2_INPUT_R, 0);
	PlayWav1.play(filename);
}

void SoundFX_PlayFileLayered(const char * filename, bool backgroundForeground)
{
	if (filename == 0) return;

	if(PlayWav1.isPlaying() || PlayWav2.isPlaying())
	{
		SoundFXOut.gain(WAV1_INPUT_L, 0.1);
		SoundFXOut.gain(WAV1_INPUT_R, 0.1);
		SoundFXOut.gain(WAV2_INPUT_L, 0.4);
		SoundFXOut.gain(WAV2_INPUT_R, 0.4);

		if(backgroundForeground)
			PlayWav2.play(filename);
		else
			PlayWav1.play(filename);
	}
	else
	{
		if(backgroundForeground)
		{
			SoundFXOut.gain(WAV1_INPUT_L, 0);
			SoundFXOut.gain(WAV1_INPUT_R, 0);
			SoundFXOut.gain(WAV2_INPUT_L, 0.5);
			SoundFXOut.gain(WAV2_INPUT_R, 0.5);
			PlayWav2.play(filename);
		}
		else
		{
			SoundFXOut.gain(WAV1_INPUT_L, 0.5);
			SoundFXOut.gain(WAV1_INPUT_R, 0.5);
			SoundFXOut.gain(WAV2_INPUT_L, 0);
			SoundFXOut.gain(WAV2_INPUT_R, 0);
			PlayWav1.play(filename);
		}
	}
}

void SoundFX_PlayFX(SOUND_FX_T * soundFX)
{
	SoundFX_PlayFile(((char*)soundFX->Filenames + soundFX->WavIndex * soundFX->FilenameLenMax));

	if(soundFX->WavIndex < soundFX->WavCount - 1)
		soundFX->WavIndex++;
	else
		soundFX->WavIndex = 0;
}

void SoundFX_PlayFXIndex(SOUND_FX_T * soundFX, uint8_t index)
{
	if (index > soundFX->WavCount - 1)
		index = 0;

	SoundFX_PlayFile(((char*)soundFX->Filenames + index * soundFX->FilenameLenMax));
}

void SoundFX_PlayFXRandom(SOUND_FX_T * soundFX)
{
	uint8_t index =	index = rand() % soundFX->WavCount;

	SoundFX_PlayFile(((char*)soundFX->Filenames + index * soundFX->FilenameLenMax));
}

// 0 for background
void SoundFX_PlayFXLayered(SOUND_FX_T * soundFX, bool backgroundOrForeground)
{
	SoundFX_PlayFileLayered(((char*)soundFX->Filenames + soundFX->WavIndex * soundFX->FilenameLenMax), backgroundOrForeground);

	if(soundFX->WavIndex < soundFX->WavCount - 1)
		soundFX->WavIndex++;
	else
		soundFX->WavIndex = 0;
}

void SoundFX_PlayFileLayeredBackgroudLoop(const char * filename)
{
	if (!PlayWav1.isPlaying())
		SoundFX_PlayFileLayered(filename, 0);

	Wav2Changed = Wav2IsPlaying ^ PlayWav2.isPlaying();
	Wav2IsPlaying = PlayWav2.isPlaying();

	if(!Wav2IsPlaying && Wav2Changed)
	{
		SoundFXOut.gain(WAV1_INPUT_L, 0.5);
		SoundFXOut.gain(WAV1_INPUT_R, 0.5);
		SoundFXOut.gain(WAV2_INPUT_L, 0);
		SoundFXOut.gain(WAV2_INPUT_R, 0);
	}
}

void SoundFX_PlayFXLayeredBackgroudLoop(SOUND_FX_T * soundFX)
{
	if (!PlayWav1.isPlaying())
		SoundFX_PlayFXLayered(soundFX, 0);

	Wav2Changed = Wav2IsPlaying ^ PlayWav2.isPlaying();
	Wav2IsPlaying = PlayWav2.isPlaying();

	if(!Wav2IsPlaying && Wav2Changed)
	{
		SoundFXOut.gain(WAV1_INPUT_L, 0.5);
		SoundFXOut.gain(WAV1_INPUT_R, 0.5);
		SoundFXOut.gain(WAV2_INPUT_L, 0);
		SoundFXOut.gain(WAV2_INPUT_R, 0);
	}
}

// good for music selection
void SoundFX_PlayNext(SOUND_FX_T * soundFX)
{
	if(soundFX->WavIndex < soundFX->WavCount - 1)
		soundFX->WavIndex++;
	else
		soundFX->WavIndex = 0;

	SoundFX_PlayFile(((char*)soundFX->Filenames + soundFX->WavIndex * soundFX->FilenameLenMax));
}

void SoundFX_PlayPrevious(SOUND_FX_T * soundFX)
{
	if(soundFX->WavIndex > 0)
		soundFX->WavIndex--;
	else
		soundFX->WavIndex = soundFX->WavCount - 1;

	SoundFX_PlayFile(((char*)soundFX->Filenames + soundFX->WavIndex * soundFX->FilenameLenMax));
}

void SoundFX_QueueFile(const char * filename)
{

}

uint8_t SoundFX_GetPeak()
{
	if (PlayWav1.isPlaying())

	if (peak_L.available() && peak_R.available())
		Peak = (peak_L.read() + peak_R.read())/2;

	return Peak;
}

uint8_t SoundFX_GetRMS()
{
	if (PlayWav1.isPlaying())
	{
		if (RMS_L.available() && RMS_R.available())
			RMS = (RMS_L.read() + RMS_R.read())/2;

		return RMS;
	}
	else return 0;
}

void debug_SoundFX_PrintPeak()
{
	static elapsedMillis fps;
	static uint8_t cnt=0;

	  if(fps > 24) {
	    if (peak_L.available() && peak_R.available()) {
	      fps=0;
	      uint8_t leftPeak=peak_L.read() * 30.0;
	      uint8_t rightPeak=peak_R.read() * 30.0;

	      for(cnt=0;cnt<30-leftPeak;cnt++) {
	        Serial.print(" ");
	      }
	      while(cnt++<30) {
	        Serial.print("<");
	      }
	      Serial.print("||");
	      for(cnt=0;cnt<rightPeak;cnt++) {
	        Serial.print(">");
	      }
	      while(cnt++<30) {
	        Serial.print(" ");
	      }
	      Serial.println();
	    }
	  }
}

void debug_SoundFX_PrintFXFilenames(SOUND_FX_T * soundFX)
{
    for (uint8_t i = 0; i < soundFX->WavCount; i++)
    {
 	  Serial.printf("%s", ((char*)soundFX->Filenames + i * soundFX->FilenameLenMax));
      Serial.println();
    }
}

void debug_SoundFX_PrintFilenames(char ** filenameBuffer, uint8_t fileCount, uint8_t filenameLen)
{
    for (uint8_t i = 0; i < fileCount; i++)
    {
      for (uint8_t j = 0; j < filenameLen; j++)
      {
    	  Serial.printf("%c", *((char*)filenameBuffer+(i*filenameLen) + j));
      }
      Serial.println();
    }
}
