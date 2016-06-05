#include "ImageLoader.h"

#include <cassert>

Image* ImageFramework::readAndParseJPEGFile(const std::string& fileName)
{
    int channels;                       //  3 =>RGB   4 =>RGBA
    unsigned char* rowptr[1];           // pointer to an array
    struct jpeg_decompress_struct info; // for our jpeg info
    struct jpeg_error_mgr err;          // the error handler

    FILE* file = fopen(fileName.c_str(), "rb"); // open the file

    if (file == nullptr)
    {
        return nullptr;
    }

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info); // fills info structure

    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE); // read jpeg file header

    // decompress the file
    if (!jpeg_start_decompress(&info))
    {
        jpeg_destroy_decompress(&info);
        return nullptr;
    }

    Image* img = new Image();
    if (img == nullptr)
    {
        return nullptr;
    }

    Image& imgRef = *img;

    // set width and height
    imgRef.width = info.output_width;
    imgRef.height = info.output_height;
    channels = info.num_components;

    //--------------------------------------------
    // read scanlines one at a time & put bytes
    //    in currData[] array.
    //--------------------------------------------
    unsigned char* currData = new unsigned char[imgRef.width * imgRef.height * channels];
    while ((int)info.output_scanline < imgRef.height)
    {
        // Enable jpeg_read_scanlines() to fill our currData array
        rowptr[0] = (unsigned char*)currData + channels * imgRef.width * info.output_scanline;

        if (jpeg_read_scanlines(&info, rowptr, 1) != 1)
        {
            delete[] currData;
            delete img;
            jpeg_destroy_decompress(&info);
            return nullptr;
        }
    }

    if (!jpeg_finish_decompress(&info))
    {
        jpeg_destroy_decompress(&info);
        delete img;
        delete[] currData;
        return nullptr;
    }

    fclose(file);

    const int imgSize = imgRef.width * imgRef.height;
    imgRef.imgData = new Pixel[imgSize];

    int idx = 0;
    int x = 0;
    int y = 0;
    unsigned char* beginningimgData = currData;
    while (1)
    {
        if (idx >= imgSize)
        {
            break;
        }

        imgRef.imgData[idx].color.red = *currData;
        currData++;
        imgRef.imgData[idx].color.green = *currData;
        currData++;
        imgRef.imgData[idx].color.blue = *currData;
        currData++;

        if (channels == 4)
        {
            currData++;
        }

        imgRef.imgData[idx].x = x;
        imgRef.imgData[idx].y = y;

        x++;
        if (x >= imgRef.width)
        {
            y++;
            x = 0;
        }
        idx++;
    }

    delete[] beginningimgData;
    return img;
}

Image* ImageFramework::readAndParseBMPFile(const std::string& imageFile)
{
    std::streampos size;
    char* memblock;

    std::ifstream file(imageFile, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(memblock, size);
        file.close();

        if (size > NORMAL_BMP_HEADER_SIZE)
        {
            if (memblock[0] == 'B' && memblock[1] == 'M')
            {
                return parseBMPFile(memblock, size);
            }
            else
            {
                return nullptr;
                // no bmp
            }
        }
        else
        {
            return nullptr;
            // no bmp
        }

        delete[] memblock;
    }
    else
    {
        return nullptr;
        // Unable to open file
    }
    return nullptr;
}

Image* ImageFramework::readAndParsePNGFile(const std::string& imageFile)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;

    unsigned char header[8]; // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE* fp = fopen(imageFile.c_str(), "rb");
    if (!fp)
    {
        return nullptr;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
        return nullptr;
    }

    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
        return nullptr;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        return nullptr;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    Image* img = new Image();
    if (img == nullptr)
    {
        return nullptr;
    }

    img->width = png_get_image_width(png_ptr, info_ptr);
    img->height = png_get_image_height(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    /* Change a paletted/grayscale image to RGB */
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
        png_set_expand(png_ptr);

    if (color_type & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    /* read file */

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * img->height);
    for (int y = 0; y < img->height; y++)
    {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, row_pointers);

    fclose(fp);

    const int imgSize = img->width * img->height;
    img->imgData = new Pixel[imgSize];

    int idx = 0;

    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            const int newX = x * 3;
            img->imgData[idx].color.red = row_pointers[y][newX];
            img->imgData[idx].color.green = row_pointers[y][newX + 1];
            img->imgData[idx].color.blue = row_pointers[y][newX + 2];
            img->imgData[idx].x = x;
            img->imgData[idx].y = y;
            idx++;
        }
    }

    for (int y = 0; y < img->height; y++)
    {
        free(row_pointers[y]);
    }
    free(row_pointers);

    return img;
}

Image* ImageFramework::parseBMPFile(char* memblock, unsigned int size)
{
    bool bottomUpImage = true;

    unsigned char* currData = (unsigned char*)memblock;

    currData += 10;
    unsigned int bmpOffset = *((unsigned int*)currData);
    currData += 8;
    int bmpWidth = *((int*)currData);
    currData += 4;
    int bmpHeight = *((int*)currData);
    if (bmpHeight < 0)
    {
        bottomUpImage = false;
        bmpHeight = abs(bmpHeight);
    }
    currData += 4;
    while (*currData != 1)
    {
        currData++;
    }
    currData += 2;
    unsigned short bmpType = *((unsigned short*)currData); // 16, 24 or 32 bpp

    if (bmpType != 16 && bmpType != 24 && bmpType != 32)
    {
        bmpType = *((unsigned int*)currData); // 16, 24 or 32 bpp
        currData += 2;
        if (bmpType != 16 && bmpType != 24 && bmpType != 32)
        {
            return nullptr;
        }
    }
    currData += 2;
    unsigned int compression = *((unsigned int*)currData); // should be 0
    if (compression != 0)
    {
        return nullptr;
    }

    currData = (unsigned char*)memblock + bmpOffset;

    Image* img = new Image();
    if (img == nullptr)
    {
        return nullptr;
    }

    img->imgData = new Pixel[bmpWidth * bmpHeight];

    img->paddigPerLine = 4 - (((bmpType >> 3) * bmpWidth) % 4);
    if (img->paddigPerLine == 4)
    {
        img->paddigPerLine = 0;
    }

    unsigned int totalSize = bmpHeight * bmpWidth;

    parseBMPData(img, currData, bmpWidth, bmpHeight, totalSize, bottomUpImage, bmpType);

    return img;
}

void ImageFramework::parseBMPData(Image* img, unsigned char* currData, int bmpWidth, int bmpHeight,
    unsigned int totalSize, bool bottomUpImage, int bmpType)
{
    assert(img != nullptr);
    Image& imgRef = *img;
    unsigned int idx = 0;
    int x = 0;
    int y = 0;
    int addY = 1;

    if (bottomUpImage == true)
    {
        y = bmpHeight - 1;
        addY = -1;
    }

    while (1)
    {
        idx = x + y * bmpWidth;
        if (idx >= totalSize)
        {
            break;
        }
        if (bmpType == 24)
        {
            imgRef.imgData[idx].color.blue = *currData;
            currData++;
            imgRef.imgData[idx].color.green = *currData;
            currData++;
            imgRef.imgData[idx].color.red = *currData;
            currData++;
        }
        else if (bmpType == 32)
        {
            imgRef.imgData[idx].color.blue = *currData;
            currData++;
            imgRef.imgData[idx].color.green = *currData;
            currData++;
            imgRef.imgData[idx].color.red = *currData;
            currData++;
            imgRef.imgData[idx].color.alphaChannel = *currData;
            currData++;
        }
        else if (bmpType == 16)
        {
            imgRef.imgData[idx].color.blue = *((unsigned int*)currData) & 0x0000001F;
            imgRef.imgData[idx].color.green = *((unsigned int*)currData) & 0x000003E0;
            imgRef.imgData[idx].color.red = *((unsigned int*)currData) & 0x00007C00;
            currData += 2;
        }
        imgRef.imgData[idx].x = x;
        imgRef.imgData[idx].y = y;

        x++;
        if (x >= bmpWidth)
        {
            y += addY;
            x = 0;
            currData += imgRef.paddigPerLine;
        }
    }

    imgRef.width = bmpWidth;
    imgRef.height = bmpHeight;
}

#if REMOVE_WINDOWS_SPECIFIC != 1

Image* ImageFramework::captureWindow(const std::string& str)
{
    HWND hWnd = FindWindow((LPCSTR)str.c_str(), (LPCSTR)str.c_str());

    return captureWindowFromHandle(hWnd);
}

Image* ImageFramework::captureWindowFromHandle(HWND hWnd)
{
    Image* img = new Image();
    if (img == nullptr)
    {
        return nullptr;
    }

    HDC hScreenDC = GetDC(hWnd);
    int w = GetDeviceCaps(hScreenDC, HORZRES);
    int h = GetDeviceCaps(hScreenDC, VERTRES);

    HDC hdc2 = CreateCompatibleDC(hScreenDC);

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h; // bottomUp
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    paddigPerLine = 4 - ((3 * w) % 4);
    if (paddigPerLine == 4)
    {
        img->paddigPerLine = 0;
    }

    char* memblock;
    HBITMAP hBitmap = CreateDIBSection(hdc2, &bmi, DIB_RGB_COLORS, (void**)&memblock, 0, 0);
    SelectObject(hdc2, hBitmap);
    BitBlt(hdc2, 0, 0, w, h, hScreenDC, 0, 0, SRCCOPY);

    img->imgData = new Pixel[w * h];
    parseBMPData(img, (unsigned char*)memblock, w, h, w * h, true, 24);

    ReleaseDC(hWnd, hScreenDC);
    DeleteObject(hBitmap);
    if (hdc2)
    {
        DeleteDC(hdc2);
    }

    return img;
}
#endif

Image* ImageFramework::readFile(const std::string& imageFile)
{
    Image* img = readAndParseBMPFile(imageFile);

    if (img == nullptr)
    {
        img = readAndParsePNGFile(imageFile);
        if (img == nullptr)
        {
            img = readAndParseJPEGFile(imageFile);
            if (img == nullptr)
            {
                return nullptr;
            }
        }
    }
    return img;
}
