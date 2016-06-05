#include "ImageManipulation.h"

void ImageFramework::invertImage(Image* img)
{
    Image& imgRef=*img;

    for(int i=0; i<imgRef.width*imgRef.height; i++)
    {
        imgRef.imgData[i].color.red=255-imgRef.imgData[i].color.red;
        imgRef.imgData[i].color.green=255-imgRef.imgData[i].color.green;
        imgRef.imgData[i].color.blue=255-imgRef.imgData[i].color.blue;
    }
}

void ImageFramework::removeSmallRegions(std::vector<bool>& binarizedImage, int width, int height, int minSize, int radius, bool groupingColor)
{
    std::vector<bool> visited(binarizedImage.size(),false);
    std::vector<std::pair<int,int>> pointsToCheck;
    std::vector<int> checkedPoints;

    for(int y=0;y<height;y++)
    {
        const int yOffset=y*width;
        for(int x=0;x<width;x++)
        {
            if(visited[yOffset+x]==false && binarizedImage[yOffset+x]==groupingColor)
            {
                int cPointsInRegion=1;
                int currX=x;
                int currY=y;
                pointsToCheck.push_back(std::pair<int,int>(currX,currY));
                visited[yOffset+x]=true;
                checkedPoints.push_back(yOffset+x);

                while(pointsToCheck.size()>0)
                {
                    currX=std::get<0>(pointsToCheck.back());
                    currY=std::get<1>(pointsToCheck.back());
                    pointsToCheck.pop_back();

                    for(int i=-radius; i<=radius; i++)
                    {
                        if(currY+i>=0 && currY+i<height)
                        {
                            const int offset=(i+currY)*width+currX;
                            for(int j=-radius; j<=radius; j++)
                            {
                                if(currX+j>=0 && currX+j<width)
                                {
                                    if(visited[offset+j]==false && binarizedImage[offset+j]==groupingColor)
                                    {
                                        visited[offset+j]=true;
                                        checkedPoints.push_back(offset+j);
                                        pointsToCheck.push_back(std::pair<int,int>(currX+j,currY+i));
                                        cPointsInRegion++;
                                    }

                                }
                            }
                        }
                    }
                }

                if(cPointsInRegion<minSize)
                {
                    for(unsigned int i=0;i<checkedPoints.size();i++)
                    {
                        binarizedImage[checkedPoints[i]]=!groupingColor;
                    }
                }
                checkedPoints.clear();

            }

        }
    }
}

void ImageFramework::fillOnePixelHoles(std::vector<bool>& binPixel)
{
    if(binPixel.size()<3)
    {
        return;
    }

    const unsigned int binPixelSize=binPixel.size();
    bool intersectingPixelValue=binPixel[binPixel.size()-1];

    bool beforePixelValue=intersectingPixelValue;
    bool currPixelValue=intersectingPixelValue;
    bool nextPixelValue=binPixel[0];

    for(unsigned int i=0; i<binPixelSize-1; i++)
    {
        beforePixelValue=currPixelValue;
        currPixelValue=nextPixelValue;
        nextPixelValue=binPixel[i+1];

        if(beforePixelValue==0 && currPixelValue==1 && nextPixelValue==0)
        {
            binPixel[i]=0;
        }
        else if(beforePixelValue==1 && currPixelValue==0 && nextPixelValue==1)
        {
            binPixel[i]=1;
        }
    }

    if(binPixel[0]==0 && intersectingPixelValue==1 && binPixel[binPixelSize-2]==0)
    {
        binPixel[binPixelSize-1]=0;
    }
    else if(binPixel[0]==1 && intersectingPixelValue==0 && binPixel[binPixelSize-2]==1)
    {
        binPixel[binPixelSize-1]=1;
    }
}

void ImageFramework::dilate(std::vector<bool>& binPixel, int widthBinPixel, int heightBinPixel, int radiusWidthDilation, int radiusHeightDilation)
{
    bool cross=false;
    if(radiusWidthDilation==0 && radiusHeightDilation==0)
    {
        cross=true;
        radiusHeightDilation=1;
        radiusWidthDilation=1;
    }
    std::vector<bool> binPixelCopy(binPixel);

    for( int y=0; y< heightBinPixel; y++)
    {
        for( int x=0; x < widthBinPixel; x++)
        {
            if(binPixelCopy[y*widthBinPixel+x]==true)
            {
                const int lowerBoundX=std::max(x-radiusWidthDilation,0);
                const int lowerBoundY=std::max(y-radiusHeightDilation,0);

                const int upperBoundX=std::min(x+radiusWidthDilation,widthBinPixel-1);
                const int upperBoundY=std::min(y+radiusHeightDilation,heightBinPixel-1);

                for(int yDilate=lowerBoundY; yDilate <= upperBoundY; yDilate++)
                {
                    const int yOffset=yDilate*widthBinPixel;
                    for(int xDilate=lowerBoundX; xDilate <= upperBoundX; xDilate++)
                    {
                        if(cross==false || (cross==true && (xDilate==x || yDilate==y)))
                        {
                            binPixel[xDilate+yOffset]=true;
                        }
                    }
                }
            }
        }
    }
}

void ImageFramework::erode(std::vector<bool>& binPixel, int widthBinPixel, int heightBinPixel, int radiusWidthDilation, int radiusHeightDilation)
{
    bool cross=false;
    if(radiusWidthDilation==0 && radiusHeightDilation==0)
    {
        cross=true;
        radiusHeightDilation=1;
        radiusWidthDilation=1;
    }

    std::vector<bool> binPixelCopy(binPixel);

    for( int y=0; y< heightBinPixel; y++)
    {
        for( int x=0; x < widthBinPixel; x++)
        {
            if(binPixelCopy[y*widthBinPixel+x]==false)
            {
                const int lowerBoundX=std::max(x-radiusWidthDilation,0);
                const int lowerBoundY=std::max(y-radiusHeightDilation,0);

                const int upperBoundX=std::min(x+radiusWidthDilation,widthBinPixel-1);
                const int upperBoundY=std::min(y+radiusHeightDilation,heightBinPixel-1);

                for(int yDilate=lowerBoundY; yDilate <= upperBoundY; yDilate++)
                {
                    const int yOffset=yDilate*widthBinPixel;
                    for(int xDilate=lowerBoundX; xDilate <= upperBoundX; xDilate++)
                    {
                        if(cross==false || (cross==true && (xDilate==x || yDilate==y)))
                        {
                            binPixel[xDilate+yOffset]=false;
                        }
                    }
                }
            }
        }
    }
}

void ImageFramework::convertToGrayscaleImage(Image* img, bool equalWeights)
{
    Image& imgRef=*img;

    float grayValue;

    float rWeight=0.21;
    float gWeight=0.72;
    float bWeight=0.07;

    if(equalWeights==true)
    {
        rWeight=0.33;
        gWeight=0.33;
        bWeight=0.33;
    }

    for(int i=0; i<imgRef.width*imgRef.height; i++)
    {
        grayValue = (float)imgRef.imgData[i].color.red*rWeight;
        grayValue += (float)imgRef.imgData[i].color.green*gWeight;
        grayValue += (float)imgRef.imgData[i].color.blue*bWeight;

        if(grayValue>=256)
        {
            grayValue=255;
        }

        imgRef.imgData[i].color.red=grayValue;
        imgRef.imgData[i].color.green=grayValue;
        imgRef.imgData[i].color.blue=grayValue;
    }
}

Image* ImageFramework::convolutionFilter(Image* img, std::vector<float>& filter, int filterWidth, int filterHeight, bool needsNormalization)
{
    return convolutionFilter(img, &filter[0], filterWidth, filterHeight, needsNormalization);
}

Image* ImageFramework::convolutionFilter(Image* img, float *filter, int filterWidth, int filterHeight, bool needsNormalization)
{
    Image& imgRef=*img;

    Image *output = new Image(imgRef.width, imgRef.height, 24);
    int distanceRight=filterWidth/2;
    int distanceTop=filterHeight/2;
    float *tmpValuesR=nullptr;
    float *tmpValuesG=nullptr;
    float *tmpValuesB=nullptr;

    if(needsNormalization)
    {
        tmpValuesR=new float[imgRef.width*imgRef.height];
        tmpValuesG=new float[imgRef.width*imgRef.height];
        tmpValuesB=new float[imgRef.width*imgRef.height];
    }

    // Fill border with old values
    for(int y=0; y<distanceTop; y++)
    {
        for(int x=0; x<imgRef.width; x++)
        {
            output->imgData[x+y*imgRef.width]=imgRef.imgData[x+y*imgRef.width];
        }
    }

    for(int y=0; y<imgRef.height; y++)
    {
        for(int x=0; x<distanceRight; x++)
        {
            output->imgData[x+y*imgRef.width]=imgRef.imgData[x+y*imgRef.width];
        }
    }

    for(int y=imgRef.height-distanceTop; y<imgRef.height; y++)
    {
        for(int x=0; x<imgRef.width; x++)
        {
            output->imgData[x+y*imgRef.width]=imgRef.imgData[x+y*imgRef.width];
        }
    }

    for(int y=0; y<imgRef.height; y++)
    {
        for(int x=imgRef.width-distanceRight; x<imgRef.width; x++)
        {
            output->imgData[x+y*imgRef.width]=imgRef.imgData[x+y*imgRef.width];
        }
    }


    // do convolution
    float newValueR;
    float newValueG;
    float newValueB;

    float maxValueR=-10000;
    float maxValueG=-10000;
    float maxValueB=-10000;

    float minValueR=10000;
    float minValueG=10000;
    float minValueB=10000;

    for(int y=distanceTop; y<imgRef.height-distanceTop; y++)
    {
        for(int x=distanceRight; x<imgRef.width-distanceRight; x++)
        {
            newValueR=0;
            newValueG=0;
            newValueB=0;
            for(int i=0; i<filterHeight; i++)
            {
                int currYOffset=i*filterWidth;
                for(int j=0; j<filterWidth; j++)
                {
                    newValueR+=filter[currYOffset+j]*(float)imgRef.imgData[(x-distanceRight+j)+(y-distanceTop+i)*imgRef.width].color.red;
                    newValueG+=filter[currYOffset+j]*(float)imgRef.imgData[(x-distanceRight+j)+(y-distanceTop+i)*imgRef.width].color.green;
                    newValueB+=filter[currYOffset+j]*(float)imgRef.imgData[(x-distanceRight+j)+(y-distanceTop+i)*imgRef.width].color.blue;

                }
            }

            if(needsNormalization==false)
            {
                output->imgData[x+y*imgRef.width].color.red=newValueR;
                output->imgData[x+y*imgRef.width].color.green=newValueG;
                output->imgData[x+y*imgRef.width].color.blue=newValueB;
            }
            else
            {
                tmpValuesR[x+y*imgRef.width]=newValueR;
                tmpValuesG[x+y*imgRef.width]=newValueG;
                tmpValuesB[x+y*imgRef.width]=newValueB;

                minValueR=std::min(minValueR, newValueR);
                minValueG=std::min(minValueG, newValueG);
                minValueB=std::min(minValueB, newValueB);

                maxValueR=std::max(maxValueR, newValueR);
                maxValueG=std::max(maxValueG, newValueG);
                maxValueB=std::max(maxValueB, newValueB);
            }
        }
    }

    if(needsNormalization)
    {
        float distR=(maxValueR-minValueR);
        float distG=(maxValueG-minValueG);
        float distB=(maxValueB-minValueB);

        for(int y=distanceTop; y<imgRef.height-distanceTop; y++)
        {
            for(int x=distanceRight; x<imgRef.width-distanceRight; x++)
            {
                output->imgData[x+y*imgRef.width].color.red=(tmpValuesR[x+y*imgRef.width]-minValueR)/distR*256;
                output->imgData[x+y*imgRef.width].color.green=(tmpValuesG[x+y*imgRef.width]-minValueG)/distG*256;
                output->imgData[x+y*imgRef.width].color.blue=(tmpValuesB[x+y*imgRef.width]-minValueB)/distB*256;
            }
        }
    }

    if(needsNormalization)
    {
        delete[] tmpValuesR;
        delete[] tmpValuesG;
        delete[] tmpValuesB;
    }

    return output;
}

void ImageFramework::sub(Image* destImg, const Image* img, bool needsNormalization)
{
    const Image& imgRef=*img;
    Image& destImgRef=*destImg;

    const int imgSize=destImgRef.width*destImgRef.height;
    const int imgSize2=imgRef.width*imgRef.height;
    const int minImgSize=std::min(imgSize,imgSize2);

    float *tmpValuesR=nullptr;
    float *tmpValuesG=nullptr;
    float *tmpValuesB=nullptr;

    if(needsNormalization)
    {
        tmpValuesR=new float[destImgRef.width*destImgRef.height];
        tmpValuesG=new float[destImgRef.width*destImgRef.height];
        tmpValuesB=new float[destImgRef.width*destImgRef.height];
    }

    float newValueR;
    float newValueG;
    float newValueB;

    float maxValueR=-10000;
    float maxValueG=-10000;
    float maxValueB=-10000;

    float minValueR=10000;
    float minValueG=10000;
    float minValueB=10000;

    for(int i=0; i<minImgSize; i++)
    {
        if(needsNormalization==false)
        {
            destImgRef.imgData[i].color.red-=imgRef.imgData[i].color.red;
            destImgRef.imgData[i].color.green-=imgRef.imgData[i].color.green;
            destImgRef.imgData[i].color.blue-=imgRef.imgData[i].color.blue;
        }
        else
        {
            newValueR=(float)destImgRef.imgData[i].color.red-(float)imgRef.imgData[i].color.red;
            newValueG=(float)destImgRef.imgData[i].color.green-(float)imgRef.imgData[i].color.green;
            newValueB=(float)destImgRef.imgData[i].color.blue-(float)imgRef.imgData[i].color.blue;

            tmpValuesR[i]=newValueR;
            tmpValuesG[i]=newValueG;
            tmpValuesB[i]=newValueB;

            minValueR=std::min(minValueR, newValueR);
            minValueG=std::min(minValueG, newValueG);
            minValueB=std::min(minValueB, newValueB);

            maxValueR=std::max(maxValueR, newValueR);
            maxValueG=std::max(maxValueG, newValueG);
            maxValueB=std::max(maxValueB, newValueB);
        }
    }

    if(needsNormalization)
    {
        float distR=(maxValueR-minValueR);
        float distG=(maxValueG-minValueG);
        float distB=(maxValueB-minValueB);

        for(int i=0; i<minImgSize; i++)
        {
            destImgRef.imgData[i].color.red=(tmpValuesR[i]-minValueR)/distR*256;
            destImgRef.imgData[i].color.green=(tmpValuesG[i]-minValueG)/distG*256;
            destImgRef.imgData[i].color.blue=(tmpValuesB[i]-minValueB)/distB*256;
        }

        delete[] tmpValuesR;
        delete[] tmpValuesG;
        delete[] tmpValuesB;
    }
}

void ImageFramework::add(Image* destImg, const Image* img, bool needsNormalization)
{
    const Image& imgRef=*img;
    Image& destImgRef=*destImg;

    const int imgSize=destImgRef.width*destImgRef.height;
    const int imgSize2=imgRef.width*imgRef.height;
    const int minImgSize=std::min(imgSize,imgSize2);

    float *tmpValuesR=nullptr;
    float *tmpValuesG=nullptr;
    float *tmpValuesB=nullptr;

    if(needsNormalization)
    {
        tmpValuesR=new float[destImgRef.width*destImgRef.height];
        tmpValuesG=new float[destImgRef.width*destImgRef.height];
        tmpValuesB=new float[destImgRef.width*destImgRef.height];
    }

    float newValueR;
    float newValueG;
    float newValueB;

    float maxValueR=-10000;
    float maxValueG=-10000;
    float maxValueB=-10000;

    float minValueR=10000;
    float minValueG=10000;
    float minValueB=10000;

    for(int i=0; i<minImgSize; i++)
    {
        if(needsNormalization==false)
        {
            destImgRef.imgData[i].color.red+=imgRef.imgData[i].color.red;
            destImgRef.imgData[i].color.green+=imgRef.imgData[i].color.green;
            destImgRef.imgData[i].color.blue+=imgRef.imgData[i].color.blue;
        }
        else
        {
            newValueR=(float)destImgRef.imgData[i].color.red+(float)imgRef.imgData[i].color.red;
            newValueG=(float)destImgRef.imgData[i].color.green+(float)imgRef.imgData[i].color.green;
            newValueB=(float)destImgRef.imgData[i].color.blue+(float)imgRef.imgData[i].color.blue;

            tmpValuesR[i]=newValueR;
            tmpValuesG[i]=newValueG;
            tmpValuesB[i]=newValueB;

            minValueR=std::min(minValueR, newValueR);
            minValueG=std::min(minValueG, newValueG);
            minValueB=std::min(minValueB, newValueB);

            maxValueR=std::max(maxValueR, newValueR);
            maxValueG=std::max(maxValueG, newValueG);
            maxValueB=std::max(maxValueB, newValueB);
        }
    }

    if(needsNormalization)
    {
        float distR=(maxValueR-minValueR);
        float distG=(maxValueG-minValueG);
        float distB=(maxValueB-minValueB);

        for(int i=0; i<minImgSize; i++)
        {
            destImgRef.imgData[i].color.red=(tmpValuesR[i]-minValueR)/distR*256;
            destImgRef.imgData[i].color.green=(tmpValuesG[i]-minValueG)/distG*256;
            destImgRef.imgData[i].color.blue=(tmpValuesB[i]-minValueB)/distB*256;
        }

        delete[] tmpValuesR;
        delete[] tmpValuesG;
        delete[] tmpValuesB;
    }
}


Image* ImageFramework::resizeImageNearestNeighbour(Image* img, int newWidth, int newHeight)
{
    Image& imgRef=*img;

    Image* img2=new Image(newWidth,newHeight);
    unsigned int xRatio = (unsigned int)((imgRef.width<<16)/newWidth) + 1;
    unsigned int yRatio = (unsigned int)((imgRef.height<<16)/newHeight) + 1;

    for (int i=0; i<newHeight; i++)
    {
        const unsigned int y = ((i*yRatio)>>16);
        const unsigned int yOffset=y*imgRef.width;
        const unsigned int yOffsetNew=i*newWidth;

        for (int j=0; j<newWidth; j++)
        {
            const unsigned int x = ((j*xRatio)>>16);
            img2->imgData[yOffsetNew+j].color.red = imgRef.imgData[yOffset+x].color.red;
            img2->imgData[yOffsetNew+j].color.blue = imgRef.imgData[yOffset+x].color.blue;
            img2->imgData[yOffsetNew+j].color.green = imgRef.imgData[yOffset+x].color.green;
        }
    }

    return img2;
}


std::vector<bool> ImageFramework::adaptiveBinarization(Image* img, int radius, int radiusSecondary)
{
    Image& imgRef=*img;

    std::vector<bool> binarized(imgRef.width*imgRef.height,false);
    std::vector<int> standardDev(imgRef.width*imgRef.height,0);
    std::vector<int> mean(imgRef.width*imgRef.height,0);
    std::vector<int> minVal(imgRef.width*imgRef.height,100000);

    for(int y=0;y<imgRef.height;y++)
    {
        for(int x=0;x<imgRef.width;x++)
        {
            const int idx=x+y*imgRef.width;

            int countVals=0;
            for(int i=-radius;i<=radius;i++)
            {
                if(y+i>=0 && y+i<imgRef.height)
                {
                    const int offset=(y+i)*imgRef.width+x;
                    for(int j=-radius;j<=radius;j++)
                    {
                        if(x+j>=0 && x+j<imgRef.width)
                        {
                            const int val=imgRef.imgData[offset+j].color.red;
                            minVal[idx]=std::min(minVal[idx],val);
                            mean[idx]+=val;
                            countVals++;
                        }
                    }
                }
            }

            mean[idx]/=countVals;
            for(int i=-radius;i<=radius;i++)
            {
                if(y+i>=0 && y+i<imgRef.height)
                {
                    const int offset=(y+i)*imgRef.width+x;
                    for(int j=-radius;j<=radius;j++)
                    {
                        if(x+j>=0 && x+j<imgRef.width)
                        {
                            const int val=imgRef.imgData[offset+j].color.red-mean[idx];
                            standardDev[idx]+=val*val;
                        }
                    }
                }
            }
            standardDev[idx]/=countVals;
            standardDev[idx]=isqrt(standardDev[idx]);
        }
    }

    const float k1=0.03;
    const float k2=0.10;
    const float a1=0.10;

    for(int y=0;y<imgRef.height;y++)
    {
        for(int x=0;x<imgRef.width;x++)
        {
            const int idx=x+y*imgRef.width;
            int maxDev=0;

            for(int i=-radiusSecondary;i<=radiusSecondary;i++)
            {
                if(y+i>=0 && y+i<imgRef.height)
                {
                    const int offset=(y+i)*imgRef.width+x;
                    for(int j=-radiusSecondary;j<=radiusSecondary;j++)
                    {
                        if(x+j>=0 && x+j<imgRef.width)
                        {
                            maxDev=std::max(standardDev[offset+j],maxDev);
                        }
                    }
                }
            }

            float threshold;
            if(maxDev==0)
            {
                threshold=(1.0-a1)*(float)mean[idx];
            }
            else
            {
                const float ratio=(standardDev[idx]/maxDev);
                const float a2=k1*ratio*ratio;
                const float a3=k2*ratio*ratio;
                threshold=(1.0-a1)*(float)mean[idx]+a2*ratio*(float)(mean[idx]-minVal[idx])+a3*(float)minVal[idx];
            }

            if(threshold < imgRef.imgData[idx].color.red)
            {
                binarized[idx]=1;
            }
        }
    }

    return binarized;
}


Image* ImageFramework::medianFilter(Image* img, int radius)
{
    Image& imgRef=*img;

    Image* output = new Image(imgRef.width,imgRef.height);
    const int area=(radius*2+1)*(radius*2+1);
    std::vector<int> points(area);

    for(int y=0;y<imgRef.height;y++)
    {
        const int yOffset=y*imgRef.width;
        for(int x=0;x<imgRef.width;x++)
        {
            points.clear();
            int currCount=0;
            for(int i=-radius; i<=radius; i++)
            {
                if(y+i>=0 && y+i<imgRef.height)
                {
                    const int offset=(i+y)*imgRef.width+x;
                    for(int j=-radius; j<=radius; j++)
                    {
                        if(x+j>=0 && x+j<imgRef.width)
                        {
                            points[currCount]=imgRef.imgData[offset+j].color.red;
                            currCount++;
                        }
                    }
                }
            }

            std::sort(points.begin(),points.end());
            output->imgData[yOffset+x].color.red=points[currCount>>1];
        }
    }
    return output;
}
