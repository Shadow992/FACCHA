#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iostream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

inline unsigned int isqrt(unsigned int x);

namespace ImageFramework
{
    void invertImage(cv::Mat& img);
    void removeSmallRegions(
        std::vector<bool>& binarizedImage, int width, int height, int minSize, int radius, bool groupingColor);
    void fillOnePixelHoles(std::vector<bool>& binPixel);
    std::vector<bool> adaptiveBinarization(cv::Mat& img, int radius, int radiusSecondary);
    cv::Vec3i getIntegralImageMean(cv::Mat& integralImg, int xStart, int xEnd, int yStart, int yEnd);
    cv::Mat getSaliencyMap(
        cv::Mat& img, int smallWindowSize, int scaleStep, int startingScale, int endingScale, int randomOtherRegions);
}
