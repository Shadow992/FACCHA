#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include "Image.h"
#include "png.h"
#include "jpeglib.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>

namespace ImageFramework
{
    int saveAsPNG(Image* img, const std::string& fileName);
    void saveAsBMP(Image* img, const std::string& fileName);
}

#endif // IMAGESAVER_H
