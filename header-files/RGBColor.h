#ifndef __RGB__
#define __RGB__

#include "Vectors.h"

template <typename T> class COLOR 
{
    public:
        T r, g, b;
        COLOR(): r(T(0)), g(T(0)), b(T(0)) {}
        COLOR(T _r): r(T(_r)), g(T(_r)), b(T(_r)) {}
        COLOR(T _r, T _g, T _b): r(T(_r)), g(T(_g)), b(T(_b)) {}
        
        COLOR<T> operator + (const COLOR<T> &v) const { return COLOR(r + v.r, g + v.g, b + v.b); }
        COLOR<T> operator - (const COLOR<T> &v) const { return COLOR(r - v.r, g - v.g, b - v.b); }
        COLOR<T> operator ^ (const COLOR<T> &v) const { return COLOR(r*v.r, g*v.g, b*v.b); }
        COLOR<T> operator % (const COLOR<T> &v) const { return COLOR(r/v.r, g/v.g, b/v.b); } 
        COLOR<T> operator * (const T &t) const { return COLOR(r*t, g*t, b*t); }
        COLOR<T> operator / (const T &t) const { return COLOR(r/t, g/t, b/t); }
        

        COLOR<T> operator & (const Vec3<T> &v) const { return COLOR(r + v.x, g + v.y, b + v.z); }
        //COLOR<T> operator ! (const Vec3<T> &v) const { return COLOR(r - v.x, g - v.y, b - v.z); }
};

typedef COLOR<double> RGBColor;

#endif