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

    @brief Simple thread manager.
*/
/**************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "OS.h"

#define TIMER_RATE 			1000	//1000 ticks per second using milis
#define UPDATE_INTERVAL_MIN 1		//1ms min in between ticks
#define UPDATE_INTERVAL_MAX 1000	//1s max in between ticks

static volatile uint32_t 	TimerCounter;
/**************************************************************************/
/*!
    @brief	Public function updating Timer shared between threads.
    		Run this function inside main loop.

    @param	ms
    		Time in milliseconds.

    @return void
*/
/**************************************************************************/
void LiteFXOS_SetTimerCounter(uint32_t ms) {TimerCounter = ms;}

inline static void ProcThread(LITE_FX_OS_THREAD_T * thread)
{
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
    @brief	Public function for processing threads.
    		Run this function inside main loop.

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

	if(thread->Context && thread->LoadContext) thread->LoadContext(thread->Context);

	thread->LastUpdate = TimerCounter;
	ProcThread(thread);
	return 1;
}

void LiteFXOS_ProcThreadNRepeat(LITE_FX_OS_THREAD_T * thread, uint32_t n)
{
	if(thread->Context && thread->LoadContext) thread->LoadContext(thread->Context);

	for(uint32_t t = 0; t < n; t++)
		ProcThread(thread);
}

/*-----------------------------------------------------------------------------
  Thread Start functions - assigns all parameters necessary to run
 *----------------------------------------------------------------------------*/
void LiteFXOS_StartThreadCycleArgFreq(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ticksPerSecond)
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick
	thread->Momentary		= false;
	thread->Run				= true;

	thread->Context = memory;
	thread->LoadContext = loadMemory;
}

// e.g. wipe 2 leds at the rate of 5 fps
void LiteFXOS_StartThreadMomentaryArgTicksFreq(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
	thread->Run				= true;

	thread->Context = memory;
	thread->LoadContext = loadMemory;
}

// e.g. rainbow 1000ms at 60fps
void LiteFXOS_StartThreadMomentaryArgTimeFreq(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ms, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ms / thread->UpdateInterval; //should round up or down?
	thread->OnComplete 		= onComplete;
	thread->Run				= true;

	thread->Context = memory;
	thread->LoadContext = loadMemory;
}

// e.g. fader cycle of 100 ticks in 1200ms
void LiteFXOS_StartThreadMomentaryArgTicksTime(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*), void (*function)(void), uint32_t ticks, uint32_t ms, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= ms / ticks;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
	thread->Run				= true;

	thread->Context = memory;
	thread->LoadContext = loadMemory;
}

/*-----------------------------------------------------------------------------
  Thread Set functions - Configures thread
 *----------------------------------------------------------------------------*/
void LiteFXOS_SetThreadCycle(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t interval)
{
	thread->Function		= function;
	thread->UpdateInterval	= interval;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick
	thread->Momentary		= false;
}

void LiteFXOS_SetThreadCycleArgFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticksPerSecond)
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick
	thread->Momentary		= false;
}

void LiteFXOS_SetThreadMomentary(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= updateInterval;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
}

void LiteFXOS_SetThreadMomentaryArgTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
}

void LiteFXOS_SetThreadMomentaryArgTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ms, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= ms / ticks;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ticks;
	thread->OnComplete 		= onComplete;
}

void LiteFXOS_SetThreadMomentaryArgTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ms, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	thread->Function		= function;
	thread->UpdateInterval	= TIMER_RATE / ticksPerSecond;
	thread->LastUpdate		= TimerCounter - thread->UpdateInterval; //ensure immediate update on next tick

	thread->Momentary		= true;
	thread->TicksRemaining 	= ms / thread->UpdateInterval; //should round up or down?
	thread->OnComplete 		= onComplete;
}

void LiteFXOS_SetThreadContext(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*))
{
	thread->Context = memory;
	thread->LoadContext = loadMemory;
}

void LiteFXOS_SetThreadStart(LITE_FX_OS_THREAD_T * thread)
{
	thread->Run				= true;
}

void LiteFXOS_SetThreadStop(LITE_FX_OS_THREAD_T * thread)
{
	thread->Run				= false;
}

void LiteFXOS_SetThreadUpdateInterval(LITE_FX_OS_THREAD_T * thread, uint16_t ms)
{
	if (ms < UPDATE_INTERVAL_MIN)	thread->UpdateInterval = UPDATE_INTERVAL_MIN;
	if (ms > UPDATE_INTERVAL_MAX)	thread->UpdateInterval = UPDATE_INTERVAL_MAX;
	else							thread->UpdateInterval = ms;
}

void LiteFXOS_SetThreadFreq(LITE_FX_OS_THREAD_T * thread, uint16_t ticksPerSecond)
{
	if 		(ticksPerSecond > TIMER_RATE / UPDATE_INTERVAL_MIN)	thread->UpdateInterval = TIMER_RATE / UPDATE_INTERVAL_MIN;
	else if (ticksPerSecond < TIMER_RATE / UPDATE_INTERVAL_MAX)	thread->UpdateInterval = TIMER_RATE / UPDATE_INTERVAL_MAX;
	else														thread->UpdateInterval = TIMER_RATE / ticksPerSecond;
}

void LiteFXOS_SetThreadFaster(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc)
{
	uint32_t freqCurrent = TIMER_RATE / thread->UpdateInterval;
	uint32_t freqMax = TIMER_RATE / UPDATE_INTERVAL_MIN;

	if(freqCurrent + freqInc > freqMax)		freqCurrent = freqMax;
	else									freqCurrent += freqInc;

	thread->UpdateInterval = TIMER_RATE / freqCurrent;
}

void LiteFXOS_SetThreadSlower(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec)
{
	uint32_t freqCurrent = TIMER_RATE / thread->UpdateInterval;
	uint32_t freqMin = TIMER_RATE / UPDATE_INTERVAL_MAX;

	if(freqCurrent - freqDec < freqMin)		freqCurrent = freqMin;
	else									freqCurrent -= freqDec;

	thread->UpdateInterval = TIMER_RATE / freqCurrent;
}

void LiteFXOS_SetThreadFasterArgBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior)
{
	uint32_t freqCurrent = TIMER_RATE / thread->UpdateInterval;

	if(freqCurrent + freqInc > freqMax)
	{
		if (boundaryBehavior)
			freqCurrent = freqMax;
		else
			freqCurrent = freqMin;
	}
	else
	{
		freqCurrent += freqInc;
	}

	thread->UpdateInterval = TIMER_RATE / freqCurrent;
}

void LiteFXOS_SetThreadSlowerArgBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior)
{
	uint32_t freqCurrent = TIMER_RATE / thread->UpdateInterval;

	if(freqCurrent - freqDec < freqMin)
	{
		if (boundaryBehavior)
			freqCurrent = freqMin;
		else
			freqCurrent = freqMax;
	}
	else
	{
		freqCurrent -= freqDec;
	}

	thread->UpdateInterval = TIMER_RATE / freqCurrent;
}
