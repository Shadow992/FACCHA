#include "DistanceTransformer.h"

#define INF 1E20


DistanceTransformer::DistanceTransformer()
{
    //ctor
}

DistanceTransformer::~DistanceTransformer()
{
    //dtor
}


void DistanceTransformer::calculateDistanceTransformation1D(const std::vector<float>& f, std::vector<float>& d)
{
    const int n=f.size();

    std::vector<float> boundaryLocation(n+1);
    std::vector<int> parabolaLocation(n);
    int idxRightmostParabola = 0;

    // Compute lower envelope
    parabolaLocation[0] = 0;
    boundaryLocation[0] = -INF;
    boundaryLocation[1] = +INF;
    for (int q = 1; q < n; q++)
    {
        const float part1=f[q]+q*q;

        float s  = (part1-(f[parabolaLocation[idxRightmostParabola]]+parabolaLocation[idxRightmostParabola]*parabolaLocation[idxRightmostParabola]))/(2.0*q-2.0*parabolaLocation[idxRightmostParabola]);
        while (s <= boundaryLocation[idxRightmostParabola])
        {
            idxRightmostParabola--;
            s  = (part1-(f[parabolaLocation[idxRightmostParabola]]+parabolaLocation[idxRightmostParabola]*parabolaLocation[idxRightmostParabola]))/(2.0*q-2.0*parabolaLocation[idxRightmostParabola]);
        }
        idxRightmostParabola++;
        parabolaLocation[idxRightmostParabola] = q;
        boundaryLocation[idxRightmostParabola] = s;
        boundaryLocation[idxRightmostParabola+1] = +INF;
    }

    // Fill in values of distance transform
    idxRightmostParabola = 0;
    for (int q = 0; q < n; q++)
    {
        while (boundaryLocation[idxRightmostParabola+1] < q)
        {
            idxRightmostParabola++;
        }

        const float part=q-parabolaLocation[idxRightmostParabola];
        d[q] = part*part + f[parabolaLocation[idxRightmostParabola]];
    }

}

DistanceTransformation DistanceTransformer::calculateDistanceTransformation(const std::vector<OrientedGradient>& inputOG, int width, int height)
{
    DistanceTransformation dt;
    dt.gradientLeftRightDT.resize(width*height);
    dt.gradientUpDownDT.resize(width*height);


    for(unsigned int i=0;i<inputOG.size();i++)
    {
        if(inputOG[i].direction==1 || inputOG[i].direction==5)
        {
            dt.gradientUpDownDT[i] = 0.0;
        }
        else
        {
            dt.gradientUpDownDT[i] = INF;
        }

        if(inputOG[i].direction==3 || inputOG[i].direction==7)
        {
            dt.gradientLeftRightDT[i] = 0.0;
        }
        else
        {
            dt.gradientLeftRightDT[i] = INF;
        }
    }
    calculateDistanceTransformation(dt.gradientUpDownDT, width, height);
    calculateDistanceTransformation(dt.gradientLeftRightDT, width, height);

    return dt;
}

std::vector<float> DistanceTransformer::calculateDistanceTransformation(const std::vector<bool>& binaryImg, int width, int height)
{
    std::vector<float> calculatedDT(width*height);

    for(unsigned int i=0;i<binaryImg.size();i++)
    {
        if(binaryImg[i]==true)
        {
            calculatedDT[i] = 0.0;
        }
        else
        {
            calculatedDT[i] = INF;
        }
    }
    calculateDistanceTransformation(calculatedDT, width, height);
    return calculatedDT;
}

void DistanceTransformer::calculateDistanceTransformation(std::vector<float>& calculatedDT, int width, int height)
{
    // transform along columns
    std::vector<float> calculatedDTTemp(height);
    std::vector<float> calculatedDTTemp2(height);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            const int idx=y*width+x;
            calculatedDTTemp[y] = calculatedDT[idx];
        }

        calculateDistanceTransformation1D(calculatedDTTemp, calculatedDTTemp2);

        for (int y = 0; y < height; y++)
        {
            const int idx=y*width+x;
            calculatedDT[idx] = calculatedDTTemp2[y];
        }
    }

    calculatedDTTemp.resize(width);
    calculatedDTTemp2.resize(width);

    // transform along rows
    for (int y = 0; y < height; y++)
    {
        const int yOffset=y*width;
        for (int x = 0; x < width; x++)
        {
            const int idx=yOffset+x;
            calculatedDTTemp[x] = calculatedDT[idx];
        }

        calculateDistanceTransformation1D(calculatedDTTemp, calculatedDTTemp2);

        for (int x = 0; x < width; x++)
        {
            const int idx=yOffset+x;
            calculatedDT[idx] = calculatedDTTemp2[x];
        }
    }
}
