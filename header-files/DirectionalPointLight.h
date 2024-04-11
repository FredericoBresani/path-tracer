#ifndef __DIRECTIONALPOINTLIGHT__
#define __DIRECTIONALPOINTLIGHT__

#include "HitInfo.h"
#include "Light.h"

class DirectionalPointLight: public Light {
    public:
        DirectionalPointLight(const Point3D& pos, const RGBColor& color, bool s, int n, Vec3D directional): 
            lightPos(pos), lightColor(color), directionalVec(directional) 
        {
            this->shadows = s;
            this->n_samples = n;
        }
        ~DirectionalPointLight() {}
        std::vector<Point3D> getLightSamples();
        
    private:
        Point3D lightPos;
        RGBColor lightColor;
        Vec3D directionalVec;
        Vec3D getDirection(HitInfo &hit);
        RGBColor incidentRadiance(HitInfo &hit);
        Point3D getPos();
        RGBColor getColor();
        bool castShadows();
        bool isExtense();
        void sampleLight();
        Object* getLightModel();
        std::vector<Point3D> getMeshControlPoints();
        void setColor(RGBColor &c);
        bool isDirectional();
        Vec3D getDirectionalVec();
        
};

Vec3D DirectionalPointLight::getDirection(HitInfo &hit)
{
    return Vec3D::normalize(this->lightPos - hit.hit_location);
}

RGBColor DirectionalPointLight::incidentRadiance(HitInfo &hit)
{
    return lightColor;
}

Point3D DirectionalPointLight::getPos()
{
    return this->lightPos;
}

RGBColor DirectionalPointLight::getColor()
{
    return this->lightColor;
}

bool DirectionalPointLight::castShadows()
{
    return this->shadows;
}

bool DirectionalPointLight::isExtense()
{
    return false;
}

void DirectionalPointLight::sampleLight()
{
    this->light_samples.push_back(this->lightPos);
}

Object* DirectionalPointLight::getLightModel()
{
    return nullptr;
}

std::vector<Point3D> DirectionalPointLight::getLightSamples()
{
    return this->light_samples;
}

std::vector<Point3D> DirectionalPointLight::getMeshControlPoints()
{
    return {Point3D()};
}

void DirectionalPointLight::setColor(RGBColor &c)
{
    this->lightColor = c;
}

bool DirectionalPointLight::isDirectional()
{
    return true;
}

Vec3D DirectionalPointLight::getDirectionalVec()
{
    return Vec3D();
}



#endif