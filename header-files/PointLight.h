#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Light.h"


class PointLight: public Light {
    public:
        PointLight(const Point3D& pos, const RGBColor& color, bool s, int n) {
            this->lightPos = pos;
            this->lightColor = color;
            this->shadows = s;
            this->n_samples = n;
        }
        PointLight(Light &l) {
            this->lightPos = l.getPos();
            this->lightColor = l.getColor();
            this->shadows = 1;
            this->n_samples = 0;
        }
        PointLight(std::shared_ptr<PointLight> &l) {
            this->lightPos = l->getPos();
            this->lightColor = l->getColor();
            this->shadows = 1;
            this->n_samples = 0;
        }
        PointLight(PointLight *l) {
            this->lightPos = l->getPos();
            this->lightColor = l->getColor();
            this->shadows = 1;
            this->n_samples = 0;
        }
        ~PointLight() {}
        std::vector<Point3D> getLightSamples();
        Point3D getPos();
        
    private:
        Point3D lightPos;
        RGBColor lightColor;
        Vec3D getDirection(HitInfo &hit);
        RGBColor incidentRadiance(HitInfo &hit);
        RGBColor getColor();
        bool castShadows();
        bool isExtense();
        void sampleLight();
        std::shared_ptr<Object> getLightModel();
        std::vector<Point3D> getMeshControlPoints();
        void setColor(RGBColor &c);
        bool isDirectional();
        Vec3D getDirectionalVec();
        
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
    return (*this).lightPos;
}

RGBColor PointLight::getColor()
{
    return (*this).lightColor;
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
    std::unique_lock<std::mutex> lock(lightLock);
    this->light_samples.push_back(this->lightPos);
}

std::shared_ptr<Object> PointLight::getLightModel()
{
    return nullptr;
}

std::vector<Point3D> PointLight::getLightSamples()
{
    return this->light_samples;
}

std::vector<Point3D> PointLight::getMeshControlPoints()
{
    return {Point3D()};
}

void PointLight::setColor(RGBColor &c)
{
    std::unique_lock<std::mutex> lock(lightLock);
    this->lightColor = c;
}

bool PointLight::isDirectional()
{
    return false;
}

Vec3D PointLight::getDirectionalVec()
{
    return Vec3D();
}



#endif