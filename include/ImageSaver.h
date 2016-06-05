#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include "Image.h"
#include "jpeglib.h"
#include "png.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>

namespace ImageFramework
{
    int saveAsPNG(Image* img, const std::string& fileName);
    void saveAsBMP(Image* img, const std::string& fileName);
}

#endif // IMAGESAVER_H
