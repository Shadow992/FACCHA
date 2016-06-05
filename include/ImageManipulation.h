#ifndef IMAGEMANIPULATION_H
#define IMAGEMANIPULATION_H

#include "Image.h"
#include <vector>
#include "ImageFramework.h"
#include <iostream>
#include <fstream>
#include <iomanip>

namespace ImageFramework
{
    Image* medianFilter(Image* img, int radius);
    std::vector<bool> adaptiveBinarization(Image* img, int radius, int radiusSecondary);
    Image* resizeImageNearestNeighbour(Image* img, int newWidth, int newHeight);
    void add(Image* destImg, const Image* img, bool needsNormalization=false);
    void sub(Image* destImg, const Image* img, bool needsNormalization=false);
    Image* convolutionFilter(Image* img, float *filter, int filterWidth, int filterHeight, bool needsNormalization=false);
    Image* convolutionFilter(Image* img, std::vector<float>& filter, int filterWidth, int filterHeight, bool needsNormalization=false);
    void convertToGrayscaleImage(Image* img, bool equalWeights=false);
    void erode(std::vector<bool>& binPixel, int widthBinPixel, int heightBinPixel, int radiusWidthDilation, int radiusHeightDilation);
    void dilate(std::vector<bool>& binPixel, int widthBinPixel, int heightBinPixel, int radiusWidthDilation, int radiusHeightDilation);
    void fillOnePixelHoles(std::vector<bool>& binPixel);
    void invertImage(Image* img);
    void removeSmallRegions(std::vector<bool>& binarizedImage, int width, int height, int minSize, int radius=1, bool groupingColor=false);
}

#endif // IMAGEMANIPULATION_H
