#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "ImageFeature.h"
#include <iostream>
#include <chrono>

using namespace cv;
using namespace ImageFramework;

#include <iostream>

int main(int argc, char* argv[])
{
    /*
    std::vector<std::string> tmp = getFileList("*.png");
    removeBadExamples(tmp);
    return 0;
    */
 
    auto start = std::chrono::steady_clock::now();
 
    float gaussSigma = 0.2;
    int gaussRadius = 5;
    char defaultSettings = 'd';
 
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0]
                  << " <input file> <configuration mode ('c' [=custom] or 'd' [=default], defaults to 'd')>"
                  << std::endl;
        return 0;
    }
 
    if (argc == 3)
    {
        defaultSettings = argv[2][0];
    }
 
    if (defaultSettings == 'c')
    {
        std::cout << "Choose the settings to use:" << std::endl;
        std::cout << "Convolution Filter Gauss Sigma (default = " << gaussSigma << ".0): ";
        std::cin >> gaussSigma;
        std::cout << "Convolution Filter Gauss Radius (default = " << gaussRadius << "): ";
        std::cin >> gaussRadius;
    }
    std::cout << "Processing image (this may take some time)..." << std::endl;
 
    Mat img=imread(argv[1]);
    Mat dst=imread(argv[1]);
 
    //bilateralFilter ( img, dst, 5, 50,50 );
 
    //img=trilateralFilter(img, 3, 1, 15, 20, 50);
    //imwrite("filtered.png",dst);
 
    Mat cieLabImg;
    cvtColor(dst, cieLabImg, CV_BGR2Lab);
 
    auto mapSaliency=getSaliencyMap(cieLabImg,5,1,1,6,50);
 
    float minVal=100000000;
    float maxVal=-1000000000;
    MatIterator_<float> it, endIt;
 
    for( it = mapSaliency.begin<float>(), endIt = mapSaliency.end<float>(); it != endIt; ++it)
    {
        minVal = std::min(minVal, *it);
        maxVal = std::max(maxVal, *it);
    }
 
    float distVal = (maxVal - minVal)*0.95;
 
    for( it = mapSaliency.begin<float>(), endIt = mapSaliency.end<float>(); it != endIt; ++it)
    {
        *it=std::min((int) ((*it - minVal) / distVal), 1);
    }
 
    invertImage(mapSaliency);
 
    imwrite("enhanced.png",mapSaliency);
 
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
 
    std::cout << "It took: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
 
 
    return 0;
}
