#ifndef DISTANCETRANSFORMER_H
#define DISTANCETRANSFORMER_H

#include "ContourDetector.h"
#include <vector>

struct DistanceTransformation
{
    std::vector<float> gradientUpDownDT;
    std::vector<float> gradientLeftRightDT;
};

class DistanceTransformer
{
public:
    DistanceTransformer();
    virtual ~DistanceTransformer();

    static void calculateDistanceTransformation1D(const std::vector<float>& f, std::vector<float>& d);
    static DistanceTransformation calculateDistanceTransformation(
        const std::vector<OrientedGradient>& inputOG, int width, int height);
    static std::vector<float> calculateDistanceTransformation(
        const std::vector<bool>& binaryImg, int width, int height);
    static void calculateDistanceTransformation(std::vector<float>& calculatedDT, int width, int height);

protected:
private:
};

#endif // DISTANCETRANSFORMER_H
