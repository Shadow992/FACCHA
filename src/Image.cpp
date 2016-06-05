#include "Image.h"

Image::Image() {}

Image::Image(Image* img)
{
    Image& imgRef = *img;

    imgData = new Pixel[imgRef.width * imgRef.height];

    for (int i = 0; i < imgRef.width * imgRef.height; i++)
    {
        imgData[i].x = imgRef.imgData[i].x;
        imgData[i].y = imgRef.imgData[i].y;
        imgData[i].color = imgRef.imgData[i].color;
    }

    width = imgRef.width;
    height = imgRef.height;
    paddigPerLine = imgRef.paddigPerLine;
}

Image::Image(int widthIn, int heightIn, int bmpType)
{
    width = widthIn;
    height = heightIn;

    if (bmpType != 16 && bmpType != 24 && bmpType != 32)
    {
        bmpType = 24;
    }

    if (imgData != nullptr)
    {
        delete[] imgData;
    }
    imgData = new Pixel[width * height];

    paddigPerLine = 4 - (((bmpType >> 3) * width) % 4);
    if (paddigPerLine == 4)
    {
        paddigPerLine = 0;
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            imgData[x + width * y].x = x;
            imgData[x + width * y].y = y;
        }
    }
}

Image::~Image()
{
    // dtor
    if (imgData != nullptr)
    {
        delete[] imgData;
    }
}

Pixel* Image::getPixel(int x, int y) const { return &(imgData[x + y * width]); }

unsigned char Image::getPixelGrayValue(int x, int y) const { return imgData[x + y * width].color.red; }

void Image::setPixelColor(int x, int y, unsigned char red, unsigned char green, unsigned char blue)
{
    register int idx = x + y * width;
    imgData[idx].color.red = red;
    imgData[idx].color.green = green;
    imgData[idx].color.blue = blue;
}

void Image::setPixelColor(int x, int y, const Color& newPixel)
{
    setPixelColor(x, y, newPixel.red, newPixel.green, newPixel.blue);
}

void Image::drawLine(int xStart, int yStart, int xEnd, int yEnd, unsigned char red, unsigned char green,
    unsigned char blue, int lineWidth)
{
    int dx = abs(xEnd - xStart);
    int sx = xStart < xEnd ? 1 : -1;
    int dy = abs(yEnd - yStart);
    int sy = yStart < yEnd ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    int halfWidth = lineWidth / 2;

    while (1)
    {
        setPixelColor(xStart, yStart, red, green, blue);
        for (int i = -halfWidth; i < halfWidth; i++)
        {
            for (int j = -halfWidth; j < halfWidth; j++)
            {
                setPixelColor(xStart + i, yStart + j, red, green, blue);
            }
        }

        if (xStart == xEnd && yStart == yEnd)
        {
            return;
        }

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            xStart += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            yStart += sy;
        }
    }
}

void Image::drawCross(int x, int y, int crossSize, const Color& newColor, int lineWidth)
{
    drawCross(x, y, crossSize, newColor.red, newColor.green, newColor.blue, lineWidth);
}

void Image::drawCross(
    int x, int y, int crossSize, unsigned char red, unsigned char green, unsigned char blue, int lineWidth)
{
    int xStart = x - crossSize;
    int yStart = y - crossSize;
    int xEnd = x + crossSize;
    int yEnd = y + crossSize;

    drawLine(xStart, y, xEnd, y, red, green, blue, lineWidth);
    drawLine(x, yStart, x, yEnd, red, green, blue, lineWidth);
}

void Image::getPixelsInShape(std::vector<Pixel*>& foundPixel, int xCenter, int yCenter, int radius, Shape shapeToUse)
{
    switch (shapeToUse)
    {
    case CIRCLE_SHAPE:
    {
        int xh, yh, d, dx, dxy;
        xh = 0;
        yh = radius;
        d = 1 - radius;
        dx = 3;
        dxy = -2 * radius + 5;

        while (yh >= xh)
        {
            foundPixel.push_back(getPixel(xCenter + xh, yCenter + yh));
            foundPixel.push_back(getPixel(xCenter + yh, yCenter + xh));
            foundPixel.push_back(getPixel(xCenter + yh, yCenter - xh));
            foundPixel.push_back(getPixel(xCenter + xh, yCenter - yh));
            foundPixel.push_back(getPixel(xCenter - xh, yCenter - yh));
            foundPixel.push_back(getPixel(xCenter - yh, yCenter - xh));
            foundPixel.push_back(getPixel(xCenter - yh, yCenter + xh));
            foundPixel.push_back(getPixel(xCenter - xh, yCenter + yh));

            if (d < 0) // if in circle
            {
                d += dx;
                dx += 2;
                dxy += 2;
                xh++;
            }
            else
            {
                d += dxy;
                dx += 2;
                dxy += 4;
                xh++;
                yh--;
            }
        }
    }
    break;
    case SQUARE_FILLED_SHAPE:
        for (int y = yCenter - radius; y < height && y <= yCenter + radius; y++)
        {
            for (int x = xCenter - radius; x < width && x <= xCenter + radius; x++)
            {
                foundPixel.push_back(getPixel(x, y));
            }
        }
        break;
    case SQUARE_SHAPE:
    {
        const int xStep[] = { 1, 0, -1, 0 };
        const int yStep[] = { 0, 1, 0, -1 };

        int x = xCenter - radius;
        if (x < 0)
        {
            x = 0;
        }

        int y = yCenter - radius;
        if (y < 0)
        {
            y = 0;
        }

        bool addPixel = true;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < radius * 2 + 1; j++)
            {
                x += xStep[i];
                if (x >= width)
                {
                    x = width - 1;
                    addPixel = false;
                }

                y += yStep[i];
                if (y >= height)
                {
                    y = height - 1;
                    addPixel = false;
                }

                if (addPixel)
                {
                    foundPixel.push_back(&(imgData[x + y * width]));
                }
                addPixel = true;
            }
        }
    }
    break;
    }
}
