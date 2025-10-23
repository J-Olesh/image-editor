

#include "bitmap.hpp"

int main()
{
    /*
    Replace with path of image you want to edit.
    The program will create a new copy rather than alrtering the original.
    The input image MUST be a 24-bit .bmp, 32-bit will not work.
    */
    Bitmap test("./input_images/testImage.bmp");
    // test.glitch(1);
    // test.flipRows(0);
    // test.recolour();
    test.saveImage("../out/output.bmp");
}
