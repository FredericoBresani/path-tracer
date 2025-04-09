#ifndef __AMBIENT__
#define __AMBIENT__

#include "RGBColor.h"


class Ambient {
    public:
        RGBColor color, backgroundColor;
        double ir;
        int depth = 1, nThreds;
        Ambient(RGBColor c, RGBColor bg, float i, int d, int nT): color(c), backgroundColor(bg), ir(i), depth(d), nThreds(nT) {}
        ~Ambient() {}
    
};

#endif