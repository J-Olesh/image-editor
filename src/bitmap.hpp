#ifndef BITMAP_H
#define BITMAP_H
#include <vector>
#include <string>

using namespace std;
struct RgbPixel
{

    unsigned char b, g, r;
    RgbPixel();
    RgbPixel(unsigned char red, unsigned char green, unsigned char blue);
};

class Bitmap
{

private:
    void readHeaders(char data[]);
    unsigned char *createBitmapFileHeader(int height, int stride);
    unsigned char *createInfoHeader(int height, int width);
    int _dataOffset;
    int _fileSize;
    vector<RgbPixel> _pixels;
    int _width;
    int _height;
    int _bitsPerPixel;
    int _imgSize; // size of raw bitmap data. needed for header if compression is not BI_RGB

public:
    Bitmap(string path);
    void saveImage(string path);
    void invertColours();
    void smooth(int iterations);
    void glitch(int iterations);
    void filter(int rThresh, int gThresh, int bThresh);
    void smudgeVertical();
    void smudgeHorizontal(bool leftToRight);
    void edgeHighlight(int sensitivity);
    void recolour();
    void flipRows(int skip);

    ~Bitmap();
};

#endif