#ifndef CONTOURDETECTOR_H
#define CONTOURDETECTOR_H

#include "Image.h"
#include "IntegralImage.h"

struct Moment
{
    float momentR;
    float momentG;
    float momentB;
};

struct OrientedGradient
{
    int direction = -1;
    int gradientMagnitude = -1;
};

namespace ContourDetector
{
    std::vector<Moment> calculateMomentForRegion(Image& img, int x, int y, int radius);
    std::vector<OrientedGradient> extractContours(
        Image& img, int radius, int threshold, int hysteresisThreshLow, int hysteresisThreshHigh, int minPts = 4);
    std::vector<OrientedGradient> getOrientedGradients(Image& img, int radius, int threshold, bool bigger = true);
    void performNMPOnOG(std::vector<OrientedGradient>& inputOG, int widthOG, int heightOG);
    void performHysteresis(
        std::vector<OrientedGradient>& inputOG, int widthOG, int heightOG, int low, int high, int minPts);
    void performHysteresisRec(std::vector<OrientedGradient>& inputOG, int widthOG, int heightOG,
        std::vector<bool>& visited, int x, int y, int low, int high, int minPts, int& countEdgePoints);
};

#endif // CONTOURDETECTOR_H
