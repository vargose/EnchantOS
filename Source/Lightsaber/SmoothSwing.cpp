// Algorithm created by Thexter
// https://www.fx-sabers.com/forum/index.php?topic=51430.0
// http://therebelarmory.com/thread/9138/smoothswing-v2-algorithm-description

#include <math.h>
#include "WProgram.h"

float SwingStrength;
float SmoothSwingSensitivity = 540;	// a total of 540 degrees per second or greater between all axes will apply the loudest swing sound
float SmoothSwingAmountA;
float SmoothSwingAmountB;

float m_flMultiHumCyclePeriod = 2*PI/1; // a full 2PI transition takes 1 seconds

// Saber is along x-axis

/******************************************************************************/
/*!
 * @name SmoothSwing V1
 * @brief
 */
/******************************************************************************/
float m_flMultiHumSensitivity = 540; 	// 540 degrees per second or greater will apply the max contribution from a paired hum

void SmoothSwingV1(uint32_t nCurrentTime, float gx, float gy, float gz)
{
	// gx, gy, gz are all instantaneous gyro values
	// flTotalRotation is the total angular velocity of gx, gy, gz
	// m_flMultiHumCyclePeriod is actually poorly named and is really the inverse of the period.

	float flTotalRotation = abs(gx) + abs(gy) + abs(gz);

	// Periodicity / Rotation of the basis
	float flCycleTime = ( nCurrentTime / 1000.0f ) * m_flMultiHumCyclePeriod;
	float flYAmount = sinf( flCycleTime );
	float flZAmount = cosf( flCycleTime );

	// Normalize to the swing sensitivity and clamp
	float flNormalizedY = max( -1.0f, min( 1.0f, gy / m_flMultiHumSensitivity ) );
	float flNormalizedZ = max( -1.0f, min( 1.0f, gz / m_flMultiHumSensitivity ) );

	// Apply the rotation
	float flMultiHumAmount0 = flYAmount * flNormalizedY;
	float flMultiHumAmount1 = flZAmount * flNormalizedZ;

	// Store off total strength
	SwingStrength = min( 1.0f, flTotalRotation / SmoothSwingSensitivity );

	// Multi-hum amount is the sum of both axes (both go from -1..1 range) divided by sqrt(2). Think of the case where both are 1.
	float m_flMultiHumAmount = ( flMultiHumAmount0 + flMultiHumAmount1 ) / 1.414214f;

	if (m_flMultiHumAmount > 0)
	{
		SmoothSwingAmountA 	= m_flMultiHumAmount;
		SmoothSwingAmountB 	= 0;
	}
	else
	{
		SmoothSwingAmountA 	= 0;
		SmoothSwingAmountB 	= abs(m_flMultiHumAmount);
	}
}

/******************************************************************************/
/*!
 * @name SmoothSwing V1S
 * @brief  A modified version of V1
 */
/******************************************************************************/
void SmoothSwingV1S(uint32_t nCurrentTime, float gx, float gy, float gz)
{
	float flTotalRotation = sqrtf( powf(gx,2) + powf(gy,2) + powf(gz,2) );
	float flCycleTime = ( nCurrentTime / 1000.0f ) * m_flMultiHumCyclePeriod;

	SwingStrength = min( 1.0f, flTotalRotation / SmoothSwingSensitivity );

	SmoothSwingAmountA 	= (sinf(flCycleTime			)+1)/2;
	SmoothSwingAmountB 	= (sinf(flCycleTime + PI	)+1)/2;
}

/******************************************************************************/
/*!
 * @name SmoothSwing V2
 * @brief
 */
/******************************************************************************/
//float g_flMaximumHumDucking = 0.75f;
float g_flSwingSharpness = 1; //<some value between 1.0 and 2.0>
float g_flSameSwingDirThreshold = 0.0f; // This seems to be a reasonable value, but experiment
float g_flSwingStrengthThreshold = 0.05f; // Again, configurable

float m_flMinPitchBoundaryRads = 10;
float m_flMaxPitchBoundaryRads = 60;

float m_flHalfTransition1AngleRads = 30; // changed to degrees
float m_flHalfTransition2AngleRads = 90;

float m_flPitchBoundary1;
float m_flPitchBoundary2;

float m_flSwingHiStart;
float m_flSwingLowEnd;

float m_flPitchBoundaryLerp;
float m_flSwingPitchLowHiLerp;

void SmoothSwingV2(uint32_t nCurrentTime, float gx, float gy, float gz)
{
	// read raw accel/gyro measurements from device
	// All values are in Gs for accel and degrees/s for gyro

	// Keep track of how long a "Frame" is.
	// Use this to integrate angular velocity in DoNewSwing

	static float m_nLastUpdateTime;

	float flTimeSinceLastUpdate = ( nCurrentTime - m_nLastUpdateTime ) / 1000.0f;
	m_nLastUpdateTime = nCurrentTime;

	// Calculate magnitude of the angular velocity
	float flTotalAngularVelSq = powf(gx,2) + powf(gy,2) + powf(gz,2);
	float flTotalAngularVel = sqrtf( flTotalAngularVelSq );

	// Smooth this value out (box filter over X frames)
//	m_flSmoothedTotalAngularVel.StoreValue( flTotalAngularVel );
//	flTotalAngularVel = m_flSmoothedTotalAngularVel.GetSmoothedValue();

	// Calculate how HARD our swing is
	// This is a normalized value from 0..1, clamped at 1
	SwingStrength = min( 1.0f, flTotalAngularVel / SmoothSwingSensitivity );

	// instantaneous angular velocity vector
	// used for acceleration direction changes and smoothed by the called code
//	Vector vCurrentGyroValues;
//	vCurrentGyroValues.x = gx;
//	vCurrentGyroValues.y = gy;
//	vCurrentGyroValues.z = gz;

	// On second thought, when you've tried as many swing algorithms as I have, using the convention New, really doesn't mean anything.
	//DoNewSwing( vCurrentGyroValues, flTotalAngularVel , flTimeSinceLastUpdate );

	// How far have we travelled?
	static float m_flTotalRotationThisSwing;

	m_flTotalRotationThisSwing += flTotalAngularVel * flTimeSinceLastUpdate;

	// Wrap
	float flWrappedRotationThisSwing = fmod(m_flTotalRotationThisSwing, 2*PI);

	// See if we are in transition zone 1
	float flAngleToTransition1 =  m_flPitchBoundary1 - flWrappedRotationThisSwing;
	if ( flAngleToTransition1 > m_flPitchBoundary1 - m_flHalfTransition1AngleRads && flAngleToTransition1 < m_flPitchBoundary1 + m_flHalfTransition1AngleRads )
	{
		// How far across the zone are we?
		float flPercentAcross = ( flAngleToTransition1 + m_flHalfTransition1AngleRads ) / ( m_flHalfTransition1AngleRads * 2.0f );

		// That's how much we lerp between the two looping hum pairs
		m_flPitchBoundaryLerp = flPercentAcross;
	}

	// Pitch boundary 2 is only for spins. If you've gone at least 180 past the pitch boundary, assume it's a spin.
	// In the second pitch boundary, attenuate the swing strength and change the pitch lerp back to 0 so that the next time
	// through the pitch boundary produces yet another pitch shift. Attenuation hides the instant change in the pitch lerp.

	// See if we are in transition zone 2
	float flAngleToTransition2 = m_flPitchBoundary2 - flWrappedRotationThisSwing;
	if ( flAngleToTransition2 > m_flPitchBoundary2 - m_flHalfTransition2AngleRads && flAngleToTransition2 < m_flPitchBoundary2 + m_flHalfTransition2AngleRads )
	{
		// How far across the zone are we?
		float flPercentAcross = ( flAngleToTransition2 + m_flHalfTransition2AngleRads ) / ( m_flHalfTransition2AngleRads * 2.0f );

		// Since we're going back the other way, we do 1-flPercentAcross
		m_flPitchBoundaryLerp = 1.0f - flPercentAcross;
	}

	// Quick remap in case we flipped which sounds were start and end.
	// Note, the value of m_flPitchBoundaryLerp persists so that it stays as whatever it was left at in the event we leave a transition zone.
	m_flSwingPitchLowHiLerp = m_flSwingHiStart * ( 1.0f - m_flPitchBoundaryLerp ) + m_flSwingLowEnd * m_flPitchBoundaryLerp;

	// What's our current non-smoothed accel direction?
	// If we want to, we can assume that the average saber is about 1m in length.
	// Then we can think of the gyro values as the velocities at the tip of the saber.
	// The difference is a really bad approx for the first derivative.
	// Blah Blah Blah...
	// In reality, we're just looking for a change in acceleration big enough that we'll want to do a new sound
//	Vector vSwingAccel = vCurrentGyroValues - m_vLastGyroValues;
//	m_vLastGyroValues = vCurrentGyroValues;

	static float lastGX;
	static float lastGY;
	static float lastGZ;

	float gxAccel = gx - lastGX;
	float gyAccel = gy - lastGY;
	float gzAccel = gz - lastGZ;

	lastGX = gx;
	lastGY = gy;
	lastGZ = gz;

	float magnitude = sqrtf(powf(gxAccel,2) + pow(gyAccel,2) + powf(gzAccel,2));

	// We only care about the direction of the change, not the magnitude
	//vSwingAccel.NormalizeInPlace();
	gxAccel = gx / magnitude;
	gyAccel = gy / magnitude;
	gzAccel = gz / magnitude;

	// Box filter over the last 5 vSwingAccel values
//	Vector vSmoothedLastSwingAccel = GetSmoothedLastSwingAccel();

	// Determine if we've changed directions. We do this for cases where we may not actually go below our strength threshold, but have changed motion enough to warrant a new swing.
	// In order to change acceleration direction, we'll need to have approached a zero velocity at least along the projection of the dot product between the two accel vector changes.
	//float flSameDir = DotProduct( vSmoothedLastSwingAccel, vSwingAccel );

	static float lastGXAccel;
	static float lastGYAccel;
	static float lastGZAccel;

	float flSameDir = gxAccel*lastGXAccel + gyAccel*lastGYAccel + gzAccel*lastGZAccel;

	lastGXAccel = gxAccel;
	lastGYAccel = gyAccel;
	lastGZAccel = gzAccel;

	// Keep track of the value we calculated this time so that it is used to calculate the smoothed value for next time
	//AddLastSwingAccel( vSwingAccel );

	// Either of two conditions for new swing
	// 1. The "sameness" of the direction of our acceleration is below a certain threshold
	// 2. We're not moving much
	if ( ( flSameDir < g_flSameSwingDirThreshold ) || ( SwingStrength < g_flSwingStrengthThreshold ) )
	{
		// When the saber is still, this will be firing EVERY tick/frame
		// That's ok, because we shouldn't be playing any swings then, so rapidly picking new ones won't be noticed
		//OnNewSwing();

		// Select which exact pair to use
//		m_nHumPair = rand() % NUM_SWING_PAIRS;

		// Select swing high-low starts
		// Do we start with the high sound or the low sound from the pair?
		if ( rand() % 100 > 50 )
		{
			m_flSwingHiStart = 1.0f;
			m_flSwingLowEnd = 0.0f;
		}
		else
		{
			m_flSwingHiStart = 0.0f;
			m_flSwingLowEnd = 1.0f;
		}

		// Current lerp between the two sounds in the current pair
		m_flPitchBoundaryLerp = 0.0f;

		// How far have we rotated without changing directions?
		m_flTotalRotationThisSwing = 0.0f;

		// How far do we need to rotate to change pitches?
		// Good values are between 10 and 60 degrees
		// Note, that the transition region is often larger than 10 degrees, so if m_flPitchBoundary ends up being close to 10,
		// we'll actually start off the swing in the middle of a transition. This is OK. More variation.
		m_flPitchBoundary1 = (rand() % (int)(m_flMaxPitchBoundaryRads - m_flMinPitchBoundaryRads + 1)) + m_flMinPitchBoundaryRads;

		// The second transition zone is 180 degrees from pitch boundary. This is only useful for spins where we've gone at least 180
		// from the pitch boundary without changing directions.
		m_flPitchBoundary2 = fmod( m_flPitchBoundary1 + M_PI, 2*PI );
	}

	SmoothSwingAmountA 	= m_flSwingPitchLowHiLerp;
	SmoothSwingAmountB 	= 1.0 - m_flSwingPitchLowHiLerp;
}

/******************************************************************************/
/*!
 * @name SmoothSwing V2S
 * @brief  A modified version of SmoothSwingV2
 */
/******************************************************************************/
void SmoothSwingV2S(uint32_t nCurrentTime, float gx, float gy, float gz)
{
	static float m_nLastUpdateTime;
	static float m_flTotalRotationThisSwing;

	float flTimeSinceLastUpdate = ( nCurrentTime - m_nLastUpdateTime ) / 1000.0f;
	m_nLastUpdateTime = nCurrentTime;

	float flTotalAngularVel = sqrtf( powf(gx,2) + powf(gy,2) + powf(gz,2) );

	SwingStrength = min( 1.0f, flTotalAngularVel / SmoothSwingSensitivity );

	m_flTotalRotationThisSwing += flTotalAngularVel * flTimeSinceLastUpdate;

	float flWrappedRotationThisSwing = fmod(m_flTotalRotationThisSwing, 2*PI);

	// gryo outputs degrees, sinf takes radians
	flWrappedRotationThisSwing = flWrappedRotationThisSwing*PI/180;

	SmoothSwingAmountA 	= (sinf(flWrappedRotationThisSwing		)+1)/2;
	SmoothSwingAmountB 	= (sinf(flWrappedRotationThisSwing + PI	)+1)/2;
}

float SmoothSwing_GetSwingStrength()
{
	return SwingStrength;
}

// SmoothSwingAmountA + SmoothSwingAmountB always equal to 1
float SmoothSwing_GetSwingA()
{
	return SmoothSwingAmountA;
}

float SmoothSwing_GetSwingB()
{
	return SmoothSwingAmountB;
}

// returns value such that:
// SwingAGain + SwingBGain + HumGain <= 1 + SwingOverdrive
// MinGainHum < HumGain < MaxGainHum
// SwingAGain + SwingBGain == MAX_GAIN_SWING, when SwingStrength == 1
float MaxGainHum  = .7; // 1 for loudest hum, or something less than 1 so swing sound is comparatively louder.
float MinGainHum  = .2;
#define MAX_GAIN_SWING (1.0f - MinGainHum + SwingOverdrive)

float SwingOverdrive = 0.1; // for louder swings at the expense of clipping. set depending on font.

// In this case the total volume increase from .7 to 1 over the strength of a swing.
// The percentage of the sound that is hum decreases from 100 to 20 percent.
// The percentage of the sound that is swing increases from  from 0 to 80 percent + SwingOverdrive %.

float SmoothSwing_GetSwingAGain()
{
	return SmoothSwingAmountA*SwingStrength*MAX_GAIN_SWING;
}

float SmoothSwing_GetSwingBGain()
{
	return SmoothSwingAmountB*SwingStrength*MAX_GAIN_SWING;
}

float SmoothSwing_GetHumGain()
{
	return MinGainHum + (1 - SmoothSwingAmountA*SwingStrength - SmoothSwingAmountB*SwingStrength) * (MaxGainHum - MinGainHum);
}

static elapsedMillis timeElapsed;

void debug_SmoothSwing()
{
	if (timeElapsed > 1000)
	{
		timeElapsed = 0;

		Serial.print("SmoothSwingAmountA = ");Serial.println( SmoothSwingAmountA);
		Serial.print("SmoothSwingAmountB = ");Serial.println( SmoothSwingAmountB);
		Serial.print("SwingStrength = ");Serial.println( SwingStrength);

		Serial.print("SmoothSwing_GetSwingAGain = ");Serial.println( SmoothSwing_GetSwingAGain());
		Serial.print("SmoothSwing_GetSwingBGain = ");Serial.println( SmoothSwing_GetSwingBGain());
		Serial.print("SmoothSwing_GetHumGain = ");Serial.println( SmoothSwing_GetHumGain());

		Serial.println( );
	}
}


