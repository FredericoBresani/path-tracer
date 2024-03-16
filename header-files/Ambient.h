#ifndef __AMBIENT__
#define __AMBIENT__

#include "RGBColor.h"


class Ambient {
    public:
        RGBColor color;
        float ir;
        int depth = 1;
        Ambient(RGBColor c, float i, int d): color(c), ir(i), depth(d) {}
        ~Ambient() {}
};

#endif