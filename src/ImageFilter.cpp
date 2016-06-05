#include "ImageFilter.h"


std::vector<float> ImageFramework::calculate1DGaussianKernel(int size, float sigma)
{
    if(size%2==0)
    {
        size++;
    }

    std::vector<float> kernel(size);
    const int midOfKernel=size>>1;
    float sum=0.0;

    for(int i=-midOfKernel; i<=midOfKernel; i++)
    {
        kernel[i+midOfKernel]=1.0/sqrt(2.0*M_PI*sigma*sigma)*exp((-(float)(i*i)/(2.0*sigma*sigma)));
        sum+=kernel[i+midOfKernel];
    }

    // Normalize values so the sum is 1.0
    for(unsigned int i=0; i<kernel.size(); i++)
    {
        kernel[i]/=sum;
    }
    return kernel;
}
