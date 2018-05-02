/*
 * IMU.h
 *
 *  Created on: Jul 25, 2017
 *      Author: SLi
 */

#ifndef SOURCE_IMU_H_
#define SOURCE_IMU_H_

#include <stdint.h>

void IMU_Poll ();


void IMU_SetThresholdImpact(float g);
void IMU_SetThresholdSwing(float g, float d);

bool IMU_DetectImpact(void);
bool IMU_DetectSwing(void);
bool IMU_DetectMovement();

void IMU_SetLockOutDetectTimeSwing(uint16_t ms);
void IMU_SetLockOutDetectTimeImpact(uint16_t ms);

void IMU_Init();

void IMU_PrintSerial();
void IMU_PrintSerialReg();
#endif /* SOURCE_IMU_H_ */
