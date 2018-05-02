#include <Wire.h>

#include "src/SparkFunLSM6DS3.h"

LSM6DS3 Sensor( I2C_MODE, 0x6A );

static float AX, AY, AZ;
static float GX, GY, GZ;

// for cases where I2C bus gets stuck
static uint32_t LockOutPolling;
static uint16_t LockOutPollingTime = 10;

static float ThresholdImpact = 7;
static float ThresholdSwingGyro = 600;

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

static uint32_t LockOutDetectSwing;
static uint32_t LockOutDetectImpact;
static uint16_t LockOutDetectTimeSwing = 500;
static uint16_t LockOutDetectTimeImpact = 500;

static uint32_t TimerCounter;

void IMU_SetLockOutDetectTimeSwing(uint16_t ms)
{
	LockOutDetectTimeSwing = ms;
}

void IMU_SetLockOutDetectTimeImpact(uint16_t ms)
{
	LockOutDetectTimeImpact = ms;
}

void IMU_Poll ()
{
	uint8_t status;

	TimerCounter = millis();

	Sensor.readRegister(&status, LSM6DS3_ACC_GYRO_STATUS_REG);

	if( (status & LSM6DS3_ACC_GYRO_XLDA_DATA_AVAIL) == LSM6DS3_ACC_GYRO_XLDA_DATA_AVAIL)
	{
		AX = Sensor.readFloatAccelX();
		AY = Sensor.readFloatAccelY();
		AZ = Sensor.readFloatAccelZ();
	}

	if( (status & LSM6DS3_ACC_GYRO_GDA_DATA_AVAIL) == LSM6DS3_ACC_GYRO_GDA_DATA_AVAIL)
	{
		GX = Sensor.readFloatGyroX();
		GY = Sensor.readFloatGyroY();
		GZ = Sensor.readFloatGyroZ();
	}
}

inline static bool DetectImpactFunction(void)
{
	if (abs(AX) + abs(AZ) + abs(AZ) > ThresholdImpact)
		return 1;
	else
		return 0;
}

inline static bool DetectSwingFunction(void)
{
	if ( (abs(GX) > ThresholdSwingGyro || abs(GY)  > ThresholdSwingGyro || abs(GZ)  > ThresholdSwingGyro) )
		return 1;
	else
		return 0;
}

bool IMU_DetectImpact(void)
{
	if(TimerCounter < LockOutDetectImpact)
		return 0;

	if (DetectImpactFunction())
	{
		LockOutDetectSwing	 = TimerCounter + LockOutDetectTimeSwing;
		LockOutDetectImpact	 = TimerCounter + LockOutDetectTimeImpact;
		return 1;
	}
	else
		return 0;
}

bool IMU_DetectSwing(void)
{
	if(TimerCounter < LockOutDetectSwing)
		return 0;

	if (DetectSwingFunction())
	{
		LockOutDetectSwing	 = TimerCounter + LockOutDetectTimeSwing;
		return 1;
	}
	else
		return 0;
}

bool IMU_DetectMovement(void)
{
	if (
		(abs(AX) + abs(AY) + abs(AZ) > 1.25) ||
		(abs(AX) + abs(AY) + abs(AZ) < 1) ||
		(abs(GX) > 100 || abs(GY)  > 100 || abs(GZ)  > 100)
		)
		return 1;
	else
		return 0;
}

static bool write_reg(uint8_t i2c, uint8_t addr, uint8_t val)
{
	Wire.beginTransmission(i2c);
	Wire.write(addr);
	Wire.write(val);
	return Wire.endTransmission() == 0;
}

void IMU_Init()
{
	uint8_t dataToWrite = 0;  //Temporary variable

	Wire.setSCL(16);
	Wire.setSDA(17);
	Sensor.begin();

	dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
	dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_8g;
	dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;

	Sensor.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
}

void IMU_PrintSerial()
{

    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(Sensor.readFloatAccelX(), 4);
    Serial.print(" Y = ");
    Serial.println(Sensor.readFloatAccelY(), 4);
    Serial.print(" Z = ");
    Serial.println(Sensor.readFloatAccelZ(), 4);

    Serial.print("\nGyroscope:\n");
    Serial.print(" X = ");
    Serial.println(Sensor.readFloatGyroX(), 4);
    Serial.print(" Y = ");
    Serial.println(Sensor.readFloatGyroY(), 4);
    Serial.print(" Z = ");
    Serial.println(Sensor.readFloatGyroZ(), 4);

    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(Sensor.readRawAccelX());
    Serial.print(" Y = ");
    Serial.println(Sensor.readRawAccelY());
    Serial.print(" Z = ");
    Serial.println(Sensor.readRawAccelZ());

    Serial.print("\nGyroscope:\n");
    Serial.print(" X = ");
    Serial.println(Sensor.readRawGyroX());
    Serial.print(" Y = ");
    Serial.println(Sensor.readRawGyroY());
    Serial.print(" Z = ");
    Serial.println(Sensor.readRawGyroZ());
}

void IMU_PrintSerialReg()
{
	uint32_t data;
}
