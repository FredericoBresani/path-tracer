#ifndef __AMBIENT__
#define __AMBIENT__

#include "RGBColor.h"


class Ambient {
    public:
        RGBColor color;
        double ir;
        int depth = 1, nThreds;
        Ambient(RGBColor c, float i, int d, int nT): color(c), ir(i), depth(d), nThreds(nT) {}
        ~Ambient() {}
    
};

#endif