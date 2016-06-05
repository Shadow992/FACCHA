#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>

#include "Image.h"
#include "jpeglib.h"
#include "png.h"

namespace ImageFramework
{
    int saveAsPNG(Image* img, const std::string& fileName);
    void saveAsBMP(Image* img, const std::string& fileName);
}

#endif // IMAGESAVER_H
