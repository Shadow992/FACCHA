#include "ImageFramework.h"
#include <chrono>
#include <iostream>

// Required Libs:
//  libJPEG
//  libPNG
//  libZLib (only used for libPNG)

using namespace ImageFramework;

int main(int argc, char* argv[])
{
    auto start = std::chrono::steady_clock::now();

    int primaryWindowSize = 12;
    int secondaryWindowSize = 10;
    float gaussSigma = 1;
    int gaussRadius = 5;
    char defaultSettings = 'c';
    bool invert = false;

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <input file> <configuration mode ('c' or 'd', defaults to 'd')"
                  << std::endl;
        return 0;
    }

    if (argc == 3)
    {
        defaultSettings = argv[2][0];
    }
    else
    {
        std::cout << "Default settings or custom settings (d/c): ";
        std::cin >> defaultSettings;
    }

    if (defaultSettings == 'c')
    {
        std::cout << "Choose the settings to use:" << std::endl;
        std::cout << "Primary Window Size for binarization (default = " << primaryWindowSize << "): ";
        std::cin >> primaryWindowSize;
        std::cout << "Secondary Window Size for binarization (default = " << secondaryWindowSize << "): ";
        std::cin >> secondaryWindowSize;
        std::cout << "Invert image (default = " << invert << "): ";
        std::cin >> invert;
        std::cout << "Convolution Filter Gauss Sigma (default = " << gaussSigma << ".0): ";
        std::cin >> gaussSigma;
        std::cout << "Convolution Filter Gauss Radius (default = " << gaussRadius << "): ";
        std::cin >> gaussRadius;
    }
    std::cout << "Processing image (this may take several time)..." << std::endl;

    Image* img;
    img = readFile(argv[1]);
    if (img == nullptr)
    {
        return 0;
    }

    Image* im2 = new Image(img->width, img->height);

    if (invert == true)
    {
        invertImage(img);
    }

    convertToGrayscaleImage(img, true);

    auto gauss1D = calculate1DGaussianKernel(gaussRadius, gaussSigma);
    Image* imgCon = convolutionFilter(img, gauss1D, 1, gaussRadius);
    Image* imgCon2 = convolutionFilter(img, gauss1D, gaussRadius, 1);

    auto binarizedImage = adaptiveBinarization(imgCon2, primaryWindowSize, secondaryWindowSize);
    removeSmallRegions(binarizedImage, imgCon2->width, imgCon->height, 18, 1);
    removeSmallRegions(binarizedImage, imgCon2->width, imgCon->height, 4, 1, true);

    for (unsigned int i = 0; i < binarizedImage.size(); i++)
    {
        if (binarizedImage[i] == true)
        {
            im2->imgData[i].color.red = 255;
            im2->imgData[i].color.green = 255;
            im2->imgData[i].color.blue = 255;
        }
        else
        {
            im2->imgData[i].color.red = 0;
            im2->imgData[i].color.green = 0;
            im2->imgData[i].color.blue = 0;
        }
    }

    saveAsPNG(im2, "tmp.png");

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    std::cout << "It took: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    return 0;
}
