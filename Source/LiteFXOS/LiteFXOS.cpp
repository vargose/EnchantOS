/**************************************************************************/
/*!
    @file     LiteFXOS.cpp
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

    @brief Simple thread manager
*/
/**************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "LiteFXOS.h"

#define TIMER_RATE 			1000	//1000 ticks per second using milis
#define UPDATE_INTERVAL_MIN 1		//1ms min in between ticks
#define UPDATE_INTERVAL_MAX 1000	//1s max in between ticks

static volatile uint32_t 	TimerCounter;
void LiteFXOS_SetTimerCounter(uint32_t ms) {TimerCounter = ms;}

static void ProcThread(LITE_FX_OS_THREAD_T * thread)
{
	if(thread->Memory && thread->LoadMemory)
	{
		thread->LoadMemory(thread->Memory);
	}

	if (thread->Momentary)
	{
		if (thread->TicksRemaining) // last update on 1 ticks remaining
		{
			thread->TicksRemaining--;
			if (thread->Function) thread->Function();
		}
		else
		{
			thread->Run = false;
			if (thread->OnComplete) thread->OnComplete();
		}
	}
	else
	{
		if (thread->Function) thread->Function();
	}
}

/**************************************************************************/
/*!
    @brief	Public function for processing threads

    @param    thread
              pointer to the thread to be processed
    @return True if the function ran, otherwise false.
*/
/**************************************************************************/
bool LiteFXOS_ProcThread(LITE_FX_OS_THREAD_T * thread)
{
	if (!thread->Run)
		return 0;

	if(TimerCounter - thread->LastUpdate < thread->UpdateInterval)
		return 0;

	thread->LastUpdate = TimerCounter;
	ProcThread(thread);
	return 1;
}

void LiteFXOS_ProcThreadNRepeat(LITE_FX_OS_THREAD_T * thread, uint32_t n)
{
	for(uint32_t t = 0; t < n; t++)
		ProcThread(thread);
}

void LiteFXOS_SetThreadCycleArgFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticksPerSecond)
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick
	thread->Momentary		= false;
	thread->Run				= true;
}

void LiteFXOS_SetThreadCycleArgInterval(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t interval)
{
	thread->Function		= function;
	thread->UpdateInterval	= interval;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick
	thread->Momentary		= false;
	thread->Run				= true;
}

// e.g. wipe 2 leds in 1 second.
void LiteFXOS_SetThreadMomentaryArgTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ms, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= ms / ticks;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
	thread->Run				= true;
}

// e.g. wipe 2 leds at the rate of 5 fps
void LiteFXOS_SetThreadMomentaryArgTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
	thread->Run				= true;
}

// e.g. rainbow 1000ms at 60fps
void LiteFXOS_SetThreadMomentaryArgTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ms, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ms / thread->UpdateInterval; //should round up or down?
	thread->OnComplete 		= onComplete;
	thread->Run				= true;
}

void LiteFXOS_SetThreadMemory(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*))
{
	thread->Memory = memory;
	thread->LoadMemory = loadMemory;
}

void LiteFXOS_SetThreadUpdateInterval(LITE_FX_OS_THREAD_T * thread, uint8_t ms)
{
	if (ms < UPDATE_INTERVAL_MIN)	thread->UpdateInterval = UPDATE_INTERVAL_MIN;
	if (ms > UPDATE_INTERVAL_MAX)	thread->UpdateInterval = UPDATE_INTERVAL_MAX;
	else							thread->UpdateInterval = ms;
}

void LiteFXOS_SetThreadFreq(LITE_FX_OS_THREAD_T * thread, uint8_t ticksPerSecond)
{
	if 		(ticksPerSecond > TIMER_RATE / UPDATE_INTERVAL_MIN)				thread->UpdateInterval = TIMER_RATE / UPDATE_INTERVAL_MIN;
	else if (ticksPerSecond < UPDATE_INTERVAL_MAX / UPDATE_INTERVAL_MIN)	thread->UpdateInterval = UPDATE_INTERVAL_MAX / UPDATE_INTERVAL_MIN;
	else																	thread->UpdateInterval = TIMER_RATE / ticksPerSecond;
}

void LiteFXOS_SetThreadFaster(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc, uint8_t freqMax)
{
	uint32_t freqCurrent = TIMER_RATE / thread->UpdateInterval;

	if(freqCurrent + freqInc > freqMax)		freqCurrent = freqMax;
	else									freqCurrent += freqInc;

	thread->UpdateInterval = TIMER_RATE / freqCurrent;
}

void LiteFXOS_SetThreadSlower(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec, uint8_t freqMin)
{
	uint32_t freqCurrent = TIMER_RATE / thread->UpdateInterval;

	if(freqCurrent - freqDec < freqMin)		freqCurrent = freqMin;
	else									freqCurrent -= freqDec;

	thread->UpdateInterval = TIMER_RATE / freqCurrent;
}
