#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <math.h>
#include <cmath>
#include <algorithm>

enum Shape {CIRCLE_SHAPE, SQUARE_SHAPE, SQUARE_FILLED_SHAPE};

struct Color
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;

    unsigned char alphaChannel; //unused at the moment

    bool equals(const Color& a, const unsigned char variance) const
    {
        return equals(a.red,a.green,a.blue,variance);
    }

    bool equals(unsigned char r,unsigned char g,unsigned char b, const unsigned char variance) const
    {
        return abs(red-r) <= variance && abs(blue-b) <= variance && abs(green-g) <= variance;
    }
};

struct Pixel
{
    int x;
    int y;
    Color color;

    Pixel(int xIn, int yIn)
    {
        x=xIn;
        y=yIn;
    }

    Pixel()
    {

    }

    void setColor(unsigned char red, unsigned char green, unsigned char blue)
    {
        color.red=red;
        color.blue=blue;
        color.green=green;
    }

    bool colorEquals(const Pixel* a, const unsigned char variance) const
    {
        return color.equals(a->color,variance);
    }

    bool colorEquals(const Color& a, const unsigned char variance) const
    {
        return color.equals(a,variance);
    }

    bool colorEquals(unsigned char r,unsigned char g,unsigned char b, const unsigned char variance) const
    {
        return color.equals(r, g, b, variance);
    }
};

class Image
{
    public:
        Image();
        Image(int widthIn, int heightIn, int bmpType=24);
        Image(Image* img);
        virtual ~Image();

        Pixel* getPixel(int x, int y) const;
        unsigned char getPixelGrayValue(int x, int y) const;
        void getPixelsInShape(std::vector<Pixel*>& foundPixel, int xCenter, int yCenter, int radius, Shape shapeToUse=SQUARE_SHAPE);

        void setPixelColor(int x, int y, unsigned char red,unsigned char green,unsigned char blue);
        void setPixelColor(int x, int y, const Color& newPixel);
        void drawLine(int xStart, int yStart, int xEnd, int yEnd, unsigned char red,unsigned char green,unsigned char blue, int lineWidth=1);
        void drawLine(int xStart, int yStart, int xEnd, int yEnd, const Color& newColor, int lineWidth=1);
        void drawCross(int x, int y, int crossSize,const Color& newColor, int lineWidth=1);
        void drawCross(int x, int y, int crossSize, unsigned char red, unsigned char green, unsigned char blue, int lineWidth=1);

        Pixel* imgData=nullptr;
        int width;
        int height;
        int histogramAccuracy;
        int histogramWidth;
        int histogramHeight;
        unsigned int paddigPerLine;
        std::vector<float> histogram;
    protected:
    private:
};

#endif // IMAGE_H
