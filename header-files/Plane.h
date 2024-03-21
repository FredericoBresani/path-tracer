#ifndef __PLANE__
#define __PLANE__

#include "Object.h"
#include "Points.h"
#include "Vectors.h"
#include "Definitions.h"
#include "RGBColor.h"
#include "Material.h"
#include <vector>


class Plane: public Object 
{
    public:
        Vec3D normal;
        Point3D pp;
        Material *material;
        bool castShadows;
        Plane(const Vec3D &n, const Point3D &p, Material *m, bool s): normal(n), pp(p), material(m), castShadows(s) {}
        ~Plane() {}
        bool rayObjectIntersect(const Ray &ray, double *tmin, HitInfo &info);
    private:
        RGBColor getColor();
        double getKd();
        double getKs();
        double getKa();
        double getKr();
        double getKt();
        double getPhongExp();
        double getIor();
        Vec3D getNormal(const Point3D& hit, const Ray &ray);
        bool getShadows();
        bool getCastShadows();
        std::vector<Point3D> sampleObject();
        char getObjectType();
};

bool Plane::rayObjectIntersect(const Ray &ray, double *tmin, HitInfo& info)
{
    double t = ((pp - ray.origin) * this->normal) / (ray.direction * this->normal);
    if (t > kEpsilon)
    {
        (*tmin) = t;
        if (!(this->getKd() == 0 && this->getKr() == 0 && this->getKs() == 0)) { // situation where is not beeing used as a subroutine
            info.hit_object = true;                                         // to the triangle or mesh intersection test
        }
        return true;

    } else {
        return false;
    }
}
RGBColor Plane::getColor()
{
    return this->material->color;
}
double Plane::getKd()
{
    return this->material->kd;
}
double Plane::getKs()
{
    return this->material->ks;
}
double Plane::getKa()
{
    return this->material->ks;
}
double Plane::getKr()
{
    return this->material->kr;
}
double Plane::getKt()
{
    return this->material->kt;
}
double Plane::getPhongExp()
{
    return this->material->phong;
}
double Plane::getIor()
{
    return this->material->ior;
}
Vec3D Plane::getNormal(const Point3D &hit, const Ray &ray)
{
    return Vec3D::normalize(normal);
}
bool Plane::getShadows()
{
    return this->material->getShadows;
}
bool Plane::getCastShadows() 
{
    return this->castShadows;
}
std::vector<Point3D> Plane::sampleObject()
{
    std::vector<Point3D> samples = {Point3D()};
    return samples;
}
char Plane::getObjectType()
{
    return 'p';
}

#endif