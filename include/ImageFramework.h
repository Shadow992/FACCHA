#ifndef IMAGEFRAMEWORK_H
#define IMAGEFRAMEWORK_H

#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include "Image.h"

#define SQUARE_OF_2 1.41421356237
#define NUM_1_DIVIDED_SQUARE_OF_2 0.70710678118
#define M_PI 3.14159265359
#define M_E 2.71828182846
#define REMOVE_WINDOWS_SPECIFIC 1

const unsigned int NORMAL_BMP_HEADER_SIZE=54;

#if REMOVE_WINDOWS_SPECIFIC !=1
	#include <Windows.h>
#endif

struct Point
{
    float x;
    float y;
};

std::ostream& operator<<(std::ostream& os, const Point& p);

struct ImageLBP
{
    int archDirection;
    int length;
	int gradientX;
	int gradientY;
	int gradientMagnitudeSquare;
	int highestGradientColor;
};

unsigned int isqrt(unsigned int x);



#include "ImageFilter.h"
#include "ImageLoader.h"
#include "ImageManipulation.h"
#include "ImageSaver.h"
#include "ImageSearch.h"
#include "ImageStatistics.h"
#include "FeatureExtraction.h"
#include "IntegralImage.h"
#include "ContourDetector.h"
#include "DistanceTransformer.h"
#include "PartTemplate.h"

#endif // IMAGEFRAMEWORK_H
