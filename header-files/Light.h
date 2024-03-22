#ifndef __LIGHT__
#define __LIGHT__

#include "Points.h"
#include <vector>
#include "RGBColor.h"
#include "Object.h"

class Light
{
    public:
        virtual RGBColor getColor() = 0;
        virtual bool castShadows() = 0;
        virtual void sampleLight() = 0;
        virtual bool isExtense() = 0;
        virtual Object* getLightModel() = 0;
        virtual std::vector<Point3D> getLightSamples() = 0;
        virtual Vec3D getDirection(HitInfo &info) = 0;
        virtual Point3D getPos() = 0;
        virtual std::vector<Point3D> getMeshControlPoints() = 0;
        virtual void setColor(RGBColor &color) = 0;
    protected:
        bool shadows;
        std::vector<Point3D> light_samples;
        int n_samples;
};

#endif