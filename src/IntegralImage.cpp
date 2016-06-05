#include "IntegralImage.h"

void IntegralImage::getMean(int x, int y, int widthWindow, int heightWindow, int& meanR, int& meanG, int& meanB)
{
    const int idx1=y*width+x;
    const int idx2=idx1+widthWindow;
    const int idx3=(y+heightWindow)*width+x;
    const int idx4=idx3+widthWindow;
    const int area=widthWindow*heightWindow;

    meanR=sumR[idx1]-sumR[idx2];
    meanR+=sumR[idx4]-sumR[idx3];

    meanG=sumG[idx1]-sumG[idx2];
    meanG+=sumG[idx4]-sumG[idx3];

    meanB=sumB[idx1]-sumB[idx2];
    meanB+=sumB[idx4]-sumB[idx3];

    meanR/=area;
    meanG/=area;
    meanB/=area;
}


void IntegralImage::calculate(Image& img)
{
    width=img.width;
    height=img.height;

    const int sizeImg=img.width*img.height;
    sumR.resize(sizeImg);
    sumG.resize(sizeImg);
    sumB.resize(sizeImg);

    sumR[0]=img.imgData[0].color.red;
    sumG[0]=img.imgData[0].color.green;
    sumB[0]=img.imgData[0].color.blue;

    for(int x=1; x<width; x++)
    {
        sumR[x]=sumR[x-1]+img.imgData[x].color.red;
        sumG[x]=sumG[x-1]+img.imgData[x].color.green;
        sumB[x]=sumB[x-1]+img.imgData[x].color.blue;
    }

    int yOffsetBefore=0;
    int yOffset=0;
    for(int y=1; y<height; y++)
    {
        int currRowSumR=0;
        int currRowSumG=0;
        int currRowSumB=0;

        yOffsetBefore=yOffset;
        yOffset=y*width;

        for(int x=0; x<width; x++)
        {
            const int idx=yOffset+x;
            const int idxAbove=yOffsetBefore+x;
            currRowSumR+=img.imgData[idx].color.red;
            currRowSumG+=img.imgData[idx].color.green;
            currRowSumB+=img.imgData[idx].color.blue;

            sumR[idx]=sumR[idxAbove]+currRowSumR;
            sumG[idx]=sumG[idxAbove]+currRowSumG;
            sumB[idx]=sumB[idxAbove]+currRowSumB;
        }
    }
}

int IntegralImage::getRegionDifference(int x, int y, int radius, int& direction)
{
    int leftMeanR;
    int leftMeanG;
    int leftMeanB;

    int rightMeanR;
    int rightMeanG;
    int rightMeanB;

    getMean(x-radius-1,y-radius,radius,radius,leftMeanR,leftMeanG,leftMeanB);
    getMean(x+1,y-radius,radius,radius,rightMeanR,rightMeanG,rightMeanB);

    int topMeanR;
    int topMeanG;
    int topMeanB;

    int botMeanR;
    int botMeanG;
    int botMeanB;

    getMean(x-radius,y-radius-1,radius,radius,topMeanR,topMeanG,topMeanB);
    getMean(x-radius,y+1,radius,radius,botMeanR,botMeanG,botMeanB);

    const int meanDifLeftRight=std::abs(leftMeanR-rightMeanR)+std::abs(leftMeanG-rightMeanG)+std::abs(leftMeanB-rightMeanB);
    const int meanDifUpperLower=std::abs(topMeanR-botMeanR)+std::abs(topMeanG-botMeanG)+std::abs(topMeanB-botMeanB);

    if(meanDifLeftRight>meanDifUpperLower)
    {
        direction=3;
        return meanDifLeftRight;
    }

    direction=5;

    return meanDifUpperLower;
}
