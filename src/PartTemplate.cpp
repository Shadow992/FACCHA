#include "PartTemplate.h"
#if USE_TO_STRING_PATCH == 1
// may be necessary for old (buggy) mingw compiler
template <typename T> std::string toString(const T& n)
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}
#else
template <typename T> std::string toString(const T& n) { return std::to_string(n); }
#endif
PartTemplate::PartTemplate()
{
    // ctor
}

PartTemplate::~PartTemplate()
{
    // dtor
}

float PartTemplate::calculateChamferDistance(DistanceTransformation& distanceTransformedImage, int& index, int x, int y,
    int width, int height, float maxDistanceOnePoint)
{
    float score = 0;
    float bestScore = 10000000000;
    int cPoints = 0;

    for (unsigned int j = 0; j < possibleEntities.size(); j++)
    {
        score = 0;
        cPoints = 0;
        for (unsigned int i = 0; i < possibleEntities[j].relativeXOffset.size(); i++)
        {
            const int newX = x + possibleEntities[j].relativeXOffset[i];
            const int newY = y + possibleEntities[j].relativeYOffset[i];

            if (newX > -1 && newY > -1 && newX < width && newY < height)
            {
                const int idx = newX + newY * width;
                if (possibleEntities[j].direction[i] == 1 || possibleEntities[j].direction[i] == 5)
                {
                    score += std::min(distanceTransformedImage.gradientUpDownDT[idx], maxDistanceOnePoint);
                }
                else if (possibleEntities[j].direction[i] == 3 || possibleEntities[j].direction[i] == 7)
                {
                    score += std::min(distanceTransformedImage.gradientLeftRightDT[idx], maxDistanceOnePoint);
                }
                else
                {
                    score += std::min(std::min(distanceTransformedImage.gradientLeftRightDT[idx], maxDistanceOnePoint),
                        distanceTransformedImage.gradientUpDownDT[idx]);
                }
                cPoints++;
            }
            else
            {
                score += maxDistanceOnePoint;
            }
        }

        score /= static_cast<float>(cPoints);
        if (bestScore > score)
        {
            bestScore = score;
            index = j;
        }
    }

    return bestScore;
}

bool PartTemplate::readTemplate(const std::string& partFile, unsigned char threshold, int numberOfScales)
{
    Image* img;
    Image* origImg = ImageFramework::readFile(partFile);

    if (origImg == nullptr)
    {
        return false;
    }

    for (int i = 0; i < numberOfScales; i++)
    {
        TemplateEntity part;
        part.scaling = i;

        if (i != 0)
        {
            img = ImageFramework::resizeImageNearestNeighbour(
                origImg, origImg->width * i / numberOfScales, origImg->height * i / numberOfScales);
        }
        else
        {
            img = origImg;
        }

        bool firstPoint = true;
        int relativeX = 0;
        int relativeY = 0;

        part.centerX = img->width >> 1;
        part.centerY = img->height >> 1;
        part.width = img->width;
        part.height = img->height;

        for (int y = 0; y < img->height; y++)
        {
            const int yOffset = y * img->width;
            for (int x = 0; x < img->width; x++)
            {
                const int idx = yOffset + x;

                if (img->imgData[idx].color.red < threshold || img->imgData[idx].color.green < threshold
                    || img->imgData[idx].color.blue < threshold)
                {
                    if (firstPoint)
                    {
                        firstPoint = false;
                        relativeX = x;
                        relativeY = y;
                        part.centerX -= relativeX;
                        part.centerY -= relativeY;
                    }

                    part.relativeXOffset.push_back(x - relativeX);
                    part.relativeYOffset.push_back(y - relativeY);

                    if (img->imgData[idx].color.red > threshold)
                    {
                        part.direction.push_back(5);
                    }
                    else if (img->imgData[idx].color.blue > threshold)
                    {
                        part.direction.push_back(3);
                    }
                    else
                    {
                        part.direction.push_back(-1);
                    }
                }
            }
        }

        possibleEntities.push_back(part);
    }

    return true;
}
