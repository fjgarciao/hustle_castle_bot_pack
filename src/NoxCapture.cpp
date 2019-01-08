#include "NoxCapture.h"

// 958 539

NoxCapture::NoxCapture()
	: WindowCapture("NoxPlayer")
{

}

HWND NoxCapture::GetClickWnd()
{
	return mWnd;
}

void NoxCapture::ShiftCapture(cv::Mat &img)
{
	cv::Mat out = cv::Mat::zeros(633, 973, CV_8UC3);

	cv::Mat win = img(cv::Rect(2, 30, 958, 539));
	win.copyTo(out(cv::Rect(7, 47, win.cols, win.rows)));

	img = out;
}

cv::Size NoxCapture::GetClickOffset()
{
	return { 2, 30 };
}

cv::Size NoxCapture::GetWindowSize()
{
	return {962, 572};
}
