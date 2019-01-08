#pragma once

#include "pch.h"

cv::Point MatchingMethod(const cv::Mat &src, const cv::Mat &templ);

void GenerateDataForTrain();

int TestWinApi();

double Compare(const cv::Mat &a, const cv::Mat &b);

bool CompareFromTemplate(const cv::Mat &src, const cv::Mat &sample, const cv::Mat &tmpl);

bool MaskToRect(const cv::Mat &img, cv::Rect &rect);

std::string type2str(int type);

void show_two(const cv::Mat &a, const cv::Mat &b);
