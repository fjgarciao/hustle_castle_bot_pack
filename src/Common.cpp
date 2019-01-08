#include "Common.h"


cv::Point MatchingMethod(const cv::Mat &src, const cv::Mat &templ)
{
	/// Source image to display
	cv::Mat img_display;
	src.copyTo(img_display);

	/// Create the result matrix
	int result_cols = src.cols - templ.cols + 1;
	int result_rows = src.rows - templ.rows + 1;

	cv::Mat result;
	result.create(result_rows, result_cols, CV_32FC1);

	auto match_method = 0;

	/// Do the Matching and Normalize
	matchTemplate(src, templ, result, match_method);
	normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
	cv::Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
		matchLoc = minLoc;
	else
		matchLoc = maxLoc;

	/// Show me what you got
//   rectangle(img_display, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
//   rectangle(result, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
// 
//   imshow("Source Image", img_display);
//   imshow("Result window", result);
// 	cv::waitKey(0);

	return matchLoc;
}

void GenerateDataForTrain()
{
	auto template_info_img = cv::imread("data\\template_info.png", cv::IMREAD_COLOR);
	auto mask_right = cv::imread("data\\mask_right.png", cv::IMREAD_COLOR);

	cv::Mat result_img;
	{
		cv::Rect area;
		MaskToRect(mask_right, area);

		for (auto& p : std::filesystem::directory_iterator("data\\src\\"))
		{
			auto src_img = cv::imread(p.path().string(), cv::IMREAD_COLOR);
			auto roi_img = src_img(area);

			auto pos = MatchingMethod(roi_img, template_info_img);
			roi_img = roi_img(cv::Rect(0, 0, pos.x, roi_img.rows));

			int row = result_img.rows;
			auto old_img = result_img;
			result_img = cv::Mat::zeros(row + roi_img.rows, std::max(result_img.cols, roi_img.cols), CV_8UC3);

			old_img.copyTo(result_img(cv::Rect(0, 0, old_img.cols, old_img.rows)));
			roi_img.copyTo(result_img(cv::Rect(0, row, roi_img.cols, roi_img.rows)));
		}
	}

	// 	{
	// 		Area area;
	// 		area.SetFromMask("data\\mask_left.png");
	// 
	// 		for (auto& p : std::filesystem::directory_iterator("data\\src\\"))
	// 		{
	// 			auto src_img = cv::imread(p.path().string(), cv::IMREAD_COLOR);
	// 			auto roi_img = src_img(area.GetRect());
	// 
	// 			auto pos = MatchingMethod(roi_img, template_info_img);
	// 			roi_img = roi_img(cv::Rect(0, 0, pos.x, roi_img.rows));
	// 
	// 			int row = result_img.rows;
	// 			auto old_img = result_img;
	// 			result_img = cv::Mat::zeros(row + roi_img.rows, std::max(result_img.cols, roi_img.cols), CV_8UC3);
	// 
	// 			old_img.copyTo(result_img(cv::Rect(0, 0, old_img.cols, old_img.rows)));
	// 			roi_img.copyTo(result_img(cv::Rect(0, row, roi_img.cols, roi_img.rows)));
	// 		}
	// 	}

	//   cv::imshow("resultImg", result_img);
	// 
	cv::imwrite("training_image.png", result_img);
}

int TestWinApi()
{
	
	//MatchingMethod(main_img, home_button_img);
	return 0;
}

double Compare(const cv::Mat &a, const cv::Mat &b)
{
	if (a.rows > 0 && a.rows == b.rows && a.cols > 0 && a.cols == b.cols && a.type() == b.type())
	{
//		show_two(a, b);
		double errorL2 = norm(a, b);
		double similarity = errorL2 / (double)(a.rows * a.cols);
		return similarity;
	}
	return 100000000.0;
}

bool CompareFromTemplate(const cv::Mat &src, const cv::Mat &sample, const cv::Mat &tmpl)
{
	cv::Rect area;
	if (!MaskToRect(tmpl, area))
		return false;

	auto a = sample(area);
	auto b = src(area);

	return Compare(a, b) < COMPARE_ERROR;
}

bool MaskToRect(const cv::Mat &img, cv::Rect &rect)
{
	if (!img.size().empty())
	{
		cv::Mat gray;
		cv::cvtColor(img, gray, CV_BGR2GRAY);

		cv::Mat mask;
		cv::threshold(gray, mask, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		if (contours.size() >= 1)
		{
			auto bbox = cv::minAreaRect(contours[0]);
			rect = bbox.boundingRect();
			return true;
		}
	}
	return false;
}

std::string type2str(int type)
{
	std::string r;
	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);
	switch (depth)
	{
	case CV_8U: r = "8U"; break;
	case CV_8S: r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default: r = "User"; break;
	}
	r += "C";
	r += (chans + '0');
	return r;
}

void show_two(const cv::Mat &a, const cv::Mat &b)
{
	cv::Mat both = cv::Mat::zeros(a.rows + b.rows, std::max(a.cols, b.cols), CV_8UC3);

	a.copyTo(both(cv::Rect(0, 0, a.cols, a.rows)));
	b.copyTo(both(cv::Rect(0, a.rows, b.cols, b.rows)));

	cv::imshow("img", both);
	cv::waitKey(0);
}

