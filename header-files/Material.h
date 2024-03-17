#ifndef __MATERIAL__
#define __MATERIAL__

#include "RGBColor.h"


class Material 
{
    public:
        RGBColor color;
        double kd, ks, ka, kt, kr, phong, ior;
        bool getShadows;
        Material(RGBColor c, double d, double s, double a, double r, double t, double p, bool ms, double i): 
            color(c), kd(d), ks(s), ka(a), kr(r), kt(t), phong(p), getShadows(ms), ior(i) {}
        ~Material() {}
        
};


#endif