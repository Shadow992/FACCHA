#ifndef COLORCONVERTER_H
#define COLORCONVERTER_H

#include <vector>
#include "Image.h"

namespace ImageFramework
{
    std::vector<ColorXYZ> convertRGBToXYZ(Image* img);
    std::vector<CIELab> convertXYZToLab(std::vector<ColorXYZ>& xyzImg);
    std::vector<ColorXYZ> convertLabToXYZ(std::vector<CIELab>& imgLab);
    bool convertXYZToRGB(std::vector<ColorXYZ>& xyzImg, Image* img);
    Image* convertXYZToRGB(std::vector<ColorXYZ>& xyzImg, int width, int height);
}

#endif // COLORCONVERTER_H
