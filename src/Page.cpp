#include "Page.h"

#include "Common.h"



Page::Page(const std::string &name)
{
	mName = name;
}

bool Page::SetPage(const std::string &sample_file, const std::string &mask_file)
{
	auto sample = cv::imread(sample_file, cv::IMREAD_COLOR);
	auto mask = cv::imread(mask_file, cv::IMREAD_COLOR);

	if (sample.empty() || mask.empty())
		return false;

	cv::Rect rect;
	if (MaskToRect(mask, rect))
	{
		mDetectArea.rect = rect;
		mDetectArea.sample = sample(rect);
		return true;
	}

	return false;
}

bool Page::AddButton(std::string name, const std::string &mask_file)
{
	auto mask = cv::imread(mask_file, cv::IMREAD_COLOR);

	if (mask.empty())
		return false;

	cv::Rect rect;
	if (MaskToRect(mask, rect))
	{
		mButtons[name] = rect;
		return true;
	}

	return false;
}

bool Page::AddState(std::string name, const std::string &sample_file, const std::string &mask_file)
{
	auto sample = cv::imread(sample_file, cv::IMREAD_COLOR);
	auto mask = cv::imread(mask_file, cv::IMREAD_COLOR);

	if (sample.empty() || mask.empty())
		return false;

	cv::Rect rect;
	if (MaskToRect(mask, rect))
	{
		auto &state = mStates[name];
		state.rect = rect;
		state.sample = sample(rect);
		return true;
	}

	return false;
}

bool Page::AddRegion(std::string name, const std::string &mask_file)
{
	auto mask = cv::imread(mask_file, cv::IMREAD_COLOR);

	if (mask.empty())
		return false;

	cv::Rect rect;
	if (MaskToRect(mask, rect))
	{
		mRegions[name] = rect;
		return true;
	}

	return false;
}

bool Page::PressButton(WindowCapture &capture, std::string name)
{
	auto it = mButtons.find(name);
	if (it == mButtons.end())
		return false;
	return capture.Click(it->second);
}

bool Page::Move(WindowCapture &capture, std::string name, int w, int h)
{
	auto it = mRegions.find(name);
	if (it == mRegions.end())
		return false;
	return capture.Move(it->second, w, h);
}

bool Page::SetMasks(cv::Mat &src, bool only_detect)
{
	if (src.empty())
		return false;

	cv::rectangle(src, mDetectArea.rect, cv::Scalar(0, 0, 255));

	if (!only_detect)
		for (auto pair : mButtons)
			cv::rectangle(src, pair.second, cv::Scalar(0, 255, 0));

	if (!only_detect)
		for (auto pair : mStates)
			cv::rectangle(src, pair.second.rect, cv::Scalar(255, 0, 0));

	return true;
}

const std::string & Page::Name() const
{
	return mName;
}

bool Page::Detect(const cv::Mat &src)
{
	return Compare(src(mDetectArea.rect), mDetectArea.sample) < COMPARE_ERROR;
}

bool Page::Detect(const cv::Mat &src, double &err)
{
	err = Compare(src(mDetectArea.rect), mDetectArea.sample);
	return err < COMPARE_ERROR;
}

bool Page::ReadRegion(const cv::Mat &src, std::string name, cv::Mat &dst)
{
	auto it = mRegions.find(name);
	if (it == mRegions.end())
		return false;
	dst = src(it->second);
	return true;
}

bool Page::CheckState(const cv::Mat &src, std::string name)
{
	auto it = mStates.find(name);
	if (it == mStates.end())
		return false;
	auto &state = it->second;
	return Compare(src(state.rect), state.sample) < COMPARE_ERROR;
}

std::string Page::DetectState(const cv::Mat &src)
{
	for (auto &pair : mStates)
		if (Compare(src(pair.second.rect), pair.second.sample) < COMPARE_ERROR)
			return pair.first;
	return {};
}

std::string Page::DetectState(const cv::Mat &src, double &err)
{
	for (auto &pair : mStates)
	{
		err = Compare(src(pair.second.rect), pair.second.sample);
		if (err < COMPARE_ERROR)
			return pair.first;
	}
	return {};
}


decltype(auto) str_sample = "sample.png";
decltype(auto) str_detect = "detect.png";
decltype(auto) str_state = "state_";
decltype(auto) str_mask = "mask_";
decltype(auto) str_button = "button_";
decltype(auto) str_region = "region_";

Book::Book()
	: mUnknownPage("unknown")
{

}

bool Book::Load()
{
	auto result = true;
	
	for (auto& it : std::filesystem::directory_iterator("data2"))
		if (it.is_directory())
		{
			auto &path = it.path();

			if (path.filename().string() == "unknown")
			{
				result &= LoadPage(path, mUnknownPage);
			}
			else
			{
				auto &page = mPages.emplace_back(path.filename().string());
				result &= LoadPage(path, page);
			}
		}

	return result;
}

Page & Book::GetUnknownPage()
{
	return mUnknownPage;
}

Page &Book::Detect(const cv::Mat &src)
{
	for (auto &data : mPages)
		if (data.Detect(src))
			return data;
	return mUnknownPage;
}

Page & Book::Detect(const cv::Mat &src, double &err)
{
	for (auto &data : mPages)
		if (data.Detect(src, err))
			return data;
	return mUnknownPage;
}

bool Book::LoadPage(const std::filesystem::path &path, Page &page)
{
	bool result = true;
	if (page.Name() != "unknown")
	{
		auto sample_path = path;
		sample_path /= (str_sample);
		auto template_path = path;
		template_path /= str_detect;

		result &= page.SetPage(sample_path.string(), template_path.string());
	}

	for (auto& jt : std::filesystem::directory_iterator(path))
	{
		auto file_name = jt.path().stem().string();
		if (file_name.compare(0, sizeof(str_state) - 1, str_state) == 0)
		{
			auto name = jt.path().filename().string();
			name.erase(name.begin(), name.begin() + sizeof(str_state) - 1);
			file_name.erase(file_name.begin(), file_name.begin() + sizeof(str_state) - 1);

			auto state_path = path;
			state_path /= (str_state + name);
			auto mask_path = path;
			mask_path /= (str_mask + name);

			result &= page.AddState(file_name, state_path.string(), mask_path.string());
		}
	}

	for (auto& jt : std::filesystem::directory_iterator(path))
	{
		auto file_name = jt.path().stem().string();
		if (file_name.compare(0, sizeof(str_button) - 1, str_button) == 0)
		{
			auto name = jt.path().filename().string();
			name.erase(name.begin(), name.begin() + sizeof(str_button) - 1);
			file_name.erase(file_name.begin(), file_name.begin() + sizeof(str_button) - 1);

			auto button_path = path;
			button_path /= (str_button + name);

			result &= page.AddButton(file_name, button_path.string());
		}
	}

	for (auto& jt : std::filesystem::directory_iterator(path))
	{
		auto file_name = jt.path().stem().string();
		if (file_name.compare(0, sizeof(str_region) - 1, str_region) == 0)
		{
			auto name = jt.path().filename().string();
			name.erase(name.begin(), name.begin() + sizeof(str_region) - 1);
			file_name.erase(file_name.begin(), file_name.begin() + sizeof(str_region) - 1);

			auto region_path = path;
			region_path /= (str_region + name);

			result &= page.AddRegion(file_name, region_path.string());
		}
	}
	return result;
}
