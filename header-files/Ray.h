#ifndef __RAY__
#define __RAY__

#include "Points.h"
#include "Vectors.h"

class Ray {
    public:
        Point3D origin;
        Vec3D direction;
        Ray(const Point3D &p, const Vec3D &d): origin(p), direction(d) {}
        Ray(const Ray &ray);
        Ray& operator = (const Ray& rhs); 
        ~Ray() {}
};

#endif