#pragma once

#include "pch.h"
#include "WindowCapture.h"
#include "Page.h"
#include "Recognizer.h"


class Applet
{
public:
	Applet();

	void SetEmulator(int type);
	void SetCaptureMode(int mode);

	bool IsLoaded() const;

	bool Scan();

	bool PressButton(const std::string &name);
	bool Move(const std::string &name, int w, int h);

	const std::string &PageName() const;
	const std::string &StateName() const;

	bool ReadRegion(const std::string &name, cv::Mat &img);

	int Recognize(const cv::Mat &img);

	double GetPageError() const;
	double GetStateError() const;

private:
	std::unique_ptr<WindowCapture> mWindowCapture;
	Book mBook;
	Recognizer mRecognizer;
	cv::Mat mBufferCapture;

	double mPageError = -1;
	double mStateError = -1;
	bool mIsLoaded = false;

	std::reference_wrapper<Page> mCurrentPage;
	std::string mCurrentState;
};


class Logic;
class AppletWrapper
{
public:
	AppletWrapper(Applet &app, Logic &logic);

	bool Scan();
	bool PressButton(int val);
	bool PressButton(const std::string &name);
	bool Move(const std::string &name, int w, int h);
	const std::string &PageName() const;
	const std::string &StateName() const;

	void RunBluestacks();

private:
	Applet &mApplet;
	Logic &mLogic;
	std::ofstream mLogFile;
};
