#ifndef __SCREEN__
#define __SCREEN__

#include <vector>
#include "RGBColor.h"

class Screen {
    public:
        int donePixels = 0;
        std::vector<RGBColor> pixels;

        Screen() {}
        Screen(int pixelQtn) {
            this->pixels = std::vector<RGBColor>(pixelQtn, RGBColor());
        }
        ~Screen() {}

        void setPixelQtn(int qtn) {
            this->pixels = std::vector<RGBColor>(qtn, RGBColor());
        }

        void checkPixel() {
            this->donePixels++;
        }

        int getPixelQtn() {
            return this->pixels.size();
        }

        int getDonePixelsQtn() {
            return this->donePixels;
        }
};



#endif