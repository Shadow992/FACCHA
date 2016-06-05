#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include "ImageFramework.h"
#include <math.h>
#include <vector>

namespace ImageFramework
{
    std::vector<float> calculate1DGaussianKernel(int size, float sigma);
}

#endif // IMAGEFILTER_H
