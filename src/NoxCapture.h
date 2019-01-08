#pragma once

#include "WindowCapture.h"

class NoxCapture : public WindowCapture
{
public:
	NoxCapture();

protected:
	virtual HWND GetClickWnd() override;
	virtual void ShiftCapture(cv::Mat &img) override;
	virtual cv::Size GetClickOffset() override;
	virtual cv::Size GetWindowSize() override;

};


