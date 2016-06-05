#include "ImageStatistics.h"

#include <cassert>

void ImageFramework::getRGBVariance(Image* img, int& rVar, int& gVar, int& bVar)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    unsigned long long meanR = 0;
    unsigned long long meanG = 0;
    unsigned long long meanB = 0;

    unsigned long long varR = 0;
    unsigned long long varG = 0;
    unsigned long long varB = 0;

    const int imgSize = imgRef.width * imgRef.height;
    for (int i = 0; i < imgSize; i++)
    {
        meanR += imgRef.imgData[i].color.red;
        meanG += imgRef.imgData[i].color.green;
        meanB += imgRef.imgData[i].color.blue;
    }

    meanR /= imgSize;
    meanG /= imgSize;
    meanB /= imgSize;

    for (int i = 0; i < imgSize; i++)
    {
        const int rVal = meanR - imgRef.imgData[i].color.red;
        const int gVal = meanG - imgRef.imgData[i].color.green;
        const int bVal = meanB - imgRef.imgData[i].color.blue;

        varR += rVal * rVal;
        varG += gVal * gVal;
        varB += bVal * bVal;
    }

    varR /= imgSize;
    varG /= imgSize;
    varB /= imgSize;

    rVar = varR;
    gVar = varG;
    bVar = varB;
}

bool ImageFramework::calculateSecondOrderDerivatives(Image* img, int x, int y, int& Ixx, int& Ixy, int& Iyy)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    const int xxFilter[] = { 1, 0, -2, 0, 1, 4, 0, -8, 0, 4, 6, 0, -12, 0, 6, 4, 0, -8, 0, 4, 1, 0, -2, 0, 1 };
    const int yyFilter[] = { 1, 4, 6, 4, 1, 0, 0, 0, 0, 0, -2, -8, -12, -8, -2, 0, 0, 0, 0, 0, 1, 4, 6, 4, 1 };
    const int xyFilter[] = { 1, 2, 0, -2, -1, 2, 4, 0, -4, -2, 0, 0, 0, 0, 0, -2, -4, 0, 4, 2, -1, -2, 0, 2, 1 };

    if (x > 1 && y > 1 && x < imgRef.width - 2 && y < imgRef.height - 2)
    {
        int values[25];

        for (int i = 0; i < 5; i++)
        {
            const int iOff = i * 5;
            const int yOff = (y + i - 2) * imgRef.width;

            for (int j = 0; j < 5; j++)
            {
                const int idx = yOff + j - 2 + x;
                values[iOff + j] = imgRef.imgData[idx].color.red;
                values[iOff + j] += imgRef.imgData[idx].color.green;
                values[iOff + j] += imgRef.imgData[idx].color.blue;
                values[iOff + j] /= 3;
            }
        }

        Ixx = 0;
        Iyy = 0;
        Ixy = 0;

        for (int i = 0; i < 25; i++)
        {
            Ixx += values[i] * xxFilter[i];
            Iyy += values[i] * yyFilter[i];
            Ixy += values[i] * xyFilter[i];
        }

        return true;
    }
    return false;
}

bool ImageFramework::calculateFirstOrderDerivatives(Image* img, int x, int y, int& Ix, int& Iy)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    if (x > 0 && y > 0 && x < imgRef.width - 1 && y < imgRef.height - 1)
    {
        int values[9];

        for (int i = 0; i < 3; i++)
        {
            const int iOff = i * 3;
            const int yOff = (y + i - 1) * imgRef.width;

            for (int j = 0; j < 3; j++)
            {
                const int idx = yOff + j - 1 + x;
                values[iOff + j] = imgRef.imgData[idx].color.red;
                values[iOff + j] += imgRef.imgData[idx].color.green;
                values[iOff + j] += imgRef.imgData[idx].color.blue;
                values[iOff + j] /= 3;
            }
        }

        // apply convolution filter:
        // -1 0 1
        Ix = -values[0] + values[2];
        // -2 0 2
        Ix += -(values[3] << 1) + (values[5] << 1);
        // -1 0 1
        Ix += values[6] + values[8];

        // -1 -2 -1
        Iy = -values[0] - (values[1] << 1) - values[2];
        // 0 0 0
        // 1 2 1
        Iy += values[6] + (values[7] << 1) + values[8];

        return true;
    }
    return false;
}

int ImageFramework::getRegionDifference(Image* img, int x, int y, int radius)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    std::vector<Pixel*> pixelData;
    imgRef.getPixelsInShape(pixelData, x, y, radius, SQUARE_FILLED_SHAPE);
    const int widthSquare = radius * 2 + 1;
    const int widthSquareHalf = widthSquare >> 1;

    int meanUpperRegionR = 0;
    int meanLowerRegionR = 0;
    int meanRightRegionR = 0;
    int meanLeftRegionR = 0;
    int meanUpperRegionG = 0;
    int meanLowerRegionG = 0;
    int meanRightRegionG = 0;
    int meanLeftRegionG = 0;
    int meanUpperRegionB = 0;
    int meanLowerRegionB = 0;
    int meanRightRegionB = 0;
    int meanLeftRegionB = 0;

    const int upperRegionBound = ((pixelData.size() >> 1) / widthSquare) * widthSquare;
    const int pixelCount = upperRegionBound + 1;

    for (int i = 0; i <= upperRegionBound; i++)
    {
        meanUpperRegionR += pixelData[i]->color.red;
        meanUpperRegionG += pixelData[i]->color.green;
        meanUpperRegionB += pixelData[i]->color.blue;
    }
    meanUpperRegionR /= pixelCount;
    meanUpperRegionG /= pixelCount;
    meanUpperRegionB /= pixelCount;

    for (unsigned int i = widthSquare + upperRegionBound + 1; i < pixelData.size(); i++)
    {
        meanLowerRegionR += pixelData[i]->color.red;
        meanLowerRegionG += pixelData[i]->color.green;
        meanLowerRegionB += pixelData[i]->color.blue;
    }
    meanLowerRegionR /= pixelCount;
    meanLowerRegionG /= pixelCount;
    meanLowerRegionB /= pixelCount;

    for (unsigned int i = 0; i < pixelData.size(); i += widthSquare)
    {
        for (int j = 0; j < widthSquareHalf; j++)
        {
            meanLeftRegionR += pixelData[j + i]->color.red;
            meanLeftRegionG += pixelData[j + i]->color.green;
            meanLeftRegionB += pixelData[j + i]->color.blue;
        }

        for (int j = widthSquareHalf + 1; j < widthSquare; j++)
        {
            meanRightRegionR += pixelData[j + i]->color.red;
            meanRightRegionG += pixelData[j + i]->color.green;
            meanRightRegionB += pixelData[j + i]->color.blue;
        }
    }
    meanLeftRegionR /= pixelCount;
    meanRightRegionR /= pixelCount;
    meanLeftRegionG /= pixelCount;
    meanRightRegionG /= pixelCount;
    meanLeftRegionB /= pixelCount;
    meanRightRegionB /= pixelCount;

    const int meanDifLeftRight = std::abs(meanLeftRegionR - meanRightRegionR)
        + std::abs(meanLeftRegionG - meanRightRegionG) + std::abs(meanLeftRegionB - meanRightRegionB);
    const int meanDifUpperLower = std::abs(meanUpperRegionR - meanLowerRegionR)
        + std::abs(meanUpperRegionG - meanLowerRegionG) + std::abs(meanUpperRegionB - meanLowerRegionB);

    return std::max(meanDifLeftRight, meanDifUpperLower);
}

int ImageFramework::getSetPixelCountInLine(Image* img, const std::vector<bool>& binPixel, int& totalPixelCountInLine,
    int xStart, int yStart, int xEnd, int yEnd, int widthArea)
{
    int dx = abs(xEnd - xStart);
    int sx = xStart < xEnd ? 1 : -1;
    int dy = abs(yEnd - yStart);
    int sy = yStart < yEnd ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    int count = 0;

    while (1)
    {
        totalPixelCountInLine++;
        if (binPixel[xStart + yStart * widthArea] == true)
        {
            count++;
        }

        if (xStart == xEnd && yStart == yEnd)
        {
            return count;
        }

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            xStart += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            yStart += sy;
        }
    }
    return count;
}

int ImageFramework::getPointSymmetricDifferences(Image* img, int x, int y, int radius, int mean)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    int differences = 0;
    Pixel* currPixelLeft;
    Pixel* currPixelRight;
    for (int yStep = -radius; yStep <= radius; yStep++)
    {
        for (int xStep = -radius; xStep < 0; xStep++)
        {
            currPixelLeft = imgRef.getPixel(x + xStep, y + yStep);
            currPixelRight = imgRef.getPixel(x - xStep, y - yStep);
            // If image is black and white all colors (red, green, blue) are holding the same value
            if ((currPixelLeft->color.red > mean && currPixelRight->color.red < mean)
                || (currPixelLeft->color.red < mean && currPixelRight->color.red > mean))
            {
                differences++;
            }
        }
    }

    return differences;
}

int ImageFramework::getMean(const std::vector<Pixel*>& pixelData)
{
    int mean = 0;
    for (unsigned int i = 0; i < pixelData.size(); i++)
    {
        mean += pixelData[i]->color.red;
    }
    return mean / pixelData.size();
}

float ImageFramework::getVariance(const std::vector<Pixel*>& pixelData, int mean)
{
    float sumVar = 0.0;
    for (unsigned int i = 0; i < pixelData.size(); i++)
    {
        sumVar += (pixelData[i]->color.red - mean) * (pixelData[i]->color.red - mean);
    }

    return sumVar / (float)pixelData.size();
}

int ImageFramework::countPixelAlternationsRing(const std::vector<Pixel*>& pixelData, int mean, int gate)
{
    int thresholdDark = mean - gate;
    int thresholdLight = mean + gate;
    Pixel* beforePixel;
    Pixel* currPixel;

    int countOfAlternations = 0;
    for (unsigned int i = 0; i < pixelData.size(); i++)
    {
        currPixel = pixelData[i];
        if (i != 0)
        {
            beforePixel = pixelData[i - 1];
        }
        else
        {
            beforePixel = pixelData[pixelData.size() - 1];
        }

        // If image is black and white all colors (red, green, blue) are holding the same value
        if (currPixel->color.red > thresholdLight && beforePixel->color.red < thresholdDark)
        {
            countOfAlternations++;
        }

        if (currPixel->color.red < thresholdDark && beforePixel->color.red > thresholdLight)
        {
            countOfAlternations++;
        }
    }

    return countOfAlternations;
}

int ImageFramework::countPixelAlternationsRingMorphology(const std::vector<Pixel*>& pixelData, int mean,
    int& countBlackArea1, int& countBlackArea2, int& countWhiteArea1, int& countWhiteArea2)
{
    Pixel* currPixel;
    std::vector<bool> binPixel;
    int countOfAlternations = 0;
    countBlackArea1 = 0;
    countBlackArea2 = 0;
    countWhiteArea1 = 0;
    countWhiteArea2 = 0;

    for (unsigned int i = 0; i < pixelData.size(); i++)
    {
        currPixel = pixelData[i];

        // If image is black and white all colors (red, green, blue) are holding the same value
        if (currPixel->color.red > mean)
        {
            binPixel.push_back(1);
        }
        else
        {
            binPixel.push_back(0);
        }
    }

    if (pixelData.size() >= 20)
    {
        fillOnePixelHoles(binPixel);
    }

    bool currPixelBin;
    bool beforePixelBin;
    bool area1 = true;

    for (unsigned int i = 0; i < binPixel.size(); i++)
    {
        currPixelBin = binPixel[i];
        if (i != 0)
        {
            beforePixelBin = binPixel[i - 1];
        }
        else
        {
            beforePixelBin = binPixel[binPixel.size() - 1];
        }

        if (currPixelBin == 0)
        {
            if (area1 == true)
            {
                countBlackArea1++;
            }
            else
            {
                countBlackArea2++;
            }

            if (beforePixelBin == 1)
            {
                countOfAlternations++;
            }
        }
        else if (currPixelBin == 1)
        {
            if (beforePixelBin == 0)
            {
                if (area1 == true)
                    area1 = false;
                else
                    area1 = true;

                countOfAlternations++;
            }

            if (area1 == true)
            {
                countWhiteArea1++;
            }
            else
            {
                countWhiteArea2++;
            }
        }
    }

    return countOfAlternations;
}

Point ImageFramework::calculateCenter(const std::vector<Pixel*>& points)
{
    Point p;

    for (unsigned int i = 0; i < points.size(); i++)
    {
        p.x += points[i]->x;
        p.y += points[i]->y;
    }

    p.x /= points.size();
    p.y /= points.size();
    return p;
}

int ImageFramework::calculateMaxPixelVariance(const std::vector<Pixel*>& pixelData, int mean, int gate)
{
    int lightPixelVar = 0;
    int darkPixelVar = 0;
    int thresholdDark = mean - gate;
    int thresholdLight = mean + gate;

    for (unsigned int i = 0; i < pixelData.size(); i++)
    {
        if (pixelData[i]->color.red < thresholdDark)
        {
            lightPixelVar += abs(pixelData[i]->color.red - mean);
        }
        else if (pixelData[i]->color.red > thresholdLight)
        {
            darkPixelVar += abs(pixelData[i]->color.red - mean);
        }
    }

    return std::max(lightPixelVar, darkPixelVar);
}
