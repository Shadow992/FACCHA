#include "ColorConverter.h"

std::vector<ColorXYZ> ImageFramework::convertRGBToXYZ(Image* img)
{
    Image& imgRef = *img;
    std::vector<ColorXYZ> newColorArray(imgRef.width * imgRef.height);

    for (int i = 0; i < imgRef.width * imgRef.height; i++)
    {
        float valR = (static_cast<float>(imgRef.imgData[i].color.red) / 255.0);   // R from 0 to 255
        float valG = (static_cast<float>(imgRef.imgData[i].color.green) / 255.0); // G from 0 to 255
        float valB = (static_cast<float>(imgRef.imgData[i].color.blue) / 255.0);  // B from 0 to 255

        if (valR > 0.04045)
        {
            valR = pow((valR + 0.055) / 1.055, 2.4);
        }
        else
        {
            valR = valR / 12.92;
        }

        if (valG > 0.04045)
        {
            valG = pow((valG + 0.055) / 1.055, 2.4);
        }
        else
        {
            valG = valG / 12.92;
        }

        if (valB > 0.04045)
        {
            valB = pow((valB + 0.055) / 1.055, 2.4);
        }
        else
        {
            valB = valB / 12.92;
        }

        valR = valR * 100.0;
        valG = valG * 100.0;
        valB = valB * 100.0;

        // Observer. = 2°, Illuminant = D65
        newColorArray[i].x = valR * 0.4124 + valG * 0.3576 + valB * 0.1805;
        newColorArray[i].y = valR * 0.2126 + valG * 0.7152 + valB * 0.0722;
        newColorArray[i].z = valR * 0.0193 + valG * 0.1192 + valB * 0.9505;
    }

    return newColorArray;
}

std::vector<CIELab> ImageFramework::convertXYZToLab(std::vector<ColorXYZ>& xyzImg)
{
    std::vector<CIELab> newColorArray(xyzImg.size());

    const float refX = 95.047;
    const float refY = 100.000;
    const float refZ = 108.883;

    for (unsigned int i = 0; i < xyzImg.size(); i++)
    {
        float valX = xyzImg[i].x / refX;
        float valY = xyzImg[i].y / refY;
        float valZ = xyzImg[i].z / refZ;

        if (valX > 0.008856)
        {
            valX = pow(valX, (1.0 / 3.0));
        }
        else
        {
            valX = (7.787 * valX) + (16.0 / 116.0);
        }
        if (valY > 0.008856)
        {
            valY = pow(valY, (1.0 / 3.0));
        }
        else
        {
            valY = (7.787 * valY) + (16.0 / 116.0);
        }
        if (valZ > 0.008856)
        {
            valZ = pow(valZ, (1.0 / 3.0));
        }
        else
        {
            valZ = (7.787 * valZ) + (16.0 / 116.0);
        }

        newColorArray[i].L = (116.0 * valY) - 16.0;
        newColorArray[i].a = 500.0 * (valX - valY);
        newColorArray[i].b = 200.0 * (valY - valZ);
    }

    return newColorArray;
}

std::vector<ColorXYZ> ImageFramework::convertLabToXYZ(std::vector<CIELab>& imgLab)
{
    std::vector<ColorXYZ> newColorArray(imgLab.size());

    const float refX = 95.047;
    const float refY = 100.000;
    const float refZ = 108.883;

    for (unsigned int i = 0; i < imgLab.size(); i++)
    {
        float valY = (imgLab[i].L + 16.0) / 116.0;
        float valX = imgLab[i].a / 500.0 + valY;
        float valZ = valY - imgLab[i].b / 200.0;

        const float valY3 = valY * valY * valY;
        if (valY3 > 0.008856)
        {
            valY = valY3;
        }
        else
        {
            valY = (valY - 16.0 / 116.0) / 7.787;
        }

        const float valX3 = valX * valX * valX;
        if (valX3 > 0.008856)
        {
            valX = valX3;
        }
        else
        {
            valX = (valX - 16.0 / 116.0) / 7.787;
        }

        const float valZ3 = valZ * valZ * valZ;
        if (valZ3 > 0.008856)
        {
            valZ = valZ3;
        }
        else
        {
            valZ = (valZ - 16.0 / 116.0) / 7.787;
        }

        newColorArray[i].x = refX * valX;
        newColorArray[i].y = refY * valY;
        newColorArray[i].z = refZ * valZ;
    }
    return newColorArray;
}

Image* ImageFramework::convertXYZToRGB(std::vector<ColorXYZ>& xyzImg, int width, int height)
{
    auto img = new Image(width, height);

    if (img == nullptr || !convertXYZToRGB(xyzImg, img))
    {
        return nullptr;
    }

    Image& imgRef = *img;

    for (int y = 0; y < height; y++)
    {
        const int yOffset = y * width;
        for (int x = 0; x < height; x++)
        {
            imgRef.imgData[x + yOffset].x = x;
            imgRef.imgData[x + yOffset].y = y;
        }
    }

    return img;
}

bool ImageFramework::convertXYZToRGB(std::vector<ColorXYZ>& xyzImg, Image* img)
{
    if (img == nullptr)
    {
        return false;
    }

    Image& imgRef = *img;

    for (unsigned int i = 0; i < xyzImg.size(); i++)
    {
        float valX = xyzImg[i].x / 100.0; // X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
        float valY = xyzImg[i].y / 100.0; // Y from 0 to 100.000
        float valZ = xyzImg[i].z / 100.0; // Z from 0 to 108.883

        float valR = valX * 3.2406 + valY * -1.5372 + valZ * -0.4986;
        float valG = valX * -0.9689 + valY * 1.8758 + valZ * 0.0415;
        float valB = valX * 0.0557 + valY * -0.2040 + valZ * 1.0570;

        if (valR > 0.0031308)
        {
            valR = 1.055 * (pow(valR, 1.0 / 2.4)) - 0.055;
        }
        else
        {
            valR = 12.92 * valR;
        }
        if (valG > 0.0031308)
        {
            valG = 1.055 * (pow(valG, 1.0 / 2.4)) - 0.055;
        }
        else
        {
            valG = 12.92 * valG;
        }
        if (valB > 0.0031308)
        {
            valB = 1.055 * (pow(valB, 1.0 / 2.4)) - 0.055;
        }
        else
        {
            valB = 12.92 * valB;
        }

        imgRef.imgData[i].color.red = valR * 255.0;
        imgRef.imgData[i].color.green = valG * 255.0;
        imgRef.imgData[i].color.blue = valB * 255.0;
    }

    return true;
}
