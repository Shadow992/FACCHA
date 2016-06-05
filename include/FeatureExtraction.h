#ifndef FEATUREEXTRACTION_H
#define FEATUREEXTRACTION_H

#include<vector>
#include "Image.h"
#include "ImageManipulation.h"
#include "ImageStatistics.h"

namespace ImageFramework
{
    std::vector<float> calcHistogram(Image* img, int accuracy,int xStart, int yStart, int w, int h);
    void calculateHistogramFeatureVectorOfLB(std::vector<float>& featureVector, const std::vector<ImageLBP>& inputLBP, int xStart, int yStart, int widthLBP, int heightLBP, int classifactionWindowWidth, int classificationWindowHeight, int histogramWidth, int histogramHeight, int valueBins, int archBins, float overlapHistogram, int* mappingValuesToBins, int* mappingArchesToBins);
    std::vector<ImageLBP> calculateLBPOfImage(Image* img, int radius, int threshold, int regionMeanThreshold, bool fillHoles);
}

#endif // FEATUREEXTRACTION_H
