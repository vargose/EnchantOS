#ifndef SOURCE_IMU_H_
#define SOURCE_IMU_H_

#include "src/SparkFunLSM6DS3.h"

extern LSM6DS3 IMU;

void IMU_Poll(uint32_t currentTime);

void IMU_SetThresholdImpact(float g);
void IMU_SetThresholdSwing(float g, float d);

bool IMU_DetectImpact(uint32_t currentTime, uint32_t supressTime);
bool IMU_DetectSwing(uint32_t currentTime, uint32_t supressTime);
bool IMU_DetectSwingDirectionChange(void);

void IMU_Init();

void IMU_PrintSerial();
void IMU_PrintSerialReg();
#endif /* SOURCE_IMU_H_ */
