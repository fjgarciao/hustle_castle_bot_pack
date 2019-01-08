#include "WindowCapture.h"
#include "Page.h"
#include "Logic.h"
#include "Common.h"

#include <rapidjson/document.h>

int main()
{
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 1200, 600, TRUE);

	std::string json_data;
	{
		std::ifstream file;
		file.open("config.json", std::ifstream::ate);
		auto size = file.tellg();
		json_data.resize(size);

		file.seekg(0, std::ios::beg);
		file.read(json_data.data(), size);

		file.close();
	}

	int capute_mode = 0;
	int emulator = 0;
	auto PAUSE_TIME = 200;
	int state_timeout = 1000 * 60;
	int opponent_timeout = 500;
	int program = 0;

	rapidjson::Document document;
	document.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag>(json_data.data());

	if (!document.IsObject())
	{
		std::cout << "error read config" << std::endl;
		int tcint;
		std::cin >> tcint;
		return 0;
	}

	if (document.HasMember("emulator") && document["emulator"].IsInt())
		emulator = document["emulator"].GetInt();

	if (document.HasMember("capute_mode") && document["capute_mode"].IsInt())
		capute_mode = document["capute_mode"].GetInt();

	if (document.HasMember("pause") && document["pause"].IsInt())
		PAUSE_TIME = document["pause"].GetInt();

	if (document.HasMember("state_timeout") && document["state_timeout"].IsInt())
		state_timeout = document["state_timeout"].GetInt();

	if (document.HasMember("opponent_timeout") && document["opponent_timeout"].IsInt())
		opponent_timeout = document["opponent_timeout"].GetInt();

	if (document.HasMember("program") && document["program"].IsInt())
		program = document["program"].GetInt();

	{
		Applet app;
		Logic logic(app);
		int stir_buy = 0;
		int arena_ticket = 0;
		int stir_update_soon = 0;
		int check_mail = 1;
		int arena_stats = 0;
		double my_power_scale = 1.0;
		int algorithm = 0;
		int portal_level = 0;
		
		if (document.HasMember("stir") && document["stir"].IsObject())
		{
			auto &value = document["stir"];

			if (value.HasMember("stir_buy") && value["stir_buy"].IsInt())
				stir_buy = value["stir_buy"].GetInt();
			if (value.HasMember("stir_update_soon") && value["stir_update_soon"].IsInt())
				stir_update_soon = value["stir_update_soon"].GetInt();
		}

		if (document.HasMember("arena") && document["arena"].IsObject())
		{
			auto &value = document["arena"];

			if (value.HasMember("arena_ticket") && value["arena_ticket"].IsInt())
				arena_ticket = value["arena_ticket"].GetInt();

			if (value.HasMember("check_mail") && value["check_mail"].IsInt())
				check_mail = value["check_mail"].GetInt();

			if (value.HasMember("arena_stats") && value["arena_stats"].IsInt())
				arena_stats = value["arena_stats"].GetInt();

			if (value.HasMember("my_power_scale") && value["my_power_scale"].IsDouble())
				my_power_scale = value["my_power_scale"].GetDouble();

			if (value.HasMember("algorithm") && value["algorithm"].IsInt())
				algorithm = value["algorithm"].GetInt();
		}

		if (document.HasMember("portal") && document["portal"].IsObject())
		{
			auto &value = document["portal"];

			if (value.HasMember("portal_level") && value["portal_level"].IsInt())
				portal_level = value["portal_level"].GetInt();
		}

		app.SetEmulator(emulator);
		app.SetCaptureMode(capute_mode);

		logic.SetMyPowerScale(my_power_scale);
		logic.SetAlgorithm(algorithm);
		logic.SetValue("state_timeout", state_timeout);
		logic.SetValue("opponent_timeout", opponent_timeout);
		logic.SetValue("program", program);
		logic.SetValue("stir_buy", stir_buy);
		logic.SetValue("stir_update_soon", stir_update_soon);
		logic.SetValue("arena_ticket", arena_ticket);
		logic.SetValue("check_mail", check_mail);
		logic.SetValue("arena_stats", arena_stats);
		logic.SetValue("portal_level", portal_level);
		
		while (logic.IsRunned())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_TIME));
			logic.Scan();
		}
	}

	std::cout << "end" << std::endl;
	int tcint;
	std::cin >> tcint;
  return 0;
}
