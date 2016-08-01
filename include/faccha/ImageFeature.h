#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iostream>
#include <vector>

using namespace cv;

inline unsigned int isqrt(unsigned int x);

namespace ImageFramework
{
    void invertImage(Mat& img);
    void removeSmallRegions(
        std::vector<bool>& binarizedImage, int width, int height, int minSize, int radius, bool groupingColor);
    void fillOnePixelHoles(std::vector<bool>& binPixel);
    std::vector<bool> adaptiveBinarization(Mat& img, int radius, int radiusSecondary);
    Vec3i getIntegralImageMean(Mat& integralImg, int xStart, int xEnd, int yStart, int yEnd);
    Mat getSaliencyMap(
        Mat& img, int smallWindowSize, int scaleStep, int startingScale, int endingScale, int randomOtherRegions);
}
