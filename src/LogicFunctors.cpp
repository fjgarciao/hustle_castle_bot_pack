#include "LogicFunctors.h"



void LogicFunctors::SetValue(const std::string &name, int value)
{
	mValues[name] = value;
}

int LogicFunctors::GetValue(const std::string &name)
{
	return mValues[name];
}

void LogicFunctors::StartTimer(const std::string &name)
{
	mTimers[name].Start();
}

int LogicFunctors::GetTimer(const std::string &name)
{
	return mTimers[name].GetTime();
}

void LogicFunctors::StartCounter(const std::string &name)
{
	mCounters[name].Start();
}

int LogicFunctors::GetCounter(const std::string &name)
{
	return mCounters[name].GetCount();
}

void LogicFunctors::IncCounter(const std::string &name)
{
	mCounters[name].Increment();
}

