#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "Image.h"
#include "ImageFramework.h"
#include "jpeglib.h"
#include "png.h"

namespace ImageFramework
{
    Image* readFile(const std::string& imageFile);

#if REMOVE_WINDOWS_SPECIFIC != 1
    Image* captureWindowFromHandle(HWND hWnd);
    Image* captureWindow(const std::string& str);
#endif

    void parseBMPData(Image* img, unsigned char* currData, int bmpWidth, int bmpHeight, unsigned int totalSize,
        bool bottomUpImage, int bmpType);
    Image* parseBMPFile(char* memblock, unsigned int size);
    Image* readAndParsePNGFile(const std::string& imageFile);
    Image* readAndParseBMPFile(const std::string& imageFile);
    Image* readAndParseJPEGFile(const std::string& fileName);
}

#endif // IMAGELOADER_H
