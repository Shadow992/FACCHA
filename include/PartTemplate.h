#ifndef PARTTEMPLATE_H
#define PARTTEMPLATE_H

#include "DistanceTransformer.h"
#include "Image.h"
#include "ImageLoader.h"
#include "ImageManipulation.h"
#include <vector>

struct TemplateEntity
{
    std::vector<int> relativeXOffset;
    std::vector<int> relativeYOffset;
    std::vector<int> direction;

    int scaling;
    int centerX;
    int centerY;
    int width;
    int height;
};

class PartTemplate
{
public:
    PartTemplate();
    virtual ~PartTemplate();
    float calculateChamferDistance(DistanceTransformation& distanceTransformedImage, int& index, int x, int y,
        int width, int height, float maxDistanceOnePoint = 1000000);
    bool readTemplate(const std::string& partFile, unsigned char threshold, int numberOfScales = 1);

    std::vector<TemplateEntity> possibleEntities;

protected:
private:
};

#endif // PARTTEMPLATE_H
