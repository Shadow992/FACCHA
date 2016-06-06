#include "ImageSaver.h"

#include <cassert>

int ImageFramework::saveAsPNG(Image* img, const std::string& fileName)
{
    assert(img != nullptr);
    Image& imgRef = *img;

    FILE* fp;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    int x, y;
    png_byte** row_pointers = nullptr;
    int pixel_size = 3;
    int depth = 8;

    fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr)
    {
        return -3;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr)
    {
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr)
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -2;
    }

    /* Set image attributes. */

    png_set_IHDR(png_ptr, info_ptr, imgRef.width, imgRef.height, depth, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers = reinterpret_cast<png_byte**>(png_malloc(png_ptr, imgRef.height * sizeof(png_byte*)));
    for (y = 0; y < imgRef.height; ++y)
    {
        png_byte* row = reinterpret_cast<png_byte*>(png_malloc(png_ptr, sizeof(uint8_t) * imgRef.width * pixel_size));
        row_pointers[y] = row;
        for (x = 0; x < imgRef.width; ++x)
        {
            *row++ = imgRef.getPixel(x, y)->color.red;
            *row++ = imgRef.getPixel(x, y)->color.green;
            *row++ = imgRef.getPixel(x, y)->color.blue;
        }
    }

    /* Write the image data to "fp". */

    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

    for (y = 0; y < imgRef.height; y++)
    {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);
    return 0;
}

void ImageFramework::saveAsBMP(Image* img, const std::string& fileName)
{
    assert(img != nullptr);
    Image& imgRef = *img;
    std::ofstream bmpFile;
    bmpFile.open(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
    int tmpInt;
    if (bmpFile.is_open())
    {
        int bmpType = 24;
        imgRef.paddigPerLine = 4 - (((bmpType >> 3) * imgRef.width) % 4);
        if (imgRef.paddigPerLine == 4)
        {
            imgRef.paddigPerLine = 0;
        }

        bmpFile << 'B' << 'M';
        tmpInt = imgRef.width * imgRef.height * 3 + imgRef.paddigPerLine * imgRef.height + 54;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 0;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 54;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 40;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = imgRef.width;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = -imgRef.height;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 1;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 2);
        tmpInt = 24;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 2);
        tmpInt = 0;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = imgRef.width * imgRef.height * 3 + imgRef.paddigPerLine * imgRef.height;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 0;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 0;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 0;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);
        tmpInt = 0;
        bmpFile.write(reinterpret_cast<char*>(&tmpInt), 4);

        std::vector<unsigned char> imageData(
            imgRef.width * imgRef.height * 3 + imgRef.paddigPerLine * imgRef.height + 5);

        int overallPos = 0;
        unsigned char* imageDataPtr = imageData.data();
        for (int y = 0; y < imgRef.height; y++)
        {
            for (int x = 0; x < imgRef.width; x++)
            {
                memcpy(imageDataPtr, reinterpret_cast<unsigned char*>(&(imgRef.imgData[overallPos].color)), 3);
                overallPos++;
                imageDataPtr += 3;
            }

            for (unsigned int i = 0; i < imgRef.paddigPerLine; i++)
            {
                *imageDataPtr = 0;
                imageDataPtr++;
            }
        }

        bmpFile.write(reinterpret_cast<char*>(imageData.data()),
            imgRef.width * imgRef.height * 3 + imgRef.paddigPerLine * imgRef.height);

        bmpFile.flush();
        bmpFile.close();
    }
}
