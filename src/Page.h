#pragma once

#include "pch.h"
#include "WindowCapture.h"

class Page
{
public:
	Page(const std::string &name);
	Page(const Page &) = delete;
	Page &operator=(const Page &) = delete;
	Page(Page &&) = default;
	Page &operator=(Page &&) = default;

	bool SetPage(const std::string &sample_file, const std::string &mask_file);

	bool AddButton(std::string name, const std::string &mask_file);

	bool AddState(std::string name, const std::string &sample_file, const std::string &mask_file);

	bool AddRegion(std::string name, const std::string &mask_file);


	bool PressButton(WindowCapture &capture, std::string name);

	bool Move(WindowCapture &capture, std::string name, int w, int h);

	bool SetMasks(cv::Mat &src, bool only_detect = false);

	const std::string &Name() const;

	bool Detect(const cv::Mat &src);
	bool Detect(const cv::Mat &src, double &err);

	bool ReadRegion(const cv::Mat &src, std::string name, cv::Mat &dst);

	bool CheckState(const cv::Mat &src, std::string name);

	std::string DetectState(const cv::Mat &src);
	std::string DetectState(const cv::Mat &src, double &err);

private:
	struct WorkArea
	{
		cv::Mat sample;
		cv::Rect rect;
	};

private:
	std::string mName;

	WorkArea mDetectArea;

	std::map<std::string, cv::Rect> mButtons;
	std::map<std::string, WorkArea> mStates;
	std::map<std::string, cv::Rect> mRegions;
};

class Book
{
public:
	Book();

	bool Load();

	Page &GetUnknownPage();
	Page &Detect(const cv::Mat &src);
	Page &Detect(const cv::Mat &src, double &err);

private:
	Page mUnknownPage;
	std::vector<Page> mPages;

private:
	bool LoadPage(const std::filesystem::path &path, Page &page);

};



