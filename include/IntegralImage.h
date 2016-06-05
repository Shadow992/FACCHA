#ifndef INTEGRALIMAGE_H
#define INTEGRALIMAGE_H

#include "Image.h"
#include <vector>

class IntegralImage
{
public:
    IntegralImage(){};
    ~IntegralImage(){};
    void calculate(Image& img);
    void getMean(int x, int y, int widthWindow, int heightWindow, int& meanR, int& meanG, int& meanB);
    int getRegionDifference(int x, int y, int radius, int& direction);

    std::vector<int> sumR;
    std::vector<int> sumG;
    std::vector<int> sumB;

    int width;
    int height;
};

#endif
