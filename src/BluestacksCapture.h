#pragma once

#include "WindowCapture.h"

class BluestacksCapture : public WindowCapture
{
public:
	BluestacksCapture();

protected:
	virtual HWND GetClickWnd() override;
	virtual void ShiftCapture(cv::Mat &img) override;
	virtual cv::Size GetClickOffset() override;
	virtual cv::Size GetWindowSize() override;

private:
	HWND FindChildWindow(HWND wnd, const std::string &name);
};


