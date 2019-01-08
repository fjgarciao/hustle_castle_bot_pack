#include "BluestacksCapture.h"

BluestacksCapture::BluestacksCapture()
	: WindowCapture("BlueStacks")
{

}

char getWndName[200], catchIt[200];       //globally declared


namespace
{
	HWND EnumChildProcHWND = nullptr;

	BOOL CALLBACK EnumChildProc(HWND hwChild, LPARAM lParam)
	{
		char finded_name[200];
		if (GetWindowText(hwChild, finded_name, 200) != 0)
			if (strcmp(finded_name, (char *)lParam) == 0)
			{
				EnumChildProcHWND = hwChild;
				return FALSE;
			}

		return TRUE;
	}
}

HWND BluestacksCapture::FindChildWindow(HWND wnd, const std::string &name)
{
	EnumChildProcHWND = nullptr;
	EnumChildWindows(mWnd, EnumChildProc, (LPARAM)name.data());
	return EnumChildProcHWND;
}

HWND BluestacksCapture::GetClickWnd()
{
	return FindChildWindow(mWnd, "BlueStacks Android PluginAndroid");
}

void BluestacksCapture::ShiftCapture(cv::Mat &img)
{

}

cv::Size BluestacksCapture::GetClickOffset()
{
	return { 0 , 0 };
}

cv::Size BluestacksCapture::GetWindowSize()
{
	return { 973 , 633 };
}
