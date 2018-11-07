//Some wrapper functions around LSM6DS3 class.

#include <stdint.h>
#include <Wire.h>
#include "src/SparkFunLSM6DS3.h"
#include "IMU.h"

LSM6DS3 IMU( I2C_MODE, 0x6A );

static float AX, AY, AZ;
static float GX, GY, GZ;

// for cases where I2C bus gets stuck
//static uint32_t LockOutPolling;
//static uint16_t LockOutPollingTime = 10;

void IMU_Poll(uint32_t currentTime)
{
	static uint32_t supressPoll;

	uint8_t status;

	if (currentTime < supressPoll) return;

	supressPoll = currentTime + 1; //no more than once per ms

	IMU.readRegister(&status, LSM6DS3_ACC_GYRO_STATUS_REG);

	if( (status & LSM6DS3_ACC_GYRO_XLDA_DATA_AVAIL) == LSM6DS3_ACC_GYRO_XLDA_DATA_AVAIL)
	{
		AX = IMU.readFloatAccelX();
		AY = IMU.readFloatAccelY();
		AZ = IMU.readFloatAccelZ();
	}

	if( (status & LSM6DS3_ACC_GYRO_GDA_DATA_AVAIL) == LSM6DS3_ACC_GYRO_GDA_DATA_AVAIL)
	{
		GX = IMU.readFloatGyroX();
		GY = IMU.readFloatGyroY();
		GZ = IMU.readFloatGyroZ();
	}
}

static float ThresholdImpact = 7;
static float ThresholdSwingGyro = 600;

// algorithm for detection, can change to more complex algorithm later.
inline static bool DetectImpact(void)
{
	if (abs(AX) + abs(AZ) + abs(AZ) > ThresholdImpact)
		return 1;
	else
		return 0;
}

inline static bool DetectSwing(void)
{
//	if ( (abs(GX) > ThresholdSwingGyro || abs(GY)  > ThresholdSwingGyro || abs(GZ)  > ThresholdSwingGyro) )
//		return 1;
//	else
//		return 0;

	if ( (abs(GX) > 600 || abs(GY)  > 300 || abs(GZ)  > 300) )
		return 1;
	else
		return 0;
}

static uint32_t LockOutDetectSwing;
static uint32_t LockOutDetectImpact;

bool IMU_DetectImpact(uint32_t currentTime, uint32_t supressTime)
{
	if(currentTime < LockOutDetectImpact)
		return 0;

	if (DetectImpact())
	{
		LockOutDetectSwing	 = currentTime + supressTime;
		LockOutDetectImpact	 = currentTime + supressTime;
		return 1;
	}
	else
		return 0;
}

bool IMU_DetectSwing(uint32_t currentTime, uint32_t supressTime)
{
	if(currentTime < LockOutDetectSwing)
		return 0;

	if (DetectSwing())
	{
		LockOutDetectSwing	 = currentTime + supressTime;
		return 1;
	}
	else
		return 0;
}

bool IMU_DetectSwingDirectionChange(void)
{
	static float lastGX;
	static float lastGY;
	static float lastGZ;

	static float lastGXAccel;
	static float lastGYAccel;
	static float lastGZAccel;

	float gxAccel = GX - lastGX;
	float gyAccel = GY - lastGY;
	float gzAccel = GZ - lastGZ;

	lastGX = GX;
	lastGY = GY;
	lastGZ = GZ;

	float magnitude = sqrtf(powf(gxAccel,2) + powf(gyAccel,2) + powf(gzAccel,2));

	gxAccel = GX / magnitude;
	gyAccel = GY / magnitude;
	gzAccel = GZ / magnitude;

	lastGXAccel = gxAccel;
	lastGYAccel = gyAccel;
	lastGZAccel = gzAccel;

	float dotProduct = gxAccel*lastGXAccel + gyAccel*lastGYAccel + gzAccel*lastGZAccel;

	if (dotProduct < 0) return true;
	else 				return false;
}

bool IMU_DetectMovement(void)
{

}

void IMU_SetThresholdImpact(float g)
{
//	if (userInput > 10)
//	if (userInput < 0)
	ThresholdImpact = g;
	//ThresholdImpact = 10*sensitivity/9;
}

void IMU_SetThresholdSwing(float d)
{
	ThresholdSwingGyro	= d;
}

void IMU_Init() //uint8_t sda, uint8_t scl
{
	Wire.setSCL(16);
	Wire.setSDA(17);

	IMU.settings.gyroEnabled = 1;  			//Can be 0 or 1
	IMU.settings.gyroRange = 1000;   		//Max deg/s.  Can be: 125, 245, 500, 1000, 2000
	IMU.settings.gyroSampleRate = 833;   	//Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666
	IMU.settings.gyroBandWidth = 100;  		//Hz.  Can be: 50, 100, 200, 400;
	IMU.settings.gyroFifoEnabled = 0;  		//Set to include gyro in FIFO
	IMU.settings.gyroFifoDecimation = 0;  	//set 1 for on /1

	IMU.settings.accelEnabled = 1;
	IMU.settings.accelRange = 8;      		//Max G force readable.  Can be: 2, 4, 8, 16
	IMU.settings.accelSampleRate = 833;  	//Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
	IMU.settings.accelBandWidth = 400;  	//Hz.  Can be: 50, 100, 200, 400;
	IMU.settings.accelFifoEnabled = 0;  	//Set to include accelerometer in the FIFO
	IMU.settings.accelFifoDecimation = 0;  	//set 1 for on /1
	IMU.settings.tempEnabled = 0;

	IMU.begin();

	uint8_t dataToWrite = 0;

	//IMU.begin() doesnt seem to use gyroBandWidth;
}

void IMU_PrintSerial()
{
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(IMU.readFloatAccelX(), 4);
    Serial.print(" Y = ");
    Serial.println(IMU.readFloatAccelY(), 4);
    Serial.print(" Z = ");
    Serial.println(IMU.readFloatAccelZ(), 4);

    Serial.print("\nGyroscope:\n");
    Serial.print(" X = ");
    Serial.println(IMU.readFloatGyroX(), 4);
    Serial.print(" Y = ");
    Serial.println(IMU.readFloatGyroY(), 4);
    Serial.print(" Z = ");
    Serial.println(IMU.readFloatGyroZ(), 4);

//    Serial.print("\nAccelerometer:\n");
//    Serial.print(" X = ");
//    Serial.println(IMU.readRawAccelX());
//    Serial.print(" Y = ");
//    Serial.println(IMU.readRawAccelY());
//    Serial.print(" Z = ");
//    Serial.println(IMU.readRawAccelZ());
//
//    Serial.print("\nGyroscope:\n");
//    Serial.print(" X = ");
//    Serial.println(IMU.readRawGyroX());
//    Serial.print(" Y = ");
//    Serial.println(IMU.readRawGyroY());
//    Serial.print(" Z = ");
//    Serial.println(IMU.readRawGyroZ());
}

void IMU_PrintSerialReg()
{
	uint32_t data;
}
