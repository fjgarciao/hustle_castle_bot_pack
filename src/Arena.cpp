#include "Arena.h"
#include "ArenaAlgorithm1.h"
#include "ArenaAlgorithm2.h"



void Arena::GetOpponentInfo(int val)
{
	auto &out = mPowerData[val];
	out = {};
	out.pos = val;

	if (mApplet.StateName() == "allowed" || mApplet.StateName() == "forbidden")
	{
		out.is_me = false;
		out.is_available = mApplet.StateName() == "allowed";

		{
			cv::Mat img;
			mApplet.ReadRegion("my_power", img);
			out.my_power = mApplet.Recognize(img);
		}

		{
			cv::Mat img;
			mApplet.ReadRegion("other_power", img);
			out.other_power = mApplet.Recognize(img);
		}
	}
}

Arena::Arena(Applet &app)
	: mApplet(app)
{
	mArenaLogFile.open("stats.txt", std::ios_base::app);
	mAlgorithm = std::make_unique<ArenaAlgorithm2>();
}

void Arena::SetAlgorithm(int val)
{
	if (val == 0)
		mAlgorithm = std::make_unique<ArenaAlgorithm1>();
	else 	if (val == 1)
		mAlgorithm = std::make_unique<ArenaAlgorithm2>();
	else
		mAlgorithm = std::make_unique<ArenaAlgorithm1>();
}

bool Arena::IsValidPhase()
{
	bool valid = true;

	int my_power = 0;
	for (auto &info : mPowerData)
	{
		if (!info.is_me)
		{
			my_power = info.my_power;
			break;
		}
	}

	int me_count = 0;
	for (auto &info : mPowerData)
	{
		if (info.pos == -1)
		{
			valid = false;
			break;
		}
		if (!info.is_me && info.my_power != my_power)
		{
			valid = false;
			break;
		}
		me_count += info.is_me;
	}
	if (me_count != 1)
		valid = false;

	auto sorted = mPowerData;
	std::sort(sorted.begin(), sorted.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
	{
		return a.other_power < b.other_power;
	});

	for (int i = 0; i < sorted.size() - 1; ++i)
	{
		if (sorted[i].other_power == sorted[i + 1].other_power)
		{
			valid = false;
			break;
		}
	}

	int count = 0;
	for (auto &info : sorted)
		if (!info.is_me)
			count += !info.is_available;
	if (count >= 5)
		valid = false;

	return valid;
}

bool Arena::IsValidOpponent(int val)
{
	if (mApplet.StateName() == "allowed")
	{
		{
			cv::Mat img;
			mApplet.ReadRegion("other_power", img);
			if (mPowerData[val].other_power == mApplet.Recognize(img))
				return true;
		}
	}
	return false;
}



int Arena::FindOpponent()
{
	return mAlgorithm->FindOpponent(GetScaledData(), GetPhase(mPowerData));
}


void Arena::SetMyPowerScale(double val)
{
	mMyPowerScale = val;
}

PowerData Arena::GetScaledData()
{
	PowerData out = mPowerData;

	for (auto &info : out)
		info.my_power = decltype(info.my_power)(double(info.my_power) * mMyPowerScale);

	return out;
}


int Arena::GetPhase(const PowerData &data)
{
	int phase = 0;
	for (auto &info : data)
		phase += info.is_available;
	phase = 15 - phase;
	return phase;
}

int Arena::FindMe(const PowerData &data)
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

void Arena::WriteLogPhase(bool result)
{
	auto scaled_power_data = GetScaledData();

	auto phase = GetPhase(scaled_power_data);
	auto other_pos = FindOpponent();

	if (phase == 1)
	{
		mArenaLog = {};

		mArenaLog.my_power = mPowerData[other_pos].my_power;
		mArenaLog.scaled_power = scaled_power_data[other_pos].my_power;
		auto powers = scaled_power_data;
		std::sort(powers.begin(), powers.end(), [](const ArenaPowerInfo &a, const ArenaPowerInfo &b)
		{
			return a.other_power > b.other_power;
		});
		for (size_t i = 0; i < mArenaLog.other_power.size(); ++i)
			mArenaLog.other_power[i] = powers[i].other_power;


	}

	mArenaLog.phase[phase - 1].my_pos = FindMe(scaled_power_data) + 1;
	mArenaLog.phase[phase - 1].other_pos = other_pos + 1;
	mArenaLog.phase[phase - 1].other_power = scaled_power_data[other_pos].other_power;
	mArenaLog.phase[phase - 1].result = result;

	WriteLogArena(scaled_power_data, phase);
}

void Arena::WriteLogArena(const PowerData &data, int phase)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	if (mArenaLogFile.is_open())
	{
		if (phase == 1)
		{
			mArenaLogFile << std::endl;
			mArenaLogFile << std::put_time(&tm, "%H:%M:%S  ");

			mArenaLogFile << "my_power: " << mArenaLog.my_power << "   scaled_power: " << mArenaLog.scaled_power << std::endl;

			mArenaLogFile << "powers: ";
			for (auto power : mArenaLog.other_power)
				mArenaLogFile << power << " ";
			mArenaLogFile << std::endl;
		}
		
		const auto &info = mArenaLog.phase[phase - 1];

		mArenaLogFile << std::setw(12) << std::left << "phase " + std::to_string(phase);
		mArenaLogFile << std::setw(12) << std::left << "my_pos " + std::to_string(info.my_pos);
		mArenaLogFile << std::setw(18) << std::left << "other_pos " + std::to_string(info.other_pos);
		mArenaLogFile << std::setw(20) << std::left << "power " + std::to_string(info.other_power);
		mArenaLogFile << (info.result ? "win    | " : "lost   | ");

		mArenaLogFile << "powers: ";
		for (const auto &info : data)
			mArenaLogFile << info.other_power << " ";
		mArenaLogFile << std::endl;

		mArenaLogFile.flush();
	}
}

void Arena::WriteLogResult()
{
	auto scaled_power_data = GetScaledData();

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	if (mArenaLogFile.is_open())
	{
		mArenaLogFile << std::put_time(&tm, "%H:%M:%S  ") << "my_pos: " << std::setw(53) << std::left << FindMe(scaled_power_data) + 1;

		
		mArenaLogFile << "powers: ";
		for (const auto &info : scaled_power_data)
			mArenaLogFile << info.other_power << " ";
		mArenaLogFile << std::endl;

		mArenaLogFile.flush();
	}
}


