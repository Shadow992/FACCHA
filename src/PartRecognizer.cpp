#include "PartRecognizer.h"

PartRecognizer::PartRecognizer()
{
    // ctor
}

PartRecognizer::~PartRecognizer()
{
    // dtor
    for (unsigned int i = 0; i < parts.size(); i++)
    {
        delete parts[i];
    }
}

void PartRecognizer::addPartTemplate(const std::string& fileName, int scaling)
{
    PartTemplate* temp = new PartTemplate;
    if (temp->readTemplate(fileName, 50, scaling) == true)
    {
        parts.push_back(temp);
    }
}

void PartRecognizer::addPartTemplate(const std::vector<std::string>& fileNames, int scaling)
{
    PartTemplate* temp = new PartTemplate;
    bool success = false;

    for (unsigned int i = 0; i < fileNames.size(); i++)
    {
        if (temp->readTemplate(fileNames[i], 50, scaling) == true)
        {
            success = true;
        }
    }

    if (success == true)
    {
        parts.push_back(temp);
    }
}

int PartRecognizer::getBestMatchingPart(DistanceTransformation& distanceTransformedImg, int& bestScaling,
    float& bestScore, int x, int y, int width, int height, int threshold)
{
    int scaling;
    int bestIdx = -1;
    float maxDistanceOnePoint = threshold * 4;

    bestScore = threshold;
    bestScaling = 0;

    for (unsigned int i = 0; i < parts.size(); i++)
    {
        const float currScore = parts[i]->calculateChamferDistance(
            distanceTransformedImg, scaling, x, y, width, height, maxDistanceOnePoint);
        if (bestScore > currScore)
        {
            bestScore = currScore;
            bestScaling = scaling;
            bestIdx = i;
        }
    }

    return bestIdx;
}
