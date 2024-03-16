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
        bool rayObjectIntersect(const Ray &ray, double *tmin, HitInfo& info)
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
        RGBColor getColor() const
        {
            return this->material->color;
        }
        double getKd() const
        {
            return this->material->difuseK;
        }
        double getKs() const
        {
            return this->material->specularK;
        }
        double getKa() const
        {
            return this->material->ambientalK;
        }
        double getKr() const
        {
            return this->material->reflectiveK;
        }
        double getKt() const
        {
            return this->material->transmissionK;
        }
        double getPhongExp() const
        {
            return this->material->roughK;
        }
        double getIor() const
        {
            return this->material->ior;
        }
        Vec3D getNormal(const Point3D &hit, const Ray &ray) const
        {
            return Vec3D::normalize(normal);
        }
        bool getShadows() const
        {
            return this->material->getShadows;
        }
        bool getCastShadows() const 
        {
            return this->castShadows;
        }
        std::vector<Point3D> sampleObject()
        {
            std::vector<Point3D> samples = {Point3D()};
            return samples;
        }
};

#endif