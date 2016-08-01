
#include "ImageFeature.h"
 
unsigned int isqrt(unsigned int x)
{
    register unsigned int op, res, one;
 
    op = x;
    res = 0;
 
    /* "one" starts at the highest power of four <= than the argument. */
    one = 1 << 30; /* second-to-top bit set */
    while (one > op)
    {
        one >>= 2;
    }
 
    while (one != 0)
    {
        if (op >= res + one)
        {
            op -= res + one;
            res += one << 1; // <-- faster than 2 * one
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}
 
void ImageFramework::invertImage(Mat& img)
{
    if (img.type() == CV_8UC3)
    {
        img.forEach<cv::Point3_<uint8_t>>([](cv::Point3_<uint8_t> &p, const int * position) -> void
        {
            p.x = 255-p.x;
            p.y = 255-p.y;
            p.z = 255-p.z;
        });
    }
    else if (img.type() == CV_32F)
    {
        img.forEach<float>([](float &p, const int * position) -> void
        {
            p = 1.0-p;
        });
    }
}
 
void ImageFramework::removeSmallRegions(
    std::vector<bool>& binarizedImage, int width, int height, int minSize, int radius, bool groupingColor)
{
    std::vector<bool> visited(binarizedImage.size(), false);
    std::vector<std::pair<int, int> > pointsToCheck;
    std::vector<int> checkedPoints;
 
    for (int y = 0; y < height; y++)
    {
        const int yOffset = y * width;
        for (int x = 0; x < width; x++)
        {
            if (!visited[yOffset + x] && binarizedImage[yOffset + x] == groupingColor)
            {
                int cPointsInRegion = 1;
                int currX = x;
                int currY = y;
                pointsToCheck.push_back(std::pair<int, int>(currX, currY));
                visited[yOffset + x] = true;
                checkedPoints.push_back(yOffset + x);
 
                while (!pointsToCheck.empty())
                {
                    currX = std::get<0>(pointsToCheck.back());
                    currY = std::get<1>(pointsToCheck.back());
                    pointsToCheck.pop_back();
 
                    for (int i = -radius; i <= radius; i++)
                    {
                        if (currY + i >= 0 && currY + i < height)
                        {
                            const int offset = (i + currY) * width + currX;
                            for (int j = -radius; j <= radius; j++)
                            {
                                if (currX + j >= 0 && currX + j < width)
                                {
                                    if (!visited[offset + j] && binarizedImage[offset + j] == groupingColor)
                                    {
                                        visited[offset + j] = true;
                                        checkedPoints.push_back(offset + j);
                                        pointsToCheck.push_back(std::pair<int, int>(currX + j, currY + i));
                                        cPointsInRegion++;
                                    }
                                }
                            }
                        }
                    }
                }
 
                if (cPointsInRegion < minSize)
                {
                    for (int checkedPoint : checkedPoints)
                    {
                        binarizedImage[checkedPoint] = !groupingColor;
                    }
                }
                checkedPoints.clear();
            }
        }
    }
}


void ImageFramework::fillOnePixelHoles(std::vector<bool>& binPixel)
{
    if (binPixel.size() < 3)
    {
        return;
    }
 
    const unsigned int binPixelSize = binPixel.size();
    bool intersectingPixelValue = binPixel[binPixel.size() - 1];
 
    bool beforePixelValue = intersectingPixelValue;
    bool currPixelValue = intersectingPixelValue;
    bool nextPixelValue = binPixel[0];
 
    for (unsigned int i = 0; i < binPixelSize - 1; i++)
    {
        beforePixelValue = currPixelValue;
        currPixelValue = nextPixelValue;
        nextPixelValue = binPixel[i + 1];
 
        if (!beforePixelValue && currPixelValue && !nextPixelValue)
        {
            binPixel[i] = false;
        }
        else if (beforePixelValue && !currPixelValue && nextPixelValue)
        {
            binPixel[i] = true;
        }
    }
 
    if (!binPixel[0] && intersectingPixelValue && !binPixel[binPixelSize - 2])
    {
        binPixel[binPixelSize - 1] = false;
    }
    else if (binPixel[0] && !intersectingPixelValue && binPixel[binPixelSize - 2])
    {
        binPixel[binPixelSize - 1] = true;
    }
}
 

std::vector<bool> ImageFramework::adaptiveBinarization(Mat& img, int radius, int radiusSecondary)
{
    Size imgSize = img.size();
 
    std::vector<bool> binarized(imgSize.width * imgSize.height, false);
    std::vector<int> standardDev(imgSize.width * imgSize.height, 0);
    std::vector<int> mean(imgSize.width * imgSize.height, 0);
    std::vector<int> minVal(imgSize.width * imgSize.height, 100000);
 
    for (int y = 0; y < imgSize.height; y++)
    {
        for (int x = 0; x < imgSize.width; x++)
        {
            const int idx = x + y * imgSize.width;
 
            float meanApprox=0;
            float standardDevApprox=0;
            float countVals = 1;
            for (int i = -radius; i <= radius; i++)
            {
                if (y + i >= 0 && y + i < imgSize.height)
                {
                    const uint8_t* currRow = img.ptr<uint8_t>(y + i);
                    for (int j = -radius; j <= radius; j++)
                    {
                        if (x + j >= 0 && x + j < imgSize.width)
                        {
                            const float val = currRow[x+j];
                            const float meanApproxBefore=meanApprox;
                            minVal[idx] = std::min(minVal[idx], (int)val);
                            meanApprox += (val-meanApprox)/countVals;
                            standardDevApprox+=(val - meanApprox) * (val - meanApproxBefore);
                            countVals+=1;
                        }
                    }
                }
            }
 
            mean[idx] = meanApprox;
            standardDev[idx]=isqrt(standardDevApprox/countVals);
        }
    }
 
    const float k1 = 0.03;
    const float k2 = 0.15;
    const float a1 = 0.15;
 
    for (int y = 0; y < imgSize.height; y++)
    {
        const uint8_t* currRow = img.ptr<uint8_t>(y);
        const int yOffset= y * imgSize.width;
        for (int x = 0; x < imgSize.width; x++)
        {
            const int idx = x + yOffset;
            int maxDev = 0;
 
            for (int i = -radiusSecondary; i <= radiusSecondary; i++)
            {
                if (y + i >= 0 && y + i < imgSize.height)
                {
                    const int offset = (y + i) * imgSize.width + x;
                    for (int j = -radiusSecondary; j <= radiusSecondary; j++)
                    {
                        if (x + j >= 0 && x + j < imgSize.width)
                        {
                            maxDev = std::max(standardDev[offset + j], maxDev);
                        }
                    }
                }
            }
 
            float threshold;
            if (maxDev == 0)
            {
                threshold = (1.0 - a1) * static_cast<float>(mean[idx]);
            }
            else
            {
                const float ratio = (standardDev[idx] / maxDev);
                const float a2 = k1 * ratio * ratio;
                const float a3 = k2 * ratio * ratio;
                threshold = (1.0 - a1) * static_cast<float>(mean[idx])
                    + a2 * ratio * static_cast<float>(mean[idx] - minVal[idx]) + a3 * static_cast<float>(minVal[idx]);
            }
 
            if (threshold < currRow[x])
            {
                binarized[idx] = true;
            }
        }
    }
 
    return binarized;
}
 
Vec3i ImageFramework::getIntegralImageMean(Mat& integralImg, int xStart, int xEnd, int yStart, int yEnd)
{
    const Vec3i leftTop = integralImg.at<Vec3i>(yStart,xStart);
    const Vec3i leftBot = integralImg.at<Vec3i>(yEnd,xStart);
    const Vec3i rightTop = integralImg.at<Vec3i>(yStart,xEnd);
    const Vec3i rightBot = integralImg.at<Vec3i>(yEnd,xEnd);
 
    Vec3i result;
 
    for(int i=0;i<3;i++)
    {
        result[i] = leftTop[i] - rightTop[i];
        result[i] += rightBot[i] - leftBot[i];
    }
 
    const int area=(xEnd-xStart)*(yEnd-yStart);
    result[0]/=area;
    result[1]/=area;
    result[2]/=area;
 
    return result;
}
 
// Salient Region Detection and Segmentation: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.167.1500&rep=rep1&type=pdf
// + added global rarity voting
Mat ImageFramework::getSaliencyMap(Mat& img, int smallWindowSize, int scaleStep, int startingScale, int endingScale, int randomOtherRegions)
{
    Size imgSize = img.size();
 
    const int width=imgSize.width;
    const int height=imgSize.height;
 
    Mat integralImg;
    integral(img,integralImg);
    Size integralImgSize = integralImg.size();

    int smallerDimension=std::min(width,height);
 
    Mat saliencyMap=Mat::zeros(height,width, CV_32F);
 
    int bigImageSize;
    int halfBigWindowSize;
    int halfSmallWindowSize=smallWindowSize>>1;
 
    std::vector<float> randL;
    std::vector<float> randa;
    std::vector<float> randb;
 
    for(int scales=startingScale;scales<=endingScale;scales+=scaleStep)
    {
        bigImageSize=smallerDimension/scales;
        halfBigWindowSize=bigImageSize>>1;
 
        randL.clear();
        randa.clear();
        randb.clear();
 
        for(int i=0;i<randomOtherRegions;i++)
        {
            const int xStartRandom=std::max(rand()%(width-10),0);
            const int xEndRandom=std::min(xStartRandom+bigImageSize,width-1);
            const int yStartRandom=std::max(rand()%(height-10),0);
            const int yEndRandom=std::min(yStartRandom+bigImageSize,height-1);
 
            const Vec3i avgValRan = getIntegralImageMean(integralImg, xStartRandom, xEndRandom, yStartRandom, yEndRandom);
 
            randL.push_back(avgValRan[0]);
            randa.push_back(avgValRan[1]);
            randb.push_back(avgValRan[2]);
        }
 
        for(int y=0;y<height;y++)
        {
            float* currRow=saliencyMap.ptr<float>(y);
 
            const int yStartSmall=std::max(y-halfSmallWindowSize,0);
            const int yStartBig=std::max(y-halfBigWindowSize,0);
            const int yEndSmall=std::min(y+halfSmallWindowSize,height-1);
            const int yEndBig=std::min(y+halfBigWindowSize,height-1);
 
            for(int x=0;x<width;x++)
            {
                const int xStartSmall=std::max(x-halfSmallWindowSize,0);
                const int xStartBig=std::max(x-halfBigWindowSize,0);
                const int xEndSmall=std::min(x+halfSmallWindowSize,width-1);
                const int xEndBig=std::min(x+halfBigWindowSize,width-1);
 
                const Vec3i avgValSmall = getIntegralImageMean(integralImg, xStartSmall, xEndSmall, yStartSmall, yEndSmall);
                const Vec3i avgValBig = getIntegralImageMean(integralImg, xStartBig, xEndBig, yStartBig, yEndBig);
 
                const float difL=std::abs(avgValSmall[0]-avgValBig[0])*1.5;
                const float difa=std::abs(avgValSmall[1]-avgValBig[1]);
                const float difb=std::abs(avgValSmall[2]-avgValBig[2]);
 
                currRow[x]+=sqrt(difL*difL+difa*difa+difb*difb)/(float)scales*(float)randomOtherRegions;
 
                for(int i=0;i<randomOtherRegions;i++)
                {
                    const float difL2=std::abs(avgValSmall[0]-randL[i])*1.5;
                    const float difa2=std::abs(avgValSmall[1]-randa[i]);
                    const float difb2=std::abs(avgValSmall[2]-randb[i]);
 
                    currRow[x]+=sqrt(difL2*difL2+difa2*difa2+difb2*difb2)/(float)scales;
                }
 
            }
        }
    }
 
    return saliencyMap;
}
