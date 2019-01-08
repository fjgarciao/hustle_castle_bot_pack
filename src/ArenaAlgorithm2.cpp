#include "ArenaAlgorithm2.h"


int ArenaAlgorithm2::FindOpponent(const PowerData &data, int phase)
{
	auto scaled_power_data = data;

	bool reserve_choise = false;

	{
		if (phase == 1)
		{
			auto opps = GetAll(scaled_power_data);
			auto weak = GetWeak(scaled_power_data);
			if (!weak.empty())
				for (auto it = opps.begin(); it != opps.end(); ++it)
					if (it->pos == weak[0].pos)
					{
						opps.erase(it);
						break;
					}
			if (!opps.empty())
				return opps[0].pos;
			else
				reserve_choise = true;
		}

		if (phase == 2)
		{
			reserve_choise = true;
		}
	}

	{
		if (phase == 3)
		{
			auto opps = GetPhase3(scaled_power_data);
			if (!opps.empty())
				return opps[opps.size() - 1].pos;
			else
				reserve_choise = true;
		}
	}

	{
		// 4 фаза. если выше есть несколько слабых противников, которые могут убить вверх - бьем самого слабого.
		// если выше 1 слабый противник, который может убить вверх, бьем самого слабого противника сверху, кроме этого.
		if (phase == 4)
		{
			{
				auto list = GetAboveWeakCan(scaled_power_data);
				if (list.size() >= 2)
				{
					auto opps = GetAboveWeak(scaled_power_data);
					if (!opps.empty())
						return opps[0].pos;
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
				}
			}

			{
				auto opps = GetAboveWeakCannot(scaled_power_data);
				for (auto it = opps.begin(); it != opps.end(); ++it)
					if (it->pos == 0)
					{
						opps.erase(it);
						break;
					}
				if (!opps.empty())
					return opps[0].pos;
			}


			if (scaled_power_data[0].is_available && !scaled_power_data[0].is_me && scaled_power_data[0].my_power > scaled_power_data[0].other_power)
				return scaled_power_data[0].pos;

			auto opps = GetAboveWeak(scaled_power_data);
			if (!opps.empty())
				return opps[0].pos;

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
			if (phase == 1 || phase == 2 || phase == 3)
			{
				auto weak = GetWeak(scaled_power_data);
				if (!weak.empty())
					for (auto it = opps.begin(); it != opps.end(); ++it)
						if (it->pos == weak[0].pos)
						{
							opps.erase(it);
							break;
						}
			}
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

int ArenaAlgorithm2::FindMe(const PowerData &data)
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

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetAboveWeakCan(const PowerData &data)
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

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetAboveWeakCannot(const PowerData &data)
{
	std::vector<ArenaPowerInfo> out;

	for (auto &info : data)
	{
		if (info.is_me)
			break;

		if (info.is_available && info.my_power > info.other_power && CountWeatAbove(data, info.pos) == 0)
			out.push_back(info);
	}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetAboveWeak(const PowerData &data)
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

int ArenaAlgorithm2::CountWeatAbove(const PowerData &data, int pos)
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

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetBelowWeak(const PowerData &data)
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

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetPhase3(PowerData data)
{
	auto pdata = data;

	std::vector<ArenaPowerInfo> out;

	auto me_pos = FindMe(data);
	data[me_pos].other_power = data[me_pos].my_power;

	// для противников ниже меня
	for (auto &info : data)
		if (info.pos > me_pos && info.is_available && info.my_power > info.other_power)
		{
			int count = 0; // противник ниже меня и может убить верх столько.
			for (auto &i : data)
			{
				if (info.other_power > i.other_power)
					++count;
				if (i.pos == info.pos)
					break;
			}
			if (count < 2)
				out.push_back(info);
		}

	std::sort(out.begin(), out.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	// исключаемсамого ближнего ниже меня.
	if (me_pos + 1 < pdata.size())
		for (auto it = out.begin(); it != out.end(); ++it)
			if (it->pos == me_pos + 1)
			{
				out.erase(it);
				break;
			}

	return out;
}

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetAll(const PowerData &data)
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

std::vector<ArenaPowerInfo> ArenaAlgorithm2::GetWeak(const PowerData &data)
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

