#include "Applet.h"
#include "Logic.h"
#include "BluestacksCapture.h"
#include "NoxCapture.h"

const auto SETW_SIZE = 30;

Applet::Applet()
	: mCurrentPage(mBook.GetUnknownPage())
{
	mWindowCapture = std::make_unique<BluestacksCapture>();
	mIsLoaded = true;
	mIsLoaded &= mBook.Load();
	mIsLoaded &= mRecognizer.LoadTrain("training.xml");
}

void Applet::SetEmulator(int type)
{
	if (type == 0)
		mWindowCapture = std::make_unique<BluestacksCapture>();
	else if (type == 1)
		mWindowCapture = std::make_unique<NoxCapture>();
}

void Applet::SetCaptureMode(int mode)
{
	mWindowCapture->SetMode(mode);
}

bool Applet::IsLoaded() const
{
	return mIsLoaded;
}

bool Applet::Scan()
{
	bool res = true;
	res &= mWindowCapture->GetCapture(mBufferCapture);
	
	if (res)
	{
		mCurrentPage = mBook.Detect(mBufferCapture, mPageError);
		mCurrentState = mCurrentPage.get().DetectState(mBufferCapture, mStateError);
	}

	return res;
}

bool Applet::PressButton(const std::string &name)
{
	return mCurrentPage.get().PressButton(*mWindowCapture, name);
}

bool Applet::Move(const std::string &name, int w, int h)
{
	return mCurrentPage.get().Move(*mWindowCapture, name, w, h);
}

const std::string & Applet::PageName() const
{
	return mCurrentPage.get().Name();
}

const std::string & Applet::StateName() const
{
	return mCurrentState;
}

bool Applet::ReadRegion(const std::string &name, cv::Mat &img)
{
	return mCurrentPage.get().ReadRegion(mBufferCapture, name, img);
}

int Applet::Recognize(const cv::Mat &img)
{
	int out = 0;
	std::istringstream(mRecognizer.Recognize(img)) >> out;
	return out;
}

double Applet::GetPageError() const
{
	return mPageError;
}

double Applet::GetStateError() const
{
	return mStateError;
}


AppletWrapper::AppletWrapper(Applet &app, Logic &logic)
	: mApplet(app), mLogic(logic)
{
	mLogFile.open("log.txt");
}

bool AppletWrapper::Scan()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	
	auto res = mApplet.Scan();
	if (res)
	{
		std::cout << std::put_time(&tm, "%H:%M:%S  ");

		std::cout << std::setw(33) << std::left
			<< "[" + std::string(mLogic.getState().getName()) + "]";
		std::cout << std::setiosflags(std::ios::fixed) << std::setw(30) << std::left
			<< "page: [" + PageName() + "]";
		std::cout << std::setiosflags(std::ios::fixed) << std::setw(20) << std::left
			<< "state: [" + StateName() + "]";
		std::cout << std::setiosflags(std::ios::fixed) << std::setw(25) << std::left
			<< "scanned";
		std::cout << std::setiosflags(std::ios::fixed) << std::setw(0) << std::setprecision(3) << std::left
			<< "ep: " << mApplet.GetPageError() << "  es: " << mApplet.GetStateError() << std::endl;
	}
	else
		std::cout << "scan error" << std::endl;

	if (mLogFile.is_open())
	{
		if (res)
		{
			mLogFile << std::put_time(&tm, "%H:%M:%S  ");

			mLogFile << std::setw(33) << std::left
				<< "[" + std::string(mLogic.getState().getName()) + "]";
			mLogFile << std::setiosflags(std::ios::fixed) << std::setw(30) << std::left
				<< "page: [" + PageName() + "]";
			mLogFile << std::setiosflags(std::ios::fixed) << std::setw(20) << std::left
				<< "state: [" + StateName() + "]";
			mLogFile << std::setiosflags(std::ios::fixed) << std::setw(25) << std::left
				<< "scanned";
			mLogFile << std::setiosflags(std::ios::fixed) << std::setw(0) << std::setprecision(3) << std::left
				<< "ep: " << mApplet.GetPageError() << "  es: " << mApplet.GetStateError() << std::endl;
		}
		else
			mLogFile << "scan error" << std::endl;
		mLogFile.flush();
	}

	mLogic.Next();
	return res;
}

bool AppletWrapper::PressButton(const std::string &name)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::cout << std::put_time(&tm, "%H:%M:%S  ");

	std::cout << std::setw(33) << std::left
		<< "[" + std::string(mLogic.getPreviousState()->getName()) + "]";
	std::cout << std::setiosflags(std::ios::fixed) << std::setw(30) << std::left
		<< "page: [" + PageName() + "]";
	std::cout << std::setiosflags(std::ios::fixed) << std::setw(20) << std::left
		<< "state: [" + StateName() + "]";
	std::cout << std::setiosflags(std::ios::fixed) << std::setw(25) << std::left
		<< "press button: [" + name + "]" << std::endl;

	if (mLogFile.is_open())
	{
		mLogFile << std::put_time(&tm, "%H:%M:%S  ");

		mLogFile << std::setw(33) << std::left
			<< "[" + std::string(mLogic.getPreviousState()->getName()) + "]";
		mLogFile << std::setiosflags(std::ios::fixed) << std::setw(30) << std::left
			<< "page: [" + PageName() + "]";
		mLogFile << std::setiosflags(std::ios::fixed) << std::setw(20) << std::left
			<< "state: [" + StateName() + "]";
		mLogFile << std::setiosflags(std::ios::fixed) << std::setw(25) << std::left
			<< "press button: [" + name + "]" << std::endl;
		mLogFile.flush();
	}

	return mApplet.PressButton(name);
}

bool AppletWrapper::PressButton(int val)
{
	return PressButton(std::to_string(val));
}

bool AppletWrapper::Move(const std::string &name, int w, int h)
{
	return mApplet.Move(name, w, h);
}

const std::string & AppletWrapper::PageName() const
{
	return mApplet.PageName();
}

const std::string & AppletWrapper::StateName() const
{
	return mApplet.StateName();
}

void AppletWrapper::RunBluestacks()
{
	system(R"("Hustle Castle.lnk")");
}
