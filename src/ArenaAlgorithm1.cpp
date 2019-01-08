#include "ArenaAlgorithm1.h"


int ArenaAlgorithm1::FindOpponent(const PowerData &data, int phase)
{
	auto scaled_power_data = data;

	bool reserve_choise = false;

	{
		// 1 фаза. бьем одного из слабых противников, в зависимости от мощности всех слабых противников.
		if (phase == 1)
		{
			// берем всех слабых противников.
			// ищем позицию самого слабого и среднюю мощность всех слабых.
			auto opps = GetWeak(scaled_power_data);
			IgnoreMeanPower(scaled_power_data, opps, 3);
			auto pos = FindAdaptivePos(opps, reserve_choise);
			if (!reserve_choise)
				return pos;
		}
	}

	{
		// 4 фаза. если выше есть несколько слабых противников, которые могут убить вверх - бьем самого слабого.
		// если выше 1 слабый противник, который может убить вверх, бьем самого слабого противника сверху, кроме этого.
		if (phase == 4)
		{
			auto list = GetAboveWeakCan(scaled_power_data);
			if (list.size() >= 2)
			{
				auto opps = GetAboveWeak(scaled_power_data);
				if (!opps.empty())
					return opps[opps.size() - 1].pos;
				else
					reserve_choise = true;
			}
			else if (list.size() == 1)
			{
				auto opps = GetAboveWeak(scaled_power_data);
				for (auto it = opps.begin(); it != opps.end(); ++it)
					if (it->pos == list[0].pos)
					{
						opps.erase(it);
						break;
					}

				if (!opps.empty())
					return opps[0].pos;
				else
					reserve_choise = true;
			}
			else
				reserve_choise = true;
		}
	}

	{
		// 2 или 3 фаза. бьем тех, которые могут убить не больше 2 или 1 выше себя.
		if (phase == 2 || phase == 3)
		{
			auto opps = GetBelowWeakCannot(scaled_power_data, phase == 2 ? 2 : 1);
			IgnoreMeanPower(scaled_power_data, opps, phase == 2 ? 2 : 1);
			if (!opps.empty())
				return opps[opps.size() - 1].pos;
			else
				reserve_choise = true;
		}
	}

	{
		// 5 фаза. бьем самого слабого выше меня.
		if (phase == 5)
		{
			auto opps = GetAboveWeak(scaled_power_data);
			if (!opps.empty())
				return opps[0].pos;
			else
				reserve_choise = true;
		}
	}

	if (reserve_choise)
	{
		{
			auto opps = GetBelowWeak(scaled_power_data);
			if (!opps.empty())
				return opps[0].pos;
		}

		{
			auto opps = GetWeak(scaled_power_data);
			if (!opps.empty())
				return opps[0].pos;
		}
	}

	auto opps = GetAll(scaled_power_data);
	return opps[0].pos;
}





int ArenaAlgorithm1::FindAdaptivePos(const std::vector<ArenaPowerInfo> &opps, bool &reserve_choise)
{
	reserve_choise = false;
	auto weak_mean_power = GetWeakMeanPower(opps);
	if (opps.empty() || !weak_mean_power)
	{
		reserve_choise = true;
		return -1;
	}
	auto weak_min_power = opps[0].other_power;
	auto weak_max_power = opps[opps.size() - 1].other_power;

	auto max = weak_max_power - weak_min_power;		// max - 1
	auto mean = weak_mean_power - weak_min_power; // mean - ?

	int pos = 0;
	if (max)
	{
		auto scale = double(mean) / double(max);
		pos = std::clamp(int(std::floor(scale * double(opps.size() - 1))), 0, int(opps.size() - 1));
	}
	return opps[pos].pos;
}

void ArenaAlgorithm1::IgnoreMeanPower(const PowerData &data, std::vector<ArenaPowerInfo> &power, int count)
{
	auto all = GetAll(data);

	int rem_count = 0;
	for (auto i = all.size() - 1; rem_count < count; --i)
	{
		if (all[i].other_power < all[i].my_power)
		{
			++rem_count;
			
			auto p = all[i].other_power;
			auto it = std::find_if(power.begin(), power.end(), [p](const ArenaPowerInfo &info)
			{
				return info.other_power == p;
			});
			if (it != power.end())
				power.erase(it);
		}

		if (i == 0)
			break;
	}
}

//============================

int ArenaAlgorithm1::FindMe(const PowerData &data)
{
	int me = -1;
	for (auto &info : data)
		if (info.is_me)
		{
			me = info.pos;
			break;
		}

	return me;
}

int ArenaAlgorithm1::GetWeakMeanPower(const std::vector<ArenaPowerInfo> &opps)
{
	if (opps.empty())
		return 0;

	int64 mean = 0;

	for (auto &info : opps)
		mean += info.other_power;

	return int(mean / opps.size());
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetAll(const PowerData &data)
{
	std::vector<ArenaPowerInfo> out;

	for (auto &info : data)
		if (!info.is_me && info.is_available)
			out.push_back(info);

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetBelowWeakCannot(const PowerData &data, int count)
{
	std::vector<ArenaPowerInfo> out;

	bool me_finded = false;
	for (auto &info : data)
	{
		if (info.is_me)
		{
			me_finded = true;
			continue;
		}
		if (!me_finded)
			continue;

		if (info.is_available && info.my_power > info.other_power && CountWeatAbove(data, info.pos) <= count)
			out.push_back(info);
	}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetAboveWeakCannot(const PowerData &data, int count)
{
	std::vector<ArenaPowerInfo> out;

	for (auto &info : data)
	{
		if (info.is_me)
			break;

		if (info.is_available && info.my_power > info.other_power && CountWeatAbove(data, info.pos) <= count)
			out.push_back(info);
	}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetAboveWeakCan(const PowerData &data)
{
	std::vector<ArenaPowerInfo> out;

	for (auto &info : data)
	{
		if (info.is_me)
			break;

		if (info.is_available && info.my_power > info.other_power && CountWeatAbove(data, info.pos) >= 1)
			out.push_back(info);
	}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetBelowWeak(const PowerData &data)
{
	std::vector<ArenaPowerInfo> out;

	bool me_finded = false;
	for (auto &info : data)
	{
		if (info.is_me)
		{
			me_finded = true;
			continue;
		}
		if (!me_finded)
			continue;

		if (info.is_available && info.my_power > info.other_power)
			out.push_back(info);
	}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetAboveWeak(const PowerData &data)
{
	std::vector<ArenaPowerInfo> out;

	for (auto &info : data)
	{
		if (info.is_me)
			break;

		if (info.is_available && info.my_power > info.other_power)
			out.push_back(info);
	}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm1::GetWeak(const PowerData &data)
{
	std::vector<ArenaPowerInfo> out;

	for (auto &info : data)
		if (!info.is_me && info.is_available && info.my_power > info.other_power)
			out.push_back(info);

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

int ArenaAlgorithm1::CountWeatAbove(const PowerData &data, int pos)
{
	int out = 0;
	for (auto &info : data)
	{
		if (info.pos == pos)
			break;

		if (data[pos].other_power > info.other_power)
			++out;
	}
	return out;
}

