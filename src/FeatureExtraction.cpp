#include "FeatureExtraction.h"

#include <cassert>

std::vector<float> ImageFramework::calcHistogram(Image* img, int accuracy, int xStart, int yStart, int w, int h)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    std::vector<float> histogram;
    int histogramAccuracy = 2;
    if (accuracy <= 4)
    {
        histogramAccuracy = 2;
    }
    else if (accuracy <= 8)
    {
        histogramAccuracy = 3;
    }
    else if (accuracy <= 16)
    {
        histogramAccuracy = 4;
    }
    else
    {
        histogramAccuracy = 5;
    }

    if (w == -1)
    {
        w = imgRef.width - xStart;
    }

    if (h == -1)
    {
        h = imgRef.height - yStart;
    }

    if (yStart + h > imgRef.height)
    {
        h = imgRef.height - yStart;
    }

    if (w + xStart > imgRef.width)
    {
        w = imgRef.width - xStart;
    }

    int sizeOfHistogram = ((1 << histogramAccuracy) << histogramAccuracy) << histogramAccuracy;
    histogram.resize(sizeOfHistogram);
    for (int i = 0; i < sizeOfHistogram; i++)
    {
        histogram[i] = 0;
    }

    int toShift = 8 - histogramAccuracy;
    int blueShift = histogramAccuracy + histogramAccuracy;

    for (int y = yStart; y < yStart + h; y++)
    {
        for (int x = xStart; x < xStart + w; x++)
        {
            Pixel* p = imgRef.getPixel(x, y);
            int redBins = p->color.red >> toShift;
            int greenBins = (p->color.green >> toShift) << histogramAccuracy;
            int blueBins = (p->color.blue >> toShift) << blueShift;

            histogram[redBins + greenBins + blueBins]++;
        }
    }

    float sampledPixels = w * h;

    for (int i = 0; i < sizeOfHistogram; i++)
    {
        if (histogram[i] != 0)
        {
            histogram[i] = sqrt(histogram[i] / sampledPixels);
        }
    }

    return histogram;
}

void ImageFramework::calculateHistogramFeatureVectorOfLB(std::vector<float>& featureVector,
    const std::vector<ImageLBP>& inputLBP, int xStart, int yStart, int widthLBP,
    int classifactionWindowWidth, int classificationWindowHeight, int histogramWidth, int histogramHeight,
    int valueBins, int archBins, float overlapHistogram, int* mappingValuesToBins, int* mappingArchesToBins)
{
    int defaultMapping[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    if (mappingValuesToBins == nullptr)
    {
        mappingValuesToBins = defaultMapping;
    }

    if (mappingArchesToBins == nullptr)
    {
        mappingArchesToBins = defaultMapping;
    }

    const int histogramYStep = (float)histogramHeight * (1.0 - overlapHistogram);
    const int histogramXStep = (float)histogramWidth * (1.0 - overlapHistogram);

    const int newHeight = classificationWindowHeight - histogramHeight + 1;
    const int newWidth = classifactionWindowWidth - histogramWidth + 1;

    const int histogramSizeTotal = valueBins * archBins * 2;
    const int histogramSizeLBP = valueBins * archBins * 2;
    const int histogramSizeHalf = valueBins * archBins;
    const int histogramsPerRow = classifactionWindowWidth / histogramXStep;
    const int histogramsPerCol = classificationWindowHeight / histogramYStep;
    const int sizeBefore = featureVector.size();
    featureVector.resize(histogramSizeTotal * histogramsPerRow * histogramsPerCol + sizeBefore, 0.0);

    const int endPixelX = newWidth + xStart;
    const int endPixelY = newHeight + yStart;

    int currHistogram = sizeBefore;
    float sumWeight = 0;

    for (int y = yStart; y < endPixelY; y += histogramYStep)
    {
        for (int x = xStart; x < endPixelX; x += histogramXStep)
        {
            sumWeight = 0;
            for (int yHistogram = 0; yHistogram < histogramHeight; yHistogram++)
            {
                const int yOffset = (yHistogram + y) * widthLBP;
                for (int xHistogram = 0; xHistogram < histogramWidth; xHistogram++)
                {
                    const int currIdx = xHistogram + yOffset + x;
                    if (inputLBP[currIdx].length > 0)
                    {
                        //const int highestGradientColor = inputLBP[currIdx].highestGradientColor;
                        const int archDirection = inputLBP[currIdx].archDirection;
                        const int val = inputLBP[currIdx].length;
                        const float currWeight = inputLBP[currIdx].gradientMagnitudeSquare;
                        const int idx = currHistogram + mappingArchesToBins[archDirection] + archBins * mappingValuesToBins[val];

                        featureVector[idx] += currWeight;

                        if (archDirection >= 4)
                        {
                            featureVector[idx + histogramSizeHalf] -= currWeight;
                        }
                        else
                        {
                            featureVector[idx + histogramSizeHalf] += currWeight;
                        }
                        sumWeight += currWeight;
                    }
                }
            }

            if (sumWeight != 0)
            {
                for (int i = 0; i < histogramSizeHalf; i++)
                {
                    featureVector[i + currHistogram] /= sumWeight;
                }

                for (int i = histogramSizeHalf; i < histogramSizeLBP; i++)
                {
                    featureVector[i + currHistogram] = std::abs(featureVector[i + currHistogram] / sumWeight);
                }
            }

            currHistogram += histogramSizeTotal;
        }
    }
}

std::vector<ImageLBP> ImageFramework::calculateLBPOfImage(
    Image* img, int radius, int threshold, int regionMeanThreshold, bool fillHoles)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    std::vector<bool> extractedLBP;
    std::vector<Pixel*> extractedPixel;
    std::vector<ImageLBP> outputLBP;
    outputLBP.reserve((imgRef.height - radius) * (imgRef.width - radius));

    const int newWidth = imgRef.width - radius;
    const int newHeight = imgRef.height - radius;
    const int regionRadius = radius * 3;

    const int idxTopPixel = radius / 2;
    const int idxRightPixel = radius * 3;
    const int idxBotPixel = radius * 5;
    const int idxLeftPixel = radius * 7;

    for (int y = radius; y < newHeight; y++)
    {
        for (int x = radius; x < newWidth; x++)
        {
            ImageLBP output[3];
            extractedPixel.clear();

            Pixel* currPixel = imgRef.getPixel(x, y);

            imgRef.getPixelsInShape(extractedPixel, x, y, radius, SQUARE_SHAPE);

            for (int colorIdx = 0; colorIdx < 3; colorIdx++)
            {
                extractedLBP.clear();
                for (unsigned int i = 0; i < extractedPixel.size(); i++)
                {
                    const int usedColorCurr[] = { currPixel->color.red, currPixel->color.green, currPixel->color.blue };
                    const int usedColorExtracted[] = { extractedPixel[i]->color.red, extractedPixel[i]->color.green,
                        extractedPixel[i]->color.blue };
                    if (usedColorCurr[colorIdx] - usedColorExtracted[colorIdx] > threshold)
                    {
                        extractedLBP.push_back(true);
                    }
                    else
                    {
                        extractedLBP.push_back(false);
                    }
                }

                const Pixel* topPixel = extractedPixel[idxTopPixel];
                const Pixel* rightPixel = extractedPixel[idxRightPixel];
                const Pixel* botPixel = extractedPixel[idxBotPixel];
                const Pixel* leftPixel = extractedPixel[idxLeftPixel];

                if (fillHoles)
                {
                    fillOnePixelHoles(extractedLBP);
                }

                bool currPixelBin;
                bool beforePixelBin;
                int countSetBits = 0;
                int countOfAlternations = 0;
                int startSetBits = -1;
                int endSetBits = -1;

                for (unsigned int i = 0; i < extractedLBP.size(); i++)
                {
                    currPixelBin = extractedLBP[i];
                    if (i != 0)
                    {
                        beforePixelBin = extractedLBP[i - 1];
                    }
                    else
                    {
                        beforePixelBin = extractedLBP[extractedLBP.size() - 1];
                    }

                    if (currPixelBin == 1)
                    {
                        countSetBits++;
                        if (beforePixelBin == 0)
                        {
                            if (endSetBits == -1)
                            {
                                endSetBits = i;
                            }
                            countOfAlternations++;
                        }
                    }
                    else if (currPixelBin == 0 && beforePixelBin == 1)
                    {
                        if (startSetBits == -1)
                        {
                            startSetBits = i;
                        }
                        countOfAlternations++;
                    }
                }

                if (countOfAlternations == 2)
                {
                    if (colorIdx == 0)
                    {
                        output[colorIdx].gradientX = rightPixel->color.red - leftPixel->color.red;
                        output[colorIdx].gradientY = topPixel->color.red - botPixel->color.red;
                    }
                    else if (colorIdx == 1)
                    {
                        output[colorIdx].gradientX = rightPixel->color.green - leftPixel->color.green;
                        output[colorIdx].gradientY = topPixel->color.green - botPixel->color.green;
                    }
                    else
                    {
                        output[colorIdx].gradientX = rightPixel->color.blue - leftPixel->color.blue;
                        output[colorIdx].gradientY = topPixel->color.blue - botPixel->color.blue;
                    }

                    output[colorIdx].gradientMagnitudeSquare = output[colorIdx].gradientX * output[colorIdx].gradientX
                        + output[colorIdx].gradientY * output[colorIdx].gradientY;
                    output[colorIdx].length = countSetBits;

                    if (startSetBits != -1)
                    {
                        output[colorIdx].archDirection = (startSetBits + (countSetBits >> 1)) % (radius << 3);
                    }
                    else if (endSetBits != -1)
                    {
                        output[colorIdx].archDirection = (endSetBits - (countSetBits >> 1)) % (radius << 3);
                    }
                    else
                    {
                        output[colorIdx].archDirection = 0;
                    }
                }
                else
                {
                    output[colorIdx].length = -1;
                    output[colorIdx].archDirection = -1;
                    output[colorIdx].gradientMagnitudeSquare = -1;
                }
            }

            int bestColor = 0;
            if (output[0].gradientMagnitudeSquare > output[1].gradientMagnitudeSquare)
            {
                if (output[0].gradientMagnitudeSquare > output[2].gradientMagnitudeSquare)
                {
                    bestColor = 0;
                }
                else
                {
                    bestColor = 2;
                }
            }
            else
            {
                if (output[1].gradientMagnitudeSquare > output[2].gradientMagnitudeSquare)
                {
                    bestColor = 1;
                }
                else
                {
                    bestColor = 2;
                }
            }

            if (output[bestColor].length != -1)
            {
                int regionDifference = regionMeanThreshold + 1;
                if (regionMeanThreshold > 0 && x > regionRadius && y > regionRadius && x < imgRef.width - regionRadius
                    && y < imgRef.height - regionRadius)
                {
                    regionDifference = getRegionDifference(img, x, y, regionRadius);
                }

                if (regionDifference <= regionMeanThreshold)
                {
                    output[bestColor].length = -1;
                    output[bestColor].archDirection = -1;
                    output[bestColor].gradientMagnitudeSquare = -1;
                }
            }

            output[bestColor].highestGradientColor = bestColor;
            outputLBP.push_back(output[bestColor]);
        }
    }
    return outputLBP;
}
