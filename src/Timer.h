#pragma once

#include "pch.h"


class Timer
{
public:
	Timer();

	void Start();

	int GetTime();

private:
	std::chrono::time_point<std::chrono::steady_clock> mStartTime;
};

