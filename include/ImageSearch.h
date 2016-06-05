#ifndef IMAGESEARCH_H
#define IMAGESEARCH_H

#include "FeatureExtraction.h"
#include "Image.h"

namespace ImageFramework
{
    Pixel* searchForImage(Image* img, int startX, int startY, const Image* searchImage, unsigned char variance);
    bool equalsImage(Image* img, int x, int y, const Image* searchImage, unsigned char variance);
    Pixel* searchForImageNextToPosition(Image* img, int x, int y, const Image* searchImage, unsigned char variance);
    Pixel* searchForPixel(Image* img, int startX, int startY, const Color& searchPixel, unsigned char variance);
    Pixel* searchForPixel(Image* img, int startX, int startY, unsigned char red, unsigned char green,
        unsigned char blue, unsigned char variance);
    Pixel* searchPixelNextToPosition(Image* img, int x, int y, const Color& searchPixel, unsigned char variance);
    Pixel* searchPixelNextToPosition(
        Image* img, int x, int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char variance);
    float checkHistogramDifferences(const std::vector<float>& histogram, const std::vector<float>& histogram2);
    float findSimilarImageHillClimbing(Image* img, int startX, int startY, int w, int h, Image* checkImage,
        int accuracy, float overlap, int& bestX, int& bestY);
    void findBetterSolution(Image* img, std::vector<float>& histogramSearch, int x, int y, int w, int h,
        Image* checkImage, int accuracy, int& bestX, int& bestY, float& fitness);
};

#endif // IMAGESEARCH_H
