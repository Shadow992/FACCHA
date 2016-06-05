#ifndef PARTRECOGNIZER_H
#define PARTRECOGNIZER_H

#include <vector>
#include <string>
#include "Image.h"
#include "PartTemplate.h"

class PartRecognizer
{
    public:
        PartRecognizer();
        virtual ~PartRecognizer();
        void addPartTemplate(const std::vector<std::string>& fileNames, int scaling=1);
        void addPartTemplate(const std::string& fileName, int scaling=1);
        int getBestMatchingPart(DistanceTransformation& distanceTransformedImg, int& bestScaling, float& bestScore, int x, int y, int width, int height, int threshold=100000000);

    protected:
    private:
        std::vector<PartTemplate*> parts;
};




#endif // PARTRECOGNIZER_H
