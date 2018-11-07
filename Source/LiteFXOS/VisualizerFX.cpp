/**************************************************************************/
/*!
    @file     VisualizerFX.cpp
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

    @brief Sound visualizer LED patterns
    	based on the original code by Michael Bartlett found at:
    	https://github.com/bartlettmic/SparkFun-RGB-LED-Music-Sound-Visualizer-Arduino-Code
*/
/**************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "WS2812Serial.h"
#include "FastLED.h"

#include "VisualizerFX.h"

// Unlike LightFX only 1 visualizer is expected to run at a time. Do not need to save state in struct.

/*-----------------------------------------------------------------------------
  LightFX with Visualizer Input
  -----------------------------------------------------------------------------
  based on the original code by Michael Bartlett found at:
  https://github.com/bartlettmic/SparkFun-RGB-LED-Music-Sound-Visualizer-Arduino-Code
 *----------------------------------------------------------------------------*/
static CRGB * 	p_LEDStrip;
static uint16_t NumLEDs;
static const CRGBPalette16 * p_Palette;
static const CRGBPalette16 ** p_PaletteSync;

float VisualizerVolume				= 0;	//Holds the volume level read from the sound detector.
float VisualizerVolumeLast 			= 0;	//Holds the value of volume from the previous loop() pass.
float VisualizerVolumeAvg 			= 0;	//Holds the "average" volume-level to proportionally adjust the visual experience.
float VisualizerVolumeMax 			= 0;	//Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
float VisualizerVolumeThreshold 	= 0;

float VisualizerAvgBump = 0;		//Holds the "average" volume-change to trigger a "bump."
bool VisualizerBump = false;		//Used to pass if there was a "bump" in volume

float VisualizerTimeBump = 0;		//Holds the time (in runtime seconds) the last "bump" occurred.
float VisualizerAvgTimeBump = 0;	//Holds the "average" amount of time between each "bump" (used for pacing the dot's movement).


void VisualizerFX_Update(float v)
{
	VisualizerVolume = v;
}

void VisualizerFX_SetThresholdMic()
{
	VisualizerVolumeThreshold = .05;
}

void VisualizerFX_SetThresholdWav()
{
	VisualizerVolumeThreshold = 0;
}

void VisualizerFX_ResetVars()
{
	VisualizerVolume = 0;
	VisualizerVolumeMax = 0;
	VisualizerVolumeAvg = 0;
	VisualizerVolumeLast 	= 0;

	VisualizerAvgBump = 0;
	VisualizerBump = false;

	VisualizerTimeBump = 0;
	VisualizerAvgTimeBump = 0;
}

void VisualizerFX_SetPalette(const CRGBPalette16 * p_palette)
{
	p_Palette	= p_palette;
}

void VisualizerFX_CalcVars()
{
	  //Sets a threshold for volume.
	  //  In practice I've found noise can get up to 15, so if it's lower, the visual thinks it's silent.
	  //  Also if the volume is less than average volume / 2 (essentially an average with 0), it's considered silent.
	  //if (volume < avgVol / 2.0 || volume < 15) volume = 0;
	  if (VisualizerVolume < VisualizerVolumeAvg / 2.0 || VisualizerVolume < VisualizerVolumeThreshold) VisualizerVolume = 0;
	  //else avgVol = (avgVol + volume) / 2.0; //If non-zeo, take an "average" of volumes.
	  else VisualizerVolumeAvg = (VisualizerVolumeAvg + VisualizerVolume) / 2.0; //If non-zero, take an "average" of volumes.

	  //If the current volume is larger than the loudest value recorded, overwrite
	  //if (volume > maxVol) maxVol = volume;
	  if (VisualizerVolume > VisualizerVolumeMax) VisualizerVolumeMax = VisualizerVolume;
	  //Experimental, adjust "Max" to not lose brightness intensity permanently
	  else if(VisualizerVolume > (VisualizerVolumeAvg + VisualizerVolumeMax) / 2.0) VisualizerVolumeMax = (VisualizerVolume + VisualizerVolumeMax) / 2.0;

//	  //This is where "gradient" is modulated to prevent overflow.
//	  if (gradient > thresholds[palette]) {
//	    gradient %= thresholds[palette] + 1;
//
//	    //Everytime a palette gets completed is a good time to readjust "maxVol," just in case
//	    //  the song gets quieter; we also don't want to lose brightness intensity permanently
//	    //  because of one stray loud sound.
//	    maxVol = (maxVol + volume) / 2.0;
//	  }

	  //If there is a decent change in volume since the last pass, average it into "avgBump"
	  //if (volume - last > 10) avgBump = (avgBump + (volume - last)) / 2.0;
	  if (VisualizerVolume - VisualizerVolumeLast > .05) VisualizerAvgBump = (VisualizerAvgBump + (VisualizerVolume - VisualizerVolumeLast)) / 2.0;

	  //If there is a notable change in volume, trigger a "bump"
	  //  avgbump is lowered just a little for comparing to make the visual slightly more sensitive to a beat.
	  //bump = (volume - last > avgBump * .9);
	  VisualizerBump = (VisualizerVolume - VisualizerVolumeLast > VisualizerAvgBump * .9);

	  //If a "bump" is triggered, average the time between bumps
	  if (VisualizerBump) {
	    VisualizerAvgTimeBump = (((millis() / 1000.0) - VisualizerTimeBump) + VisualizerAvgTimeBump) / 2.0;
	    VisualizerTimeBump = millis() / 1000.0;
	  }

	  //last = volume; //Records current volume for next pass
	  VisualizerVolumeLast = VisualizerVolume; //Records current volume for next pas
}

//PULSE
//Pulse from center of the strand
void Pulse()
{
	  static uint8_t gradient = 0;
	  gradient++;
	  VisualizerFX_CalcVars();

	  //Listed below, this function simply dims the colors a little bit each pass of loop()
	  //fade(0.75);
	  fadeToBlackBy(p_LEDStrip, NumLEDs, 64);

	  //Advances the palette to the next noticeable color if there is a "bump"
	  //if (bump) gradient += thresholds[palette] / 24;
	  if (VisualizerBump) gradient += 255 / 24;

	  //If it's silent, we want the fade effect to take over, hence this if-statement
	  if (VisualizerVolume > 0) {
	    //Our retrieved 32-bit color
		//CRGB intendedColor =  ColorFromPalette(*p_Palette, gradient); // for some reason intendedColor.r = intendedColor.r * damp; results in 0, if intendedColor is declared here.

	    //These variables determine where to start and end the pulse since it starts from the middle of the strand.
	    //  The quantities are stored in variables so they only have to be computed once (plus we use them in the loop).
	    int start = NumLEDs/2 - (NumLEDs/2 * (VisualizerVolume / VisualizerVolumeMax));
	    int finish = NumLEDs/2 + (NumLEDs/2 * (VisualizerVolume / VisualizerVolumeMax)) + NumLEDs % 2;
	    //Listed above, LED_HALF is simply half the number of LEDs on your strand. this part adjusts for an odd quantity.

	    for (int i = start; i < finish; i++) {

	      //"damp" creates the fade effect of being dimmer the farther the pixel is from the center of the strand.
	      //  It returns a value between 0 and 1 that peaks at 1 at the center of the strand and 0 at the ends.
	      float damp = sin((i - start) * PI / float(finish - start));

	      //Squaring damp creates more distinctive brightness.
	      damp = pow(damp, 2.0);

	      //Fetch the color at the current pixel so we can see if it's dim enough to overwrite.
	      CRGB existingColor = p_LEDStrip[i];

	      //Takes advantage of one for loop to do the following:
	      // Appropriatley adjust the brightness of this pixel using location, volume, and "knob"
	      // Take the average RGB value of the intended color and the existing color, for comparison
//	      uint8_t colors[3];
//	      float avgCol = 0, avgCol2 = 0;
//	      for (int k = 0; k < 3; k++) {
//	        colors[k] = split(col, k) * damp * knob * pow(volume / maxVol, 2);
//	        avgCol += colors[k];
//	        avgCol2 += split(col2, k);
//	      }
//	      avgCol /= 3.0, avgCol2 /= 3.0;

	      //CRGB color; // for some reason intendedColor.r = intendedColor.r * damp; results in 0, if intendedColor is declared earlier.
	      CRGB intendedColor =  ColorFromPalette(*p_Palette, gradient);
	      intendedColor.r = intendedColor.r * damp * pow(VisualizerVolume / VisualizerVolumeMax, 2);
	      intendedColor.g = intendedColor.g * damp * pow(VisualizerVolume / VisualizerVolumeMax, 2);
	      intendedColor.b = intendedColor.b * damp * pow(VisualizerVolume / VisualizerVolumeMax, 2);

	      //Compare the average colors as "brightness". Only overwrite dim colors so the fade effect is more apparent.
	      if (intendedColor > existingColor) p_LEDStrip[i] =  intendedColor;
	    }
	  }
	  //Experimental, fade a little faster if low volume
	  //else fadeToBlackBy(p_LEDStrip, NumLEDs, 63);
}

//PALETTEPULSE
//Same as Pulse(), but colored the entire pallet instead of one solid color
void PalettePulse() {
	static uint8_t gradient = 0;
	
	gradient++;
	VisualizerFX_CalcVars();
	
	fadeToBlackBy(p_LEDStrip, NumLEDs, 64);

	if (VisualizerBump) gradient += 255 / 24;

	if (VisualizerVolume > 0) {
		int start = NumLEDs/2 - (NumLEDs/2 * (VisualizerVolume / VisualizerVolumeMax));
		int finish = NumLEDs/2 + (NumLEDs/2 * (VisualizerVolume / VisualizerVolumeMax)) + NumLEDs % 2;
		for (int i = start; i < finish; i++) {
			float damp = sin((i - start) * PI / float(finish - start));
			damp = pow(damp, 2.0);

			//This is the only difference from Pulse(). The color for each pixel isn't the same, but rather the
			//  entire gradient fitted to the spread of the pulse, with some shifting from "gradient".
			int val = 255 * (i - start) / (finish - start);
			val += gradient;
			CRGB intendedColor =  ColorFromPalette(*p_Palette, val);
			CRGB existingColor = p_LEDStrip[i];

			intendedColor.r = intendedColor.r * damp * pow(VisualizerVolume / VisualizerVolumeMax, 2);
			intendedColor.g = intendedColor.g * damp * pow(VisualizerVolume / VisualizerVolumeMax, 2);
			intendedColor.b = intendedColor.b * damp * pow(VisualizerVolume / VisualizerVolumeMax, 2);

			if (intendedColor > existingColor) p_LEDStrip[i] =  intendedColor;
		}
	}
}

//For Traffic() visual
int8_t * TrafficPos;    //Stores a population of color "dots" to iterate across the LED strand.
CRGB * TrafficRGB;  //Stores each dot's specific RGB values.

void Traffic() {
	static uint8_t gradient = 0;

	gradient++;
	VisualizerFX_CalcVars();

	//fade() actually creates the trail behind each dot here, so it's important to include.
	fadeToBlackBy(p_LEDStrip, NumLEDs, 50);

	//Create a dot to be displayed if a bump is detected.
	if (VisualizerBump) {

		//This mess simply checks if there is an open position (-2) in the pos[] array.
		int8_t slot = 0;
		for (slot; slot < NumLEDs; slot++) {
			if (TrafficPos[slot] < -1)	break;
			else if (slot + 1 >= NumLEDs) {
				slot = -3;
				break;
			}
		}

		//If there is an open slot, set it to an initial position on the strand.
		if (slot != -3) {

			//Evens go right, odds go left, so evens start at 0, odds at the largest position.
			TrafficPos[slot] = (slot % 2 == 0) ? -1 : NumLEDs;

			//Give it a color based on the value of "gradient" during its birth.
			TrafficRGB[slot] = ColorFromPalette(*p_Palette, gradient);

			gradient += 255 / 24;
		}
	}

	//Again, if it's silent we want the colors to fade out.
	if (VisualizerVolume > 0) {

		//If there's sound, iterate each dot appropriately along the strand.
		for (int i = 0; i < NumLEDs; i++) {

			//If a dot is -2, that means it's an open slot for another dot to take over eventually.
			if (TrafficPos[i] < -1)
				continue;

			//As above, evens go right (+1) and odds go left (-1)
			TrafficPos[i] += (i % 2) ? -1 : 1;

			//Odds will reach -2 by subtraction, but if an even dot goes beyond the LED strip, it'll be purged.
			if (TrafficPos[i] >= NumLEDs) TrafficPos[i] = -2;

			//Set the dot to its new position and respective color.
			//  I's old position's color will gradually fade out due to fade(), leaving a trail behind it.
			p_LEDStrip[TrafficPos[i]].r = TrafficRGB[i].r * pow(VisualizerVolume / VisualizerVolumeMax, 2);
			p_LEDStrip[TrafficPos[i]].g = TrafficRGB[i].g * pow(VisualizerVolume / VisualizerVolumeMax, 2);
			p_LEDStrip[TrafficPos[i]].b = TrafficRGB[i].b * pow(VisualizerVolume / VisualizerVolumeMax, 2);
		}
	}
}


//For Snake() visual
bool left = false;  //Determines the direction of iteration. Recycled in PaletteDance()
int8_t dotPos = 0;  //Holds which LED in the strand the dot is positioned at. Recycled in most other visuals.

//SNAKE
//Dot sweeping back and forth to the beat
void Snake() {
	static uint8_t gradient = 0;

	gradient++;
	VisualizerFX_CalcVars();

	if (VisualizerBump) {

		//Change color a little on a bump
		gradient += 255 / 30;

		//Change the direction the dot is going to create the illusion of "dancing."
		left = !left;
	}

	fadeToBlackBy(p_LEDStrip, NumLEDs, 6); //Leave a trail behind the dot.

	CRGB col = ColorFromPalette(*p_Palette, gradient); //Get the color at current "gradient."

	//The dot should only be moved if there's sound happening.
	//  Otherwise if noise starts and it's been moving, it'll appear to teleport.
	if (VisualizerVolume > 0) {

		//Check if dot position is out of bounds.
		if (dotPos < 0)					dotPos = NumLEDs - 1;
		else if (dotPos >= NumLEDs)		dotPos = 0;

		//Sets the dot to appropriate color and intensity
		p_LEDStrip[dotPos].r = col.r * pow(VisualizerVolume / VisualizerVolumeMax, 1.5);
		p_LEDStrip[dotPos].g = col.g * pow(VisualizerVolume / VisualizerVolumeMax, 1.5);
		p_LEDStrip[dotPos].b = col.b * pow(VisualizerVolume / VisualizerVolumeMax, 1.5);

		//This is where "avgTime" comes into play.
		//  That variable is the "average" amount of time between each "bump" detected.
		//  So we can use that to determine how quickly the dot should move so it matches the tempo of the music.
		//  The dot moving at normal loop speed is pretty quick, so it's the max speed if avgTime < 0.15 seconds.
		//  Slowing it down causes the color to update, but only change position every other amount of loops.
		if (VisualizerAvgTimeBump < 0.15)																dotPos += (left) ? -1 : 1;
		else if (VisualizerAvgTimeBump >= 0.15 && VisualizerAvgTimeBump < 0.5 && gradient % 2 == 0)		dotPos += (left) ? -1 : 1;
		else if (VisualizerAvgTimeBump >= 0.5 && VisualizerAvgTimeBump < 1.0 && gradient % 3 == 0)		dotPos += (left) ? -1 : 1;
		else if (gradient % 4 == 0)																		dotPos += (left) ? -1 : 1;
	}
}

//PALETTEDANCE
//Projects a whole palette which oscillates to the beat, similar to the snake but a whole gradient instead of a dot
void PaletteDance() {
	static uint8_t gradient = 0;
	gradient++;
	VisualizerFX_CalcVars();

	//This is the most calculation-intensive visual, which is why it doesn't need delayed.

	if (VisualizerBump)	left = !left; //Change direction of iteration on bump

	//Only show if there's sound.
	if (VisualizerVolume > VisualizerVolumeAvg) {

		//Loop "dotPos" if it goes out of bounds.
		if (dotPos < 0)								dotPos = NumLEDs - NumLEDs / 6;
		else if (dotPos >= NumLEDs - NumLEDs / 6)	dotPos = 0;

		//This next part is convoluted, here's a summary of what's happening:
		//  First, a sin wave function is introduced to change the brightness of all the pixels (stored in "sinVal")
		//      This is to make the dancing effect more obvious. The trick is to shift the sin wave with the color so it all appears
		//      to be the same object, one "hump" of color. "dotPos" is added here to achieve this effect.
		//  Second, the entire current palette is proportionally fitted to the length of the LED strand (stored in "val" each pixel).
		//      This is done by multiplying the ratio of position and the total amount of LEDs to the palette's threshold.
		//  Third, the palette is then "shifted" (what color is displayed where) by adding "dotPos."
		//      "dotPos" is added to the position before dividing, so it's a mathematical shift. However, "dotPos"'s range is not
		//      the same as the range of position values, so the function map() is used. It's basically a built in proportion adjuster.
		//  Lastly, it's all multiplied together to get the right color, and intensity, in the correct spot.
		//      "gradient" is also added to slowly shift the colors over time.
		for (int i = 0; i < NumLEDs; i++) {

			float sinVal = abs(	sin( (i + dotPos) * (PI / float(NumLEDs / 1.25) ) ));
			sinVal *= sinVal;
			sinVal *= VisualizerVolume / VisualizerVolumeMax;

			unsigned int val = float(255.0 + 1.0)
								//map takes a value between -LED_TOTAL and +LED_TOTAL and returns one between 0 and LED_TOTAL
								* (float(i	+ map(dotPos, -1 * (NumLEDs - 1), NumLEDs - 1, 0, NumLEDs - 1)) / float(NumLEDs))
								+ (gradient);

			val %= 255; //make sure "val" is within range of the palette

			CRGB col = ColorFromPalette(*p_Palette, val); //get the color at "val"

			p_LEDStrip[i].r = col.r * sinVal;
			p_LEDStrip[i].g = col.g * sinVal;
			p_LEDStrip[i].b = col.b * sinVal;
		}

		//After all that, appropriately reposition "dotPos."
		dotPos += (left) ? -1 : 1;
	}

	//If there's no sound, fade.
	else fadeToBlackBy(p_LEDStrip, NumLEDs, 50);
}

//"Bleeds" colors currently in the strand by averaging from a designated "Point"
void Bleed(uint8_t Point) {
	for (int i = 1; i < NumLEDs; i++) {

		//Starts by look at the pixels left and right of "Point"
		//  then slowly works its way out
		int sides[] = {Point - i, Point + i};

		for (int j = 0; j < 2; j++) {

			//For each of Point+i and Point-i, the pixels to the left and right, plus themselves, are averaged together.
			//  Basically, it's setting one pixel to the average of it and its neighbors, starting on the left and right
			//  of the starting "Point," and moves to the ends of the strand
			int point = sides[j];
			//CRGB colors[] = {p_LEDStrip[point - 1], p_LEDStrip[point], p_LEDStrip[point + 1]};
			CRGB colors[3] = 0;

			if (point - 1 > 0)						colors[0] = p_LEDStrip[point - 1];
			if (point > 0 && point < NumLEDs - 1)	colors[1] = p_LEDStrip[point];
			if (point + 1 < NumLEDs - 1)			colors[2] = p_LEDStrip[point + 1];

			//Sets the new average values to just the central point, not the left and right points.
			if (point > 0 && point < NumLEDs - 1) {
				p_LEDStrip[point].r = float(colors[0].r + colors[1].r + colors[2].r) / 3.0;
				p_LEDStrip[point].g = float(colors[0].g + colors[1].g + colors[2].g) / 3.0;
				p_LEDStrip[point].b = float(colors[0].b + colors[1].b + colors[2].b) / 3.0;
			}
		}
	}
}

//GLITTER
//Creates white sparkles on a color palette to the beat
void Glitter() {
	static uint8_t gradient = 0;
	gradient++;
	VisualizerFX_CalcVars();

	//This visual also fits a whole palette on the entire strip
	//  This just makes the palette cycle more quickly so it's more visually pleasing
	//gradient += 255 / 204;
	gradient += 2;

	//"val" is used again as the proportional value to pass to ColorPalette() to fit the whole palette.
	for (int i = 0; i < NumLEDs; i++) {
		unsigned int val = float(255.0 + 1.0)
							* (float(i) / float(NumLEDs))
							+ (gradient);
		val %= 255;
		CRGB col = ColorFromPalette(*p_Palette, val);

		//We want the sparkles to be obvious, so we dim the background color.
		p_LEDStrip[i].r = col.r / 6.0;
		p_LEDStrip[i].g = col.g / 6.0;
		p_LEDStrip[i].b = col.b / 6.0;
	}

	//Create sparkles every bump
	if (VisualizerBump) {

		//Random generator needs a seed, and micros() gives a large range of values.
		//  micros() is the amount of microseconds since the program started running.
		//randomSeed(micros());

		//Pick a random spot on the strand.
		dotPos = random8(NumLEDs - 1);

		//Draw  sparkle at the random position, with appropriate brightness.
		p_LEDStrip[dotPos].r = 255.0 * pow(VisualizerVolume / VisualizerVolumeMax, 2.0);
		p_LEDStrip[dotPos].g = 255.0 * pow(VisualizerVolume / VisualizerVolumeMax, 2.0);
		p_LEDStrip[dotPos].b = 255.0 * pow(VisualizerVolume / VisualizerVolumeMax, 2.0);
	}
	Bleed(dotPos);
}


//PAINTBALL
//Recycles Glitter()'s random positioning; simulates "paintballs" of
//  color splattering randomly on the strand and bleeding together.
void Paintball() {
	static uint8_t gradient = 0;
	gradient++;
	VisualizerFX_CalcVars();

	//If it's been twice the average time for a "bump" since the last "bump," start fading.
	if ((millis() / 1000.0) - VisualizerTimeBump > VisualizerAvgTimeBump * 2.0)	fadeToBlackBy(p_LEDStrip, NumLEDs, 3);

	//Bleeds colors together. Operates similarly to fade. For more info, see its definition below
	Bleed(dotPos);

	//Create a new paintball if there's a bump (like the sparkles in Glitter())
	if (VisualizerBump) {

		//Random generator needs a seed, and micros() gives a large range of values.
		//  micros() is the amount of microseconds since the program started running.
		//randomSeed(micros());

		//Pick a random spot on the strip. Random was already reseeded above, so no real need to do it again.
		dotPos = random8(NumLEDs - 1);

		//Grab a random color from our palette.
		CRGB col = ColorFromPalette(*p_Palette, random8(255));

		//Array to hold final RGB values
		CRGB finalColor;

		//Relates brightness of the color to the relative volume and potentiometer value.
		finalColor.r = col.r * pow(VisualizerVolume / VisualizerVolumeMax, 2.0);
		finalColor.g = col.g * pow(VisualizerVolume / VisualizerVolumeMax, 2.0);
		finalColor.b = col.b * pow(VisualizerVolume / VisualizerVolumeMax, 2.0);

		//Splatters the "paintball" on the random position.
		p_LEDStrip[dotPos] = finalColor;

		//This next part places a less bright version of the same color next to the left and right of the
		//  original position, so that the bleed effect is stronger and the colors are more vibrant.
		finalColor.r = float(finalColor.r) * .8;
		finalColor.g = float(finalColor.g) * .8;
		finalColor.b = float(finalColor.b) * .8;

		if (dotPos > 0)				p_LEDStrip[dotPos - 1] = finalColor;
		if (dotPos < NumLEDs - 1)	p_LEDStrip[dotPos + 1] = finalColor;
	}
}

void Barometer()
{
	static uint8_t gradient = 0;
	gradient++;
	VisualizerFX_CalcVars();

	float height = VisualizerVolume / VisualizerVolumeMax;

	uint16_t n;
	for (n = 0; n < height * (NumLEDs - 1); n++)	p_LEDStrip[n] = ColorFromPalette(*p_Palette, gradient);
	for (; n < NumLEDs; n++)						p_LEDStrip[n] = 0;
}


void VisualizerFX_Init(CRGB * ledStrip, uint16_t ledStart, uint16_t ledLength, const CRGBPalette16 * startingPalette, CRGB * colorBuffer, uint8_t * valBuffer, const CRGBPalette16 ** paletteSync)
{
	p_LEDStrip 	= &ledStrip[ledStart];
	NumLEDs 	= ledLength;
	p_Palette	= startingPalette;

	TrafficPos = (int8_t *)valBuffer;
	valBuffer[0] = -2;
	TrafficRGB = colorBuffer;

	if (paletteSync) p_PaletteSync = paletteSync;
}

/*-----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
static uint8_t VisualizerFXTableIndex = 0;

void (*VISUALIZER_FX_ARRAY[])(void) =
{
	&Pulse,
	&PalettePulse,
	&Traffic,
	&Snake,
	&PaletteDance,
	&Glitter,
	&Paintball,
	&Barometer,
};

void (*VisualizerFXTableGet(void))(void)
{
	return VISUALIZER_FX_ARRAY[VisualizerFXTableIndex];
}

void (*VisualizerFXTableGetNext(void))(void)
{
	if (VisualizerFXTableIndex < sizeof(VISUALIZER_FX_ARRAY)/sizeof(void *) - 1)
		VisualizerFXTableIndex++;
	else
		VisualizerFXTableIndex = 0;

	return VISUALIZER_FX_ARRAY[VisualizerFXTableIndex];
}

void (*VisualizerFXTableGetPrev(void))(void)
{
	if (VisualizerFXTableIndex > 0)
		VisualizerFXTableIndex--;
	else
		VisualizerFXTableIndex = sizeof(VISUALIZER_FX_ARRAY)/sizeof(void *) - 1;

	return VISUALIZER_FX_ARRAY[VisualizerFXTableIndex];
}
