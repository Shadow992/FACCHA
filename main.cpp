#include <iostream>
#include <chrono>
#include "ImageFramework.h"

using namespace ImageFramework;

int main(int argc, char *argv[])
{
    auto start = std::chrono::steady_clock::now();

    int primaryWindowSize=12;
    int secondaryWindowSize=10;
    float gaussSigma=1;
    int gaussRadius=5;

    if(argc<2)
    {
        return 0;
    }

    Image* img;
    img = readFile(argv[1]);
    if(img==nullptr)
    {
        return 0;
    }

    Image* im2 = new Image(img->width,img->height);
    invertImage(img);
    convertToGrayscaleImage(img);


    auto gauss1D=calculate1DGaussianKernel(gaussRadius,gaussSigma);
    Image* imgCon=convolutionFilter(img,gauss1D,1,gaussRadius);
    Image* imgCon2=convolutionFilter(img,gauss1D,gaussRadius,1);

    auto binarizedImage=adaptiveBinarization(imgCon2,primaryWindowSize,secondaryWindowSize);
    removeSmallRegions(binarizedImage,imgCon2->width,imgCon->height,18,1);
    removeSmallRegions(binarizedImage,imgCon2->width,imgCon->height,4,1,true);

    for(unsigned int i=0;i<binarizedImage.size();i++)
    {
        if(binarizedImage[i] == true)
        {
            im2->imgData[i].color.red=255;
            im2->imgData[i].color.green=255;
            im2->imgData[i].color.blue=255;
        }
        else
        {
            im2->imgData[i].color.red=0;
            im2->imgData[i].color.green=0;
            im2->imgData[i].color.blue=0;
        }
    }

    saveAsPNG(im2,"tmp.png");

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    std::cout<< "It took: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

    return 0;
}
