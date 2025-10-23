#include "bitmap.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <cmath>
using namespace std;
const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;
const int RESOLUTION_PPM = 2834;

double distance(int x1, int y1, int x2, int y2)
{
    int xdiff = abs(x1 - x2);
    int ydiff = abs(y1 - y2);

    return sqrt((xdiff * xdiff) + (ydiff * ydiff));
}
int distance3d(int r1, int g1, int b1, int r2, int g2, int b2)
{
    int rSqr = (r1 - r2) * (r1 - r2);
    int gSqr = (g1 - g2) * (g1 - g2);
    int bSqr = (b1 - b2) * (b1 - b2);
    return sqrt(rSqr + gSqr + bSqr);
}

void Bitmap::readHeaders(char data[])
{

    for (int i = 5; i > 1; i--)
    {
        _fileSize = _fileSize << 8 | static_cast<unsigned char>(data[i]);
    }

    for (int i = 13; i > 9; i--)
    {
        _dataOffset = _dataOffset << 8 | static_cast<unsigned char>(data[i]);
    }

    for (int i = 21; i > 17; i--)
    {
        _width = _width << 8 | static_cast<unsigned char>(data[i]);
    }

    for (int i = 25; i > 21; i--)
    {
        _height = _height << 8 | static_cast<unsigned char>(data[i]);
    }

    for (int i = 30; i > 27; i--)
    {
        _bitsPerPixel = _bitsPerPixel << 8 | static_cast<unsigned char>(data[i]);
    }

    _imgSize = _fileSize - (FILE_HEADER_SIZE + INFO_HEADER_SIZE);
    cout << "file size: " << _fileSize << endl;
    cout << "offset: " << _dataOffset << endl;
    cout << "width: " << _width << endl;
    cout << "height: " << _height << endl;
    cout << "bits per Pixel: " << _bitsPerPixel << endl;
    cout << "img size: " << _imgSize << endl;
}

// Load existing 24bit bmp
Bitmap::Bitmap(string path)
{

    char BMHeaderData[54];

    fstream inputFile;

    inputFile.open(path, ios::in | ios::binary);
    inputFile.read(reinterpret_cast<char *>(BMHeaderData), sizeof(BMHeaderData));
    if ((BMHeaderData[0] != 'B') | (BMHeaderData[1] != 'M'))
    {
        cout << "invalid file header: " << BMHeaderData << endl;
    }
    else
    {
        readHeaders(BMHeaderData);
    }
    if (_bitsPerPixel != 24)
    {
        throw invalid_argument("error: input file is not a 24-bit bitmap");
    }
    int widthInBytes = _width * BYTES_PER_PIXEL;
    int paddingSize = (4 - (widthInBytes) % 4) % 4;
    cout << "widthinBytes:  " << widthInBytes << endl;
    inputFile.seekg(_dataOffset);

    _pixels.resize(_width * _height);

    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            unsigned char colour[3];
            inputFile.read(reinterpret_cast<char *>(colour), 3);
            _pixels[y * _width + x].b = colour[0];
            _pixels[y * _width + x].g = colour[1];
            _pixels[y * _width + x].r = colour[2];
        }
        inputFile.ignore(paddingSize);
    }

    inputFile.close();
}

void Bitmap::saveImage(string path)
{
    int widthInBytes = _width * BYTES_PER_PIXEL;
    unsigned char padding[3] = {0, 0, 0};

    int paddingSize = (4 - (widthInBytes) % 4) % 4;
    int stride = (widthInBytes) + paddingSize;
    FILE *imageFile = fopen(path.c_str(), "wb");

    unsigned char *fileHeader = createBitmapFileHeader(_height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char *infoHeader = createInfoHeader(_height, _width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    for (int i = 0; i < _height; i++)
    {

        //
        fwrite(_pixels.data() + (i * _width), BYTES_PER_PIXEL, _width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
        // cout << i << endl;
    }
    // cout << "done" << endl;
    fclose(imageFile);
}

void Bitmap::invertColours()
{
    for (int i = 0; i < _pixels.size(); i++)
    {
        _pixels[i].r = 255 - _pixels[i].r;
        _pixels[i].g = 255 - _pixels[i].g;
        _pixels[i].b = 255 - _pixels[i].b;
    }
}

void Bitmap::smooth(int iterations)
{
    for (int i = 0; i < iterations; i++)
    {
        for (int y = 0; y < _height - 1; y++)
        {

            for (int x = 1; x < _width; x++)
            {

                RgbPixel *px = &_pixels[y * _width + x];
                RgbPixel *prev = &_pixels[y * _width + x - 1];

                px->r = (px->r + prev->r) / 2;
                px->g = (px->g + prev->g) / 2;
                px->b = (px->b + prev->b) / 2;
            }
        }
    }
}

// more iterations = more distortion. Effect varies greatly depending on source image.
void Bitmap::glitch(int iterations)
{
    for (int i = 0; i < iterations; i++)
    {
        for (int y = 0; y < _height - 1; y++)
        {

            for (int x = 1; x < _width; x++)
            {

                RgbPixel *px = &_pixels[y * _width + x];
                RgbPixel *prev = &_pixels[y * _width + x - 1];

                px->r = px->r + prev->r / 2;
                px->g = px->g + prev->g / 2;
                px->b = px->b + prev->b / 2;
            }
        }
    }
}

// replaces pixel with solid black if any of the pixel colours are below the given threshold
// todo: refactor
void Bitmap::filter(int rThresh, int gThresh, int bThresh)
{

    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            RgbPixel *px = &_pixels[y * _width + x];
            if ((px->r < rThresh) || (px->g < gThresh))
            {
                px->r = 0;
                px->g = 0;
                px->b = 0;
            }
            if (px->b < bThresh)
            {
                px->r = 0;
                px->g = 0;
                px->b = 0;
            }
        }
    }
}
void Bitmap::smudgeVertical()
{

    for (int y = _height * 0.66; y > 0; y--)
    {
        for (int x = 0; x < _width; x++)
        {
            RgbPixel *above = &_pixels[(y + 1) * _width + x];
            RgbPixel *px = &_pixels[y * _width + x];
            px->r = above->r;
            px->g = above->g;
            px->b = above->b;
        }
    }
}
void Bitmap::smudgeHorizontal(bool leftToRight = true)
{

    for (int y = 0; y < _height; y++)
    {
        if (leftToRight)
        {
            for (int x = _width * 0.65; x < _width; x++)
            {
                RgbPixel *left = &_pixels[y * _width + x - 1];
                RgbPixel *px = &_pixels[y * _width + x];
                px->r = left->r;
                px->g = left->g;
                px->b = left->b;
            }
        }
        else
        {
            for (int x = _width * 0.45; x > 0; x--)
            {
                RgbPixel *right = &_pixels[y * _width + x + 1];
                RgbPixel *px = &_pixels[y * _width + x];
                px->r = right->r;
                px->g = right->g;
                px->b = right->b;
            }
        }
    }
}

void Bitmap::edgeHighlight(int sensitivity)
{
}

// todo: add floyd-steinberg dithering.
// should also let the new colours be passed in as args
void Bitmap::recolour()
{
    RgbPixel teal = RgbPixel(92, 201, 208);
    RgbPixel pink = RgbPixel(227, 0, 114);
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            RgbPixel *px = &_pixels[y * _width + x];

            RgbPixel newPixel;
            int rgbSum = px->r + px->g + px->b;
            if (rgbSum > 392)
            {
                newPixel = teal;
            }
            else
            {
                newPixel = pink;
            }
            px->r = newPixel.r;
            px->g = newPixel.g;
            px->b = newPixel.b;
        }
    }
}
// flips rows horizontally. skip param means every nth row will be flipped instead.
void Bitmap::flipRows(int skip = 0)
{
    int midpoint;
    if (_width % 2 != 0)
    {
        midpoint = ((_width - 1) / 2) - 1;
    }
    else
    {
        midpoint = (_width / 2) - 1;
    }

    for (int row = 0; row < _height; row++)
    {
        for (int position = 0; position < midpoint; position++)
        {
            int mirrorIndex = _width - position;

            RgbPixel *px = &_pixels[row * _width + position];
            RgbPixel *swapTarget = &_pixels[row * _width + mirrorIndex];
            RgbPixel temp = *swapTarget;

            swapTarget->r = px->r;
            swapTarget->g = px->g;
            swapTarget->b = px->b;
            px->r = temp.r;
            px->g = temp.g;
            px->b = temp.b;
        }
        row += skip;
    }
}
Bitmap::~Bitmap()
{
}

RgbPixel::RgbPixel()
{
}

RgbPixel::RgbPixel(unsigned char red, unsigned char green, unsigned char blue)
{
    r = red;
    b = blue;
    g = green;
}
unsigned char *Bitmap::createBitmapFileHeader(int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,
        0, /// signature
        0,
        0,
        0,
        0, /// image file size in bytes
        0,
        0,
        0,
        0, /// reserved
        0,
        0,
        0,
        0, /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char *Bitmap::createInfoHeader(int height, int width)
{
    static unsigned char infoHeader[] = {
        0,
        0,
        0,
        0, /// header size
        0,
        0,
        0,
        0, /// image width
        0,
        0,
        0,
        0, /// image height
        0,
        0, /// number of color planes
        0,
        0, /// bits per pixel
        0,
        0,
        0,
        0, /// compression
        0,
        0,
        0,
        0, /// image size
        0,
        0,
        0,
        0, /// horizontal resolution
        0,
        0,
        0,
        0, /// vertical resolution
        0,
        0,
        0,
        0, /// colors in color table
        0,
        0,
        0,
        0, /// important color count
    };

    infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

    // 15 - 19 signals the compression method.

    // img size
    infoHeader[20] = (unsigned char)(_imgSize);
    infoHeader[21] = (unsigned char)(_imgSize >> 8);
    infoHeader[22] = (unsigned char)(_imgSize >> 16);
    infoHeader[23] = (unsigned char)(_imgSize >> 24);

    // horizontal resolution
    infoHeader[24] = (unsigned char)(RESOLUTION_PPM);
    infoHeader[25] = (unsigned char)(RESOLUTION_PPM >> 8);
    infoHeader[26] = (unsigned char)(RESOLUTION_PPM >> 16);
    infoHeader[27] = (unsigned char)(RESOLUTION_PPM >> 24);
    // vertical resolution
    infoHeader[28] = (unsigned char)(RESOLUTION_PPM);
    infoHeader[29] = (unsigned char)(RESOLUTION_PPM >> 8);
    infoHeader[30] = (unsigned char)(RESOLUTION_PPM >> 16);
    infoHeader[31] = (unsigned char)(RESOLUTION_PPM >> 24);

    return infoHeader;
}