#pragma once

#include "pch.h"

class WindowCapture
{
public:
	WindowCapture(const std::string &name);
	virtual ~WindowCapture() = default;
	

	bool GetCapture(cv::Mat &img);

	bool Click(const cv::Rect &rect);

	bool Move(const cv::Rect &rect, int w = 0, int h = 0);

	void SetMode(int mode);

protected:
	std::string mName;
	HWND mWnd = nullptr;
	HWND mClickWnd = nullptr;

	int mMode = 0;

protected:
	virtual HWND GetClickWnd() = 0;
	virtual void ShiftCapture(cv::Mat &img) = 0;
	virtual cv::Size GetClickOffset() = 0;
	virtual cv::Size GetWindowSize() = 0;

private:
	bool CheckValid();

	bool CheckVisible();

public:
	bool CaptureModeWindow(cv::Mat &img);
	bool CaptureModeDesktop(cv::Mat &img);
};


