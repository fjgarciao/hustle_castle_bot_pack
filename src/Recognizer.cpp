#include "Recognizer.h"

#include "Common.h"

const cv::Size gTemplateSymbolSize = { 20, 30 };
const cv::Size gMinimumSymbolSize = { 5, 5 };

Recognizer::Recognizer()
{
	mKNearest = cv::ml::KNearest::create();
}

bool Recognizer::LoadTrain(const std::string &file_name)
{
	cv::FileStorage fs_classifications(file_name, cv::FileStorage::READ);

	if (fs_classifications.isOpened())
	{                                                       
		cv::Mat classifications;
		cv::Mat images;

		fs_classifications["classifications"] >> classifications;
		fs_classifications["images"] >> images;

		mKNearest->setDefaultK(1);

		return mKNearest->train(images, cv::ml::ROW_SAMPLE, classifications);
	}

	return false;
}



std::string Recognizer::RecognizeChars(cv::Mat &ready_img, std::vector<std::vector<cv::Point>> &array_chars)
{
	std::string output;               

	for (auto &symbol : array_chars) 
	{           
		cv::Mat symbol_img = ready_img(cv::boundingRect(symbol));

		cv::Mat template_img;
		cv::resize(symbol_img, template_img, gTemplateSymbolSize);

		cv::Mat tenplate_img_float;
		template_img.convertTo(tenplate_img_float, CV_32FC1);

		cv::Mat template_img_reshape = tenplate_img_float.reshape(1, 1);
		cv::Mat finded_symbol_img(0, 0, CV_32F);                  
		mKNearest->findNearest(template_img_reshape, 1, finded_symbol_img);

		auto finded = finded_symbol_img.at<float>(0, 0);
		output = output + char(finded);
	}

	return output;           
}



cv::Mat Recognizer::Preprocess(const cv::Mat &img)
{
	cv::Mat filter;

	{
		auto color_scatter_one = 15;
		auto color_scatter = cv::Scalar(color_scatter_one, color_scatter_one, color_scatter_one);
		auto color = cv::Scalar(240, 240, 240);
		cv::inRange(img, color - color_scatter, color + color_scatter, filter);
	}

	cv::Mat out;
	auto zeros = cv::Mat::zeros(img.size(), CV_8UC3);

	cv::bitwise_or(img, zeros, out, filter);

	cv::Mat gray;
	cv::cvtColor(out, gray, CV_BGR2GRAY);

	cv::bitwise_not(gray, gray);

	cv::Mat threshold;
	cv::threshold(gray, threshold, 200, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

	return threshold;
}

bool Recognizer::ToTrain(const std::string &input_file, const std::string &output_file)
{
	auto src_img = cv::imread(input_file);

	if (src_img.empty())
		return false;

	cv::Mat ready_img = Preprocess(src_img);

	cv::Mat array_chars;
	cv::Mat array_images;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(ready_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
		if (cv::contourArea(contours[i]) > gMinimumSymbolSize.area())
		{
			cv::Rect boundingRect = cv::boundingRect(contours[i]);

			cv::rectangle(src_img, boundingRect, cv::Scalar(0, 0, 255), 1);

			cv::Mat countor_img = ready_img(boundingRect);

			cv::Mat template_symbol_img;
			cv::resize(countor_img, template_symbol_img, gTemplateSymbolSize);

			cv::imshow("template_symbol_img", template_symbol_img);
			cv::imshow("src_img", src_img);

			int intChar = cv::waitKey(0);

			if (intChar == 27)
				return(0);
			else if (intChar != 20)
			{
				array_chars.push_back(intChar);

				cv::Mat matImageFloat;
				template_symbol_img.convertTo(matImageFloat, CV_32FC1);
				cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);

				array_images.push_back(matImageFlattenedFloat);
			}
		}

	{
		cv::FileStorage classifications(output_file, cv::FileStorage::WRITE);

		if (!classifications.isOpened())
			return false;

		classifications << "classifications" << array_chars;
		classifications << "images" << array_images;
	}

	return true;
}


std::string Recognizer::Recognize(const cv::Mat &img)
{
	if (img.empty())
		return {};

	cv::Mat ready_img = Preprocess(img);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(ready_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> array_chars;
	for (int i = 0; i < contours.size(); i++)
		if (cv::contourArea(contours[i]) > gMinimumSymbolSize.area())
			array_chars.push_back(contours[i]);

	std::sort(array_chars.begin(), array_chars.end(), [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b)
	{
		auto rect_a = cv::boundingRect(a);
		auto rect_b = cv::boundingRect(b);
		return (rect_a.br() + rect_a.tl()).x / 2 < (rect_b.br() + rect_b.tl()).x / 2;
	});

	return RecognizeChars(ready_img, array_chars);
}


