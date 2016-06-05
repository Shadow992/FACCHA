#include "ImageSearch.h"

#include <cassert>

Pixel* ImageFramework::searchForImage(
    Image* img, int startX, int startY, const Image* searchImage, unsigned char variance)
{
    assert(img != nullptr);
    Image& imgRef = *img;
    const Image& searchImageRef = *searchImage;

    int searchImageWidth = searchImageRef.width;
    int searchImageHeight = searchImageRef.height;

    for (int y = 0; y < imgRef.height; y++)
    {
        for (int x = 0; x < imgRef.width; x++)
        {
            bool match = true;

            for (int searchY = 0; searchY < searchImageHeight; searchY++)
            {
                for (int searchX = 0; searchX < searchImageWidth; searchX++)
                {
                    if (x + searchX < imgRef.width && y + searchY < imgRef.height
                        && imgRef.getPixel(x + searchX, y + searchY)
                                ->colorEquals(searchImageRef.getPixel(searchX, searchY), variance)
                            == false)
                    {
                        match = false;
                        break;
                    }
                }
                if (match == false)
                {
                    break;
                }
            }
            if (match == true)
            {
                return imgRef.getPixel(x, y);
            }
        }
    }
    return nullptr;
}

bool ImageFramework::equalsImage(Image* img, int x, int y, const Image* searchImage, unsigned char variance)
{
    assert(img != nullptr);
    Image& imgRef = *img;
    const Image& searchImageRef = *searchImage;

    for (int searchY = 0; searchY < searchImageRef.height; searchY++)
    {
        for (int searchX = 0; searchX < searchImageRef.width; searchX++)
        {
            if (x + searchX < imgRef.width && y + searchY < imgRef.height
                && imgRef.getPixel(x + searchX, y + searchY)
                        ->colorEquals(searchImageRef.getPixel(searchX, searchY), variance)
                    == false)
            {
                return false;
            }
        }
    }
    return true;
}

Pixel* ImageFramework::searchForImageNextToPosition(
    Image* img, int x, int y, const Image* searchImage, unsigned char variance)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    for (int radius = 0;; radius++)
    {
        if (y + radius >= imgRef.height && y - radius < 0 && x + radius >= imgRef.width && x - radius < 0)
            break;

        for (int nearestX = x - radius; nearestX <= x + radius; nearestX++)
        {
            if (y - radius >= 0)
            {
                if (nearestX >= 0 && nearestX < imgRef.width
                    && equalsImage(img, nearestX, y - radius, searchImage, variance) == true)
                {
                    return imgRef.getPixel(nearestX, y - radius);
                }
            }

            if (y + radius < imgRef.height)
            {
                if (nearestX >= 0 && nearestX < imgRef.width
                    && equalsImage(img, nearestX, y + radius, searchImage, variance) == true)
                {

                    return imgRef.getPixel(nearestX, y + radius);
                }
            }
        }

        for (int nearestY = y - radius; nearestY <= y + radius; nearestY++)
        {
            if (x + radius < imgRef.width)
            {
                if (nearestY >= 0 && nearestY < imgRef.height
                    && equalsImage(img, x + radius, nearestY, searchImage, variance) == true)
                {
                    return imgRef.getPixel(x + radius, nearestY);
                }
            }

            if (x - radius >= 0)
            {
                if (nearestY >= 0 && nearestY < imgRef.height
                    && equalsImage(img, x - radius, nearestY, searchImage, variance) == true)
                {
                    return imgRef.getPixel(x - radius, nearestY);
                }
            }
        }
    }

    return nullptr;
}

Pixel* ImageFramework::searchForPixel(
    Image* img, int startX, int startY, const Color& searchPixel, unsigned char variance)
{
    return searchForPixel(img, startX, startY, searchPixel.red, searchPixel.green, searchPixel.blue, variance);
}

Pixel* ImageFramework::searchForPixel(Image* img, int startX, int startY, unsigned char red, unsigned char green,
    unsigned char blue, unsigned char variance)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    for (int y = 0; y < imgRef.height; y++)
    {
        for (int x = 0; x < imgRef.width; x++)
        {
            if (imgRef.getPixel(x, y)->colorEquals(red, green, blue, variance))
            {
                return imgRef.getPixel(x, y);
            }
        }
    }
    return nullptr;
}

Pixel* ImageFramework::searchPixelNextToPosition(
    Image* img, int x, int y, const Color& searchPixel, unsigned char variance)
{
    return searchPixelNextToPosition(img, x, y, searchPixel.red, searchPixel.green, searchPixel.blue, variance);
}

Pixel* ImageFramework::searchPixelNextToPosition(
    Image* img, int x, int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char variance)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    for (int radius = 0;; radius++)
    {
        if (y + radius >= imgRef.height && y - radius < 0 && x + radius >= imgRef.width && x - radius < 0)
            break;

        for (int nearestX = x - radius; nearestX <= x + radius; nearestX++)
        {
            if (y - radius >= 0)
            {
                if (nearestX >= 0 && nearestX < imgRef.width
                    && imgRef.getPixel(nearestX, y - radius)->colorEquals(red, green, blue, variance))
                {
                    return imgRef.getPixel(nearestX, y - radius);
                }
            }

            if (y + radius < imgRef.height)
            {
                if (nearestX >= 0 && nearestX < imgRef.width
                    && imgRef.getPixel(nearestX, y + radius)->colorEquals(red, green, blue, variance))
                {

                    return imgRef.getPixel(nearestX, y + radius);
                }
            }
        }

        for (int nearestY = y - radius; nearestY <= y + radius; nearestY++)
        {
            if (x + radius < imgRef.width)
            {
                if (nearestY >= 0 && nearestY < imgRef.height
                    && imgRef.getPixel(x + radius, nearestY)->colorEquals(red, green, blue, variance))
                {
                    return imgRef.getPixel(x + radius, nearestY);
                }
            }

            if (x - radius >= 0)
            {
                if (nearestY >= 0 && nearestY < imgRef.height
                    && imgRef.getPixel(x - radius, nearestY)->colorEquals(red, green, blue, variance))
                {
                    return imgRef.getPixel(x - radius, nearestY);
                }
            }
        }
    }

    return nullptr;
}

float ImageFramework::checkHistogramDifferences(
    const std::vector<float>& histogram, const std::vector<float>& histogram2)
{
    int sizeOfHistogram = histogram.size();

    // use Hellinger distance to calculate differences of 2 given datasets
    float differences = 0.0;
    for (int i = 0; i < sizeOfHistogram; i++)
    {
        float squareRootDif = histogram[i] - histogram2[i];
        squareRootDif *= squareRootDif;
        differences += squareRootDif;
    }
    differences = sqrt(differences) / SQUARE_OF_2;

    return differences;
}

float ImageFramework::findSimilarImageHillClimbing(Image* img, int startX, int startY, int w, int h, Image* checkImage,
    int accuracy, float overlap, int& bestX, int& bestY)
{
    assert(img != nullptr);
    assert(checkImage != nullptr);
    Image& imgRef = *img;
    Image& checkImageRef = *checkImage;

    if (w == -1)
    {
        w = imgRef.width;
    }

    if (h == -1)
    {
        h = imgRef.height;
    }

    int widthToCheck = checkImageRef.width;
    int heightToCheck = checkImageRef.height;

    auto histogramSearch = calcHistogram(checkImage, accuracy, 0, 0, widthToCheck, heightToCheck);

    int freeSpaceX = (float)widthToCheck / overlap;
    int freeSpaceY = (float)heightToCheck / overlap;

    float currFitness = 1000000000000000;
    float bestFitness = 100000;

    for (int y = startY + freeSpaceY; y < h; y += freeSpaceY)
    {
        for (int x = freeSpaceX + startX; x < w; x += freeSpaceX)
        {
            // calculate fitness
            auto histogramSrc = calcHistogram(img, accuracy, x, y, widthToCheck, heightToCheck);
            currFitness = checkHistogramDifferences(histogramSearch, histogramSrc);

            if (bestFitness > currFitness)
            {
                bestFitness = currFitness;
                bestX = x;
                bestY = y;
            }
        }
    }

    findBetterSolution(img, histogramSearch, bestX, bestY, w, h, checkImage, accuracy, bestX, bestY, bestFitness);

    return bestFitness;
}

void ImageFramework::findBetterSolution(Image* img, std::vector<float>& histogramSearch, int x, int y, int w, int h,
    Image* checkImage, int accuracy, int& bestX, int& bestY, float& fitness)
{
    assert(checkImage != nullptr);
    Image& imgRef = *img;
    Image& checkImageRef = *checkImage;

    int widthToCheck = checkImageRef.width;
    int heightToCheck = checkImageRef.height;
    float currFitness = 111111;
    bool foundBetter = true;
    int radius = 1;

    while (foundBetter == true)
    {
        foundBetter = false;
        if (y + radius >= h && y - radius < 0 && x + radius >= w && x - radius < 0)
            break;

        for (int nearestX = x - radius; nearestX <= x + radius; nearestX++)
        {
            if (y - radius >= 0)
            {
                if (nearestX >= 0 && nearestX < w)
                {
                    auto histogramSrc
                        = calcHistogram(checkImage, accuracy, nearestX, y - radius, widthToCheck, heightToCheck);
                    currFitness = checkHistogramDifferences(histogramSearch, histogramSrc);
                    if (currFitness < fitness)
                    {
                        fitness = currFitness;
                        bestX = nearestX;
                        bestY = y - radius;
                        x = bestX;
                        y = bestY;
                        foundBetter = true;
                    }
                }
            }

            if (y + radius < h)
            {
                if (nearestX >= 0 && nearestX < w)
                {
                    auto histogramSrc
                        = calcHistogram(checkImage, accuracy, nearestX, y + radius, widthToCheck, heightToCheck);
                    currFitness = checkHistogramDifferences(histogramSearch, histogramSrc);
                    if (currFitness < fitness)
                    {
                        fitness = currFitness;
                        bestX = nearestX;
                        bestY = y + radius;
                        x = bestX;
                        y = bestY;
                        foundBetter = true;
                    }
                }
            }
        }

        for (int nearestY = y - radius; nearestY <= y + radius; nearestY++)
        {
            if (x + radius < w)
            {
                if (nearestY >= 0 && nearestY < h)
                {
                    auto histogramSrc
                        = calcHistogram(checkImage, accuracy, x + radius, nearestY, widthToCheck, heightToCheck);
                    currFitness = checkHistogramDifferences(histogramSearch, histogramSrc);
                    if (currFitness < fitness)
                    {
                        fitness = currFitness;
                        bestX = x + radius;
                        bestY = nearestY;
                        x = bestX;
                        y = bestY;
                        foundBetter = true;
                    }
                }
            }

            if (x - radius >= 0)
            {
                if (nearestY >= 0 && nearestY < h)
                {
                    auto histogramSrc
                        = calcHistogram(checkImage, accuracy, x - radius, nearestY, widthToCheck, heightToCheck);
                    currFitness = checkHistogramDifferences(histogramSearch, histogramSrc);
                    if (currFitness < fitness)
                    {
                        fitness = currFitness;
                        bestX = x - radius;
                        bestY = nearestY;
                        x = bestX;
                        y = bestY;
                        foundBetter = true;
                    }
                }
            }
        }
    }
}
