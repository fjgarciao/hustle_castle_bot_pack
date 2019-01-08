#include "Timer.h"

#include <cstdio>
#include <ctime>

using Clock = std::chrono::high_resolution_clock;

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	mStartTime = Clock::now();

}

int Timer::GetTime()
{
	return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - mStartTime).count());
}
