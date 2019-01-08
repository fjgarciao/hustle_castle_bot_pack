#pragma once

#include "pch.h"

class Recognizer
{
public:
	Recognizer();

	static bool ToTrain(const std::string &input_file, const std::string &output_file);

	std::string Recognize(const cv::Mat &img);

	bool LoadTrain(const std::string &file_name);

private:
	cv::Ptr<cv::ml::KNearest> mKNearest;
	std::string RecognizeChars(cv::Mat &ready_img, std::vector<std::vector<cv::Point>> &array_chars);

	static cv::Mat Preprocess(const cv::Mat &img);
};

