#include "WindowCapture.h"

WindowCapture::WindowCapture(const std::string &name)
{
	mName = name;
}

bool WindowCapture::GetCapture(cv::Mat &img)
{
	if (!CheckValid())
		return false;

	if (!CheckVisible())
		return false;

	cv::Mat buf = cv::Mat::zeros(GetWindowSize().height, GetWindowSize().width, CV_8UC4);

	if (mMode == 0)
	{
		if (!CaptureModeDesktop(buf))
			return false;
	}
	else if (mMode == 1)
		if (!CaptureModeWindow(buf))
			return false;

	cv::cvtColor(buf, img, cv::COLOR_BGRA2BGR);
	cv::flip(img, img, 0);

	ShiftCapture(img);

	return true;
}

bool WindowCapture::Click(const cv::Rect &rect)
{
	if (!CheckValid())
		return false;

	if (!CheckVisible())
		return false;

	auto x = rect.x + rect.width / 2 - 7 + GetClickOffset().width;
	auto y = rect.y + rect.height / 2 - 47 + GetClickOffset().height;

	PostMessage(mClickWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	PostMessage(mClickWnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));

	return true;
}


bool WindowCapture::Move(const cv::Rect &rect, int w, int h)
{
	if (!CheckValid())
		return false;

	if (!CheckVisible())
		return false;

	auto center_x = rect.x - 7 + GetClickOffset().width + rect.width / 2;
	auto center_y = rect.y - 47 + GetClickOffset().height + rect.height / 2;
	
	cv::Vec2i center(center_x, center_y);
	cv::Vec2i dir(w * rect.width, h *  rect.height);
	auto half = dir / 2;

	auto start = center - half;
	auto stop = center + half;
	auto path = stop - start;

	cv::Vec2f path_dir = path;
	path_dir = cv::normalize(path_dir);

	DWORD procces_id = 0;
	auto thread_id = GetWindowThreadProcessId(mWnd, &procces_id);
	{
		AttachThreadInput(GetCurrentThreadId(), thread_id, TRUE);

		auto err = GetLastError();

		BYTE arr[256];
		memset(arr, 0, sizeof(256));
		GetKeyboardState(arr);
		auto old = arr[VK_LBUTTON];
		arr[VK_LBUTTON] = 128;
		SetKeyboardState(arr);

		
		SendMessage(mClickWnd, WM_MOUSEACTIVATE, (WPARAM)mWnd, MAKELPARAM(HTCLIENT, WM_LBUTTONDOWN));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		SendMessage(mClickWnd, WM_SETCURSOR, (WPARAM)mClickWnd, MAKELPARAM(HTCLIENT, WM_LBUTTONDOWN));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		PostMessage(mClickWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(start[0], start[1]));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		const int count_in_step = 1;
		const int count = int(cv::norm(path));

		cv::Vec2i last_pos;
		int max_wait = 50;

		int pause = 0;
		for (int i = count_in_step; i < count / count_in_step; ++i)
		{
			auto pos = cv::Vec2i(path_dir * float(i * count_in_step));
			last_pos = start + pos;

			SetKeyboardState(arr);
			PostMessage(mClickWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(last_pos[0], last_pos[1]));

			auto pause = int((float(i) / float(count / count_in_step)) * float(max_wait));
			std::this_thread::sleep_for(std::chrono::milliseconds(pause + 10));
		}

		PostMessage(mClickWnd, WM_LBUTTONUP, 0, MAKELPARAM(last_pos[0], last_pos[1]));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		SendMessage(mClickWnd, WM_SETCURSOR, (WPARAM)mClickWnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		arr[VK_LBUTTON] = old;
		SetKeyboardState(arr);

		AttachThreadInput(GetCurrentThreadId(), thread_id, FALSE);
	}

	return true;
}

bool WindowCapture::CheckValid()
{
	if (IsWindow(mWnd) && IsWindow(mClickWnd))
		return true;

	if (IsWindow(mWnd))
	{
		mClickWnd = GetClickWnd();
		return true;
	}

	mWnd = FindWindow(NULL, mName.c_str());

	if (mWnd)
		return true;

	return false;
}

bool WindowCapture::CheckVisible()
{
	bool res = true;

// 	if (IsIconic(mWnd))
// 		SwitchToThisWindow(mWnd, true);
// 
// 	if (mWnd != GetForegroundWindow())
// 		res |= !!SetForegroundWindow(mWnd);

	WINDOWINFO pwi;
	pwi.cbSize = sizeof(WINDOWINFO);
	res |= !!GetWindowInfo(mWnd, &pwi);

	auto width = pwi.rcWindow.right - pwi.rcWindow.left;
	auto height = pwi.rcWindow.bottom - pwi.rcWindow.top;

	if (width != GetWindowSize().width || height != GetWindowSize().height)
		res |= !!SetWindowPos(mWnd, nullptr, 0, 0, GetWindowSize().width, GetWindowSize().height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	return res;
}

void WindowCapture::SetMode(int mode)
{
	mMode = mode;
}

bool WindowCapture::CaptureModeDesktop(cv::Mat &buf)
{
	bool res = true;

	WINDOWINFO pwi = { 0 };
	pwi.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo(mWnd, &pwi))
	{
		auto &rc = pwi.rcWindow;
		auto x = rc.left;
		auto y = rc.top;
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		if (width == GetWindowSize().width && height == GetWindowSize().height)
		{
			if (HDC hdc = GetDC(HWND_DESKTOP))
			{
				if (HDC memdc = CreateCompatibleDC(hdc))
				{
					if (HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height))
					{
						if (HBITMAP oldbmp = (HBITMAP)SelectObject(memdc, hbitmap))
						{
							res |= !!BitBlt(memdc, 0, 0, width, height, hdc, x, y, SRCCOPY | CAPTUREBLT);
							res |= !!SelectObject(memdc, oldbmp);

							BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), width, height, 1, 32 };
							res |= !!GetDIBits(memdc, hbitmap, 0, height, buf.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

							res |= !!DeleteObject(hbitmap);
						}
						else
							res |= false;
					}
					else
						res |= false;
					DeleteDC(memdc);
				}
				else
					res |= false;
				res |= !!ReleaseDC(HWND_DESKTOP, hdc);
			}
			else
				res |= false;
		}
		else
			res |= false;
	}
	else
		res |= false;

	if (buf.empty())
		return false;

	return res;
}


bool WindowCapture::CaptureModeWindow(cv::Mat &buf)
{
	bool res = true;

	WINDOWINFO pwi;
	pwi.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo(mWnd, &pwi))
	{
		if (auto window_dc = GetWindowDC(mWnd))
		{
			auto width = pwi.rcWindow.right - pwi.rcWindow.left;
			auto height = pwi.rcWindow.bottom - pwi.rcWindow.top;

			if (auto CompatibleHdc = CreateCompatibleDC(window_dc))
			{
				if (HBITMAP bitmap = CreateCompatibleBitmap(window_dc, width, height))
				{
					if (HGDIOBJ gdiobj = SelectObject(CompatibleHdc, bitmap))
					{
						res |= !!BitBlt(CompatibleHdc, 0, 0, width, height, window_dc, 0, 0, SRCCOPY | CAPTUREBLT);
						res |= !!SelectObject(CompatibleHdc, gdiobj);

						BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), width, height, 1, 32 };
						res |= !!GetDIBits(CompatibleHdc, bitmap, 0, height, buf.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
					}
					else
						res |= false;

					res |= !!DeleteObject(bitmap);
				}
				else
					res |= false;

				res |= !!DeleteDC(CompatibleHdc);
			}
			else
				res |= false;

			res |= !!ReleaseDC(mWnd, window_dc);
		}
		else
			res |= false;
	}
	else 
		res |= false;

	return res;
}

