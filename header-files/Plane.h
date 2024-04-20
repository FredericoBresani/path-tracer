#ifndef __PLANE__
#define __PLANE__

#include "Object.h"
#include "Definitions.h"


class Plane: public Object 
{
    public:
        Vec3D normal;
        Point3D pp;
        std::shared_ptr<Material> material;
        bool castShadows;
        Plane(const Vec3D &n, const Point3D &p, Material *m, bool s): normal(n), pp(p), castShadows(s) {
            this->normal = n;
            this->pp = p;
            this->castShadows = s;
            this->material = std::make_shared<Material>((*m));
        }
        ~Plane() {}
        bool rayObjectIntersect(const Ray &ray, double *tmin, std::shared_ptr<HitInfo> info);
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

bool Plane::rayObjectIntersect(const Ray &ray, double *tmin, std::shared_ptr<HitInfo> info)
{
    std::unique_lock<std::mutex> lock(objectLock);
    double t = ((pp - ray.origin) * this->normal) / (ray.direction * this->normal);
    if (t > kEpsilon)
    {
        (*tmin) = t;
        info->hit_object = true;                                      
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
    std::unique_lock<std::mutex> lock(objectLock);
    std::vector<Point3D> samples = {Point3D()};
    return samples;
}
char Plane::getObjectType()
{
    return 'p';
}

#endif