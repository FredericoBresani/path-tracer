#ifndef __TRIANGLEMESHLIGHT__
#define __TRIANGLEMESHLIGHT__


#include "Definitions.h"
#include "Object.h"
#include "TriangleMesh.h"
#include "Light.h"



class TriangleMeshLight: public Light {
    public:
        TriangleMeshLight(RGBColor color, TriangleMesh *mesh, int n): light_color(color) 
        {
            light_model = std::make_shared<TriangleMesh>((*mesh));
            this->n_samples = n;
            this->sampleLight();
        }
        ~TriangleMeshLight() {}
        std::vector<Point3D> getLightSamples();
    private:
        RGBColor light_color;
        std::shared_ptr<TriangleMesh> light_model;
        void sampleLight();
        Vec3D getDirection(HitInfo &info);
        RGBColor incidentRadiance(HitInfo &info);
        Point3D getPos();
        RGBColor getColor();
        bool castShadows();
        bool isExtense();
        std::shared_ptr<Object> getLightModel();
        std::vector<Point3D> getMeshControlPoints();
        void setColor(RGBColor &c);
        bool isDirectional();
        Vec3D getDirectionalVec();
};

void TriangleMeshLight::sampleLight(void) 
{
    std::unique_lock<std::mutex> lock(lightLock);
    auto vertices = this->light_model->vertices;
    auto triangles = this->light_model->triangles;
    for (uint32_t i = 0; i < this->n_samples; i++) {
        for(auto triangle : triangles) {
            Point3D A, B, C, P, sample;
            Vec3D auxAB, auxPC;
            A = vertices[triangle.x];
            B = vertices[triangle.y];
            C = vertices[triangle.z];
            auxAB = B - A;
            P = A + (auxAB*((double)rand()/(double)RAND_MAX));
            auxPC = C - P;
            sample = P + (auxPC*((double)rand()/(double)RAND_MAX));
            light_samples.push_back(sample);
        }
    }
}

bool TriangleMeshLight::isExtense()
{
    return true;
}

std::shared_ptr<Object> TriangleMeshLight::getLightModel()
{
    return this->light_model;
}

RGBColor TriangleMeshLight::getColor()
{
    return this->light_color;
}

Vec3D TriangleMeshLight::getDirection(HitInfo &info)
{
    return Vec3D();
}

RGBColor TriangleMeshLight::incidentRadiance(HitInfo &info)
{
    return RGBColor();
}

bool TriangleMeshLight::castShadows()
{
    return this->light_model->castShadows;
}

Point3D TriangleMeshLight::getPos()
{
    return Point3D();
}

std::vector<Point3D> TriangleMeshLight::getLightSamples()
{
    return this->light_samples;
}
std::vector<Point3D> TriangleMeshLight::getMeshControlPoints()
{
    auto vertices = this->light_model->vertices;
    auto triangles = this->light_model->triangles;
    return {vertices[triangles[0].x], vertices[triangles[0].y], vertices[triangles[0].z]};
}

void TriangleMeshLight::setColor(RGBColor &c)
{
    std::unique_lock<std::mutex> lock(lightLock);
    this->light_color = c;
}

bool TriangleMeshLight::isDirectional()
{
    return false;
}

Vec3D TriangleMeshLight::getDirectionalVec()
{
    return Vec3D();
}

#endif