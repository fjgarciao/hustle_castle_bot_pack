#pragma once

#include "pch.h"

#include "Timer.h"
#include "Counter.h"


class LogicFunctors
{
public:

	void SetValue(const std::string &name, int value);
	int GetValue(const std::string &name);

	void StartTimer(const std::string &name);
	int GetTimer(const std::string &name);

	void StartCounter(const std::string &name);
	int GetCounter(const std::string &name);
	void IncCounter(const std::string &name);

private:
	std::map<std::string, Timer> mTimers;
	std::map<std::string, Counter> mCounters;
	std::map<std::string, int> mValues;
};

