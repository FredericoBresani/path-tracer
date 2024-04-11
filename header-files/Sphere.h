#ifndef __SPHERE__
#define __SPHERE__


#include "Definitions.h"
#include "Object.h"
#include <math.h>


class Sphere: public Object 
{
    public:
        Point3D center;
        double radius;
        std::shared_ptr<Material> material;
        bool castShadows;
        Sphere(const Point3D &c, double r, Material *m, bool s): center(c), radius(r), castShadows(s) {
            this->material = std::make_shared<Material>((*m));
        }
        ~Sphere() {}
    private:
        bool rayObjectIntersect(const Ray &ray, double *tmin, HitInfo &info);
        RGBColor getColor();
        double getKd();
        double getKs();
        double getKa();
        double getKt();
        double getKr();
        double getPhongExp();
        double getIor();
        Vec3D getNormal(const Point3D &hit, const Ray &ray);
        bool getShadows();
        bool getCastShadows();
        std::vector<Point3D> sampleObject();
        char getObjectType();
};

bool Sphere::rayObjectIntersect(const Ray &ray, double *tmin, HitInfo& info)
{
    auto a = pow(Vec3D::norma(ray.direction), 2.0);
    auto b = ((ray.origin - this->center) * ray.direction) * 2.0;
    auto c = ((this->center ^ this->center) + (ray.origin ^ ray.origin)) + (-2.0)*(ray.origin ^ this->center) - (this->radius*this->radius);
    auto delta = (b*b) - 4.0*a*c; 
    if (delta == 0.0)
    {
        auto t = (b*b)/(2.0*a);
        if (t > kEpsilon)
        {
            (*tmin) = t;
            info.hit_object = true;
            return true;
        } else {
            return false;
        }
    }
    else if (delta > 0.0)
    {
        auto sqrtDelta = pow(delta, 0.5);
        auto t1 = (((-1.0)*b) + sqrtDelta)/(2.0*a);
        auto t2 = (((-1.0)*b) - sqrtDelta)/(2.0*a);
        if ((t1 < t2) && (t1 > kEpsilon))
        {
            (*tmin) = t1;
            info.hit_object = true;
            return true;
        }
        else if (t2 > kEpsilon)
        {
            (*tmin) = t2;
            info.hit_object = true;
            return true;
        }
        return false;
    }
    return false;
}
RGBColor Sphere::getColor()
{
    return this->material->color;
}
double Sphere::getKd()
{
    return this->material->kd;
}
double Sphere::getKs()
{
    return this->material->ks;
}
double Sphere::getKa()
{
    return this->material->ks;
}
double Sphere::getKr()
{
    return this->material->kr;
}
double Sphere::getKt()
{
    return this->material->kt;
}
double Sphere::getPhongExp()
{
    return this->material->phong;
}
double Sphere::getIor()
{
    return this->material->ior;
}
Vec3D Sphere::getNormal(const Point3D &hit, const Ray &ray)
{
    return Vec3D::normalize(hit - this->center);
}
bool Sphere::getShadows()
{
    return this->material->getShadows;
}
bool Sphere::getCastShadows()
{
    return this->castShadows;
}
std::vector<Point3D> Sphere::sampleObject()
{
    std::vector<Point3D> samples = {Point3D()};
    return samples;
}
char Sphere::getObjectType()
{
    return 's';
}

#endif