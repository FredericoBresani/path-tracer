#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Light.h"
#include "Points.h"
#include "RGBColor.h"
#include "HitInfo.h"
#include <vector>


class PointLight: public Light {
    public:
        PointLight(const Point3D& pos, const RGBColor& color, bool s, int n): lightPos(pos), lightColor(color) {
            this->shadows = s;
            this->n_samples = n;
        }
        ~PointLight() {}
        std::vector<Point3D> getLightSamples();
        
    private:
        Point3D lightPos;
        RGBColor lightColor;
        Vec3D getDirection(HitInfo &hit);
        RGBColor incidentRadiance(HitInfo &hit);
        Point3D getPos();
        RGBColor getColor();
        bool castShadows();
        bool isExtense();
        void sampleLight();
        Object* getLightModel();
        
};

Vec3D PointLight::getDirection(HitInfo &hit)
{
    return Vec3D::normalize(this->lightPos - hit.hit_location);
}

RGBColor PointLight::incidentRadiance(HitInfo &hit)
{
    return lightColor;
}

Point3D PointLight::getPos()
{
    return this->lightPos;
}

RGBColor PointLight::getColor()
{
    return this->lightColor;
}

bool PointLight::castShadows()
{
    return this->shadows;
}

bool PointLight::isExtense()
{
    return false;
}

void PointLight::sampleLight()
{
    this->light_samples.push_back(this->lightPos);
}

Object* PointLight::getLightModel()
{
    return nullptr;
}

std::vector<Point3D> PointLight::getLightSamples()
{
    return this->light_samples;
}



#endif