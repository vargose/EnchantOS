/**************************************************************************/
/*!
    @file     LiteFXOS.h
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
#ifndef SOURCE_LITEFXOS_H_
#define SOURCE_LITEFXOS_H_

typedef struct LightFXOSThread
{
	void (*Function)(void); 		// Current FX to run
	void (*OnComplete)(void);

	//uint32_t 	TicksPerSecond;		// User perspective input
	uint32_t	UpdateInterval;		// number of timer ticks (milliseconds) between updates
	uint32_t 	LastUpdate; 		// last update time

	//uint32_t 	TimeRemaining;		// User perspective input
	uint32_t 	TicksRemaining;

	bool Momentary;					// one-time or repeats
	bool Run;						// Enable or disable

	void * Context;					// option to load context before running
	void (* LoadContext)(void*);
//	void (* SaveContext)(void*);
//	uint32_t 	StartDelay;
} LITE_FX_OS_THREAD_T;

void LiteFXOS_SetTimerCounter(uint32_t ms);
bool LiteFXOS_ProcThread(LITE_FX_OS_THREAD_T * thread);
void LiteFXOS_ProcThreadNRepeat(LITE_FX_OS_THREAD_T * thread, uint32_t n);

void LiteFXOS_StartThreadCycleArgFreq(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ticksPerSecond);
void LiteFXOS_StartThreadMomentaryArgTicksFreq(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void));
void LiteFXOS_StartThreadMomentaryArgTicksTime(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ticks, uint32_t ms, void(*onComplete)(void));
void LiteFXOS_StartThreadMomentaryArgTimeFreq(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ms, uint32_t ticksPerSecond, void(*onComplete)(void));

void LiteFXOS_SetThreadCycle(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t interval);
void LiteFXOS_SetThreadCycleArgFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticksPerSecond);
void LiteFXOS_SetThreadMomentary(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void));
void LiteFXOS_SetThreadMomentaryArgTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void));
void LiteFXOS_SetThreadMomentaryArgTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ms, void(*onComplete)(void));
void LiteFXOS_SetThreadMomentaryArgTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ms, uint32_t ticksPerSecond, void(*onComplete)(void));

void LiteFXOS_SetThreadContext(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*));
void LiteFXOS_SetThreadStart(LITE_FX_OS_THREAD_T * thread);
void LiteFXOS_SetThreadStop(LITE_FX_OS_THREAD_T * thread);

void LiteFXOS_SetThreadUpdateInterval(LITE_FX_OS_THREAD_T * thread, uint16_t ms);
void LiteFXOS_SetThreadFreq(LITE_FX_OS_THREAD_T * thread, uint16_t ticksPerSecond);

void LiteFXOS_SetThreadFaster(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc);
void LiteFXOS_SetThreadSlower(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec);
void LiteFXOS_SetThreadFasterArgBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior);
void LiteFXOS_SetThreadSlowerArgBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior);

#endif /* SOURCE_LITEFXOS_H_ */
