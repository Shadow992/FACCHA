#ifndef IMAGESTATISTICS_H
#define IMAGESTATISTICS_H

#include "Image.h"
#include <vector>
#include "ImageManipulation.h"
#include "ImageFramework.h"

namespace ImageFramework
{
    int calculateMaxPixelVariance(const std::vector<Pixel*>& pixelData, int mean, int gate);
    Point calculateCenter(const std::vector<Pixel*>& points);
    int countPixelAlternationsRingMorphology( const std::vector<Pixel*>& pixelData, int mean,int& countBlackArea1 ,int& countBlackArea2,int& countWhiteArea1 ,int& countWhiteArea2);
    int countPixelAlternationsRing( const std::vector<Pixel*>& pixelData, int mean, int gate);
    float getVariance( const std::vector<Pixel*>& pixelData,int mean);
    int getMean( const std::vector<Pixel*>& pixelData);
    int getPointSymmetricDifferences(Image* img, int x, int y, int radius, int mean);
    int getSetPixelCountInLine(Image* img, const std::vector<bool>& binPixel,int& totalPixelCountInLine,int xStart, int yStart, int xEnd, int yEnd, int widthArea);
    int getRegionDifference(Image* img, int x, int y, int radius);
    bool calculateFirstOrderDerivatives(Image* img, int x, int y, int& Ix, int& Iy);
    bool calculateSecondOrderDerivatives(Image* img, int x, int y, int& Ixx, int& Ixy, int& Iyy);
    void getRGBVariance(Image* img, int& rVar, int& gVar, int& bVar);
}

#endif // IMAGESTATISTICS_H
