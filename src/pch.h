#pragma once
#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include <memory>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <array>
#include <sstream>
#include <string>
#include <cstdlib>
#include <map>
#include <rapidjson/rapidjson.h>

// constexpr auto PAUSE_TIME = 50;
// constexpr auto ATTEMPT_TIME = 1000 * 30;
// constexpr auto ATTEMPT_COUNT = ATTEMPT_TIME / PAUSE_TIME;

constexpr auto COMPARE_ERROR = 0.5;