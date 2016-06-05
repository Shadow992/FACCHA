#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <vector>
#include "ImageFramework.h"
#include <math.h>

namespace ImageFramework
{
    std::vector<float> calculate1DGaussianKernel(int size, float sigma);
}

#endif // IMAGEFILTER_H
