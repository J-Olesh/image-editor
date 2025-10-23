
#include <stdio.h>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include "bitmap.hpp"

int main()
{
    /*
    Replace with path of image you want to edit.
    The program will create a new copy rather than alrtering the original.
    The input image MUST be a 24-bit .bmp, 32-bit will not work.
    */
    Bitmap test("./input_images/testImage.bmp");
    // // bm.glitch(1);
    // // bm.flipRows(0);

    // //  bm.recolour();
    test.saveImage("../out/output.bmp");
}
