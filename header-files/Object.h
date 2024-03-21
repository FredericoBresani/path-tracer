#ifndef __OBJECT__
#define __OBJECT__

#include "Points.h"
#include "RGBColor.h"
#include "Vectors.h"
#include "Material.h"
#include "Ray.h"
#include "HitInfo.h"
#include <vector>
#include <stdlib.h>


class Object {
    public:
        Material *material;
        bool castShadows;
        Object() {}
        virtual ~Object() {}
        virtual bool rayObjectIntersect(const Ray &ray, double *tmin, HitInfo &info) = 0;
        virtual RGBColor getColor() = 0;
        virtual double getKd() = 0;
        virtual double getKs() = 0;
        virtual double getKa() = 0;
        virtual double getKr() = 0;
        virtual double getKt() = 0;
        virtual double getIor() = 0;
        virtual double getPhongExp() = 0;
        virtual Vec3D getNormal(const Point3D &hit, const Ray &ray) = 0;
        virtual bool getShadows() = 0;
        virtual bool getCastShadows() = 0;
        virtual std::vector<Point3D> sampleObject() = 0;
        virtual char getObjectType() = 0;
};

#endif