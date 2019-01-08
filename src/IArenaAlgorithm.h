#pragma once

#include "pch.h"
#include "Applet.h"


struct ArenaPowerInfo
{
	int pos = -1;
	bool is_me = true;
	int my_power = 0;
	int other_power = 0;
	bool is_available = false;
};

using PowerData = std::array<ArenaPowerInfo, 15>;


class IArenaAlgorithm
{
public:
	virtual ~IArenaAlgorithm() = default;

	virtual int FindOpponent(const PowerData &data, int phase) = 0;

};

