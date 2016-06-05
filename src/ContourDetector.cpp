#include "ContourDetector.h"

void ContourDetector::performHysteresisRec(std::vector<OrientedGradient>& inputOG, int widthOG, int heightOG,
    std::vector<bool>& visited, int x, int y, int low, int high, int minPts, int& countEdgePoints)
{
    if (x >= widthOG || x < 0 || y >= heightOG || y < 0)
    {
        return;
    }

    const int currIdx = x + y * widthOG;
    if (visited[currIdx] == true)
    {
        return;
    }

    visited[currIdx] = true;
    if (inputOG[currIdx].gradientMagnitude > low)
    {
        countEdgePoints++;
        inputOG[currIdx].gradientMagnitude = high + 10;
        for (int yNeighbour = -1; yNeighbour < 2; yNeighbour++)
        {
            for (int xNeighbour = -1; xNeighbour < 2; xNeighbour++)
            {
                performHysteresisRec(inputOG, widthOG, heightOG, visited, x + xNeighbour, y + yNeighbour, low, high,
                    minPts, countEdgePoints);
            }
        }

        if (countEdgePoints < minPts)
        {
            inputOG[currIdx].gradientMagnitude = -1;
            inputOG[currIdx].direction = -1;
        }
    }
}

void ContourDetector::performHysteresis(
    std::vector<OrientedGradient>& inputOG, int widthOG, int heightOG, int low, int high, int minPts)
{
    std::vector<bool> visited(heightOG * widthOG, false);
    for (int y = 0; y < heightOG; y++)
    {
        const int yOffset = y * widthOG;
        for (int x = 0; x < widthOG; x++)
        {
            int countEdgePoints = 0;
            const int currIdx = yOffset + x;
            if (visited[currIdx] == false)
            {
                if (inputOG[currIdx].gradientMagnitude > high)
                {
                    performHysteresisRec(inputOG, widthOG, heightOG, visited, x, y, low, high, minPts, countEdgePoints);
                }

                if (countEdgePoints < minPts)
                {
                    inputOG[currIdx].gradientMagnitude = -1;
                    inputOG[currIdx].direction = -1;
                }
            }
        }
    }
}

void ContourDetector::performNMPOnOG(std::vector<OrientedGradient>& inputOG, int widthOG, int heightOG)
{
    std::vector<int> foundEdgePoints;
    foundEdgePoints.reserve(widthOG);
    std::vector<bool> visitedPoints(widthOG * heightOG, false);

    // Index 0: Top left corner
    // Index 1: Top
    const int stepX[] = { 1, 0, 1, 1, 1, 0, 1, 1 };
    const int stepY[] = { 1, 1, -1, 0, 1, 1, -1, 0 };

    for (int y = 1; y < heightOG - 1; y++)
    {
        for (int x = 1; x < widthOG - 1; x++)
        {
            const int currIdx = x + y * widthOG;

            if (inputOG[currIdx].direction > -1)
            {
                const int currDirection = inputOG[currIdx].direction;

                int nextX = x + stepX[currDirection];
                int nextY = y + stepY[currDirection];
                int nextIdx = nextX + nextY * widthOG;

                if (inputOG[currIdx].gradientMagnitude < inputOG[nextIdx].gradientMagnitude)
                {
                    foundEdgePoints.push_back(currIdx);
                }
                else
                {
                    nextX = x - stepX[currDirection];
                    nextY = y - stepY[currDirection];
                    nextIdx = nextX + nextY * widthOG;
                    if (inputOG[currIdx].gradientMagnitude < inputOG[nextIdx].gradientMagnitude)
                    {
                        foundEdgePoints.push_back(currIdx);
                    }
                }
            }
        }
    }

    for (unsigned int i = 0; i < foundEdgePoints.size(); i++)
    {
        inputOG[foundEdgePoints[i]].direction = -1;
        inputOG[foundEdgePoints[i]].gradientMagnitude = -1;
    }
}

std::vector<OrientedGradient> ContourDetector::getOrientedGradients(Image& img, int radius, int threshold, bool bigger)
{
    std::vector<OrientedGradient> vectorOG(img.width * img.height);

    IntegralImage intImg;
    intImg.calculate(img);

    for (int y = radius + 1; y < img.height - radius - 1; y++)
    {
        const int yOffset = y * img.width;
        for (int x = radius + 1; x < img.width - radius - 1; x++)
        {
            const int currIdx = yOffset + x;

            vectorOG[currIdx].gradientMagnitude = intImg.getRegionDifference(x, y, radius, vectorOG[currIdx].direction);
            if (bigger == true && vectorOG[currIdx].gradientMagnitude < threshold)
            {
                vectorOG[currIdx].gradientMagnitude = -1;
                vectorOG[currIdx].direction = -1;
            }
            else if (bigger == false && vectorOG[currIdx].gradientMagnitude > threshold)
            {
                vectorOG[currIdx].gradientMagnitude = -1;
                vectorOG[currIdx].direction = -1;
            }
        }
    }

    return vectorOG;
}

std::vector<OrientedGradient> ContourDetector::extractContours(
    Image& img, int radius, int threshold, int hysteresisThreshLow, int hysteresisThreshHigh, int minPts)
{
    std::vector<OrientedGradient> extractedGradients = getOrientedGradients(img, radius, threshold);
    performNMPOnOG(extractedGradients, img.width, img.height);
    performHysteresis(extractedGradients, img.width, img.height, hysteresisThreshLow, hysteresisThreshHigh, minPts);

    return extractedGradients;
}

std::vector<Moment> ContourDetector::calculateMomentForRegion(Image& img, int x, int y, int radius)
{
    std::vector<Moment> m(6);
    for (unsigned int i = 0; i < m.size(); i++)
    {
        m[i].momentR = 0;
        m[i].momentG = 0;
        m[i].momentB = 0;
    }

    if (x >= radius && y >= radius && x < img.width - radius && y < img.height - radius)
    {

        for (int i = -radius; i <= radius; i++)
        {
            const int yOff = (y + i) * img.width;

            for (int j = -radius; j <= radius; j++)
            {
                const int idx = yOff + j + x;
                // m00
                m[0].momentR += img.imgData[idx].color.red;
                m[0].momentG += img.imgData[idx].color.green;
                m[0].momentB += img.imgData[idx].color.blue;

                // m10
                m[1].momentR += img.imgData[idx].color.red * j;
                m[1].momentG += img.imgData[idx].color.green * j;
                m[1].momentB += img.imgData[idx].color.blue * j;

                // m01
                m[2].momentR += img.imgData[idx].color.red * i;
                m[2].momentG += img.imgData[idx].color.green * i;
                m[2].momentB += img.imgData[idx].color.blue * i;

                // m11
                m[3].momentR += img.imgData[idx].color.red * i * j;
                m[3].momentG += img.imgData[idx].color.green * i * j;
                m[3].momentB += img.imgData[idx].color.blue * i * j;

                // m20
                m[4].momentR += img.imgData[idx].color.red * j * j;
                m[4].momentG += img.imgData[idx].color.green * j * j;
                m[4].momentB += img.imgData[idx].color.blue * j * j;

                // m02
                m[5].momentR += img.imgData[idx].color.red * i * i;
                m[5].momentG += img.imgData[idx].color.green * i * i;
                m[5].momentB += img.imgData[idx].color.blue * i * i;
            }
        }
    }
    return m;
}
