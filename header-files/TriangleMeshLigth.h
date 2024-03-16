#ifndef __TRIANGLEMESHLIGHT__
#define __TRIANGLEMESHLIGHT__

#include "RGBColor.h"
#include "GeometricObject.h"
#include "TriangleMesh.h"
#include "Light.h"
#include "HitInfo.h"
#include "Vectors.h"
#include <stdlib.h>


class TriangleMeshLight: public Light {
    public:
        TriangleMeshLight(RGBColor color, TriangleMesh *mesh, int n): light_color(color), light_model(mesh) 
        {
            this->n_samples = n;
            this->sampleLight();
        }
        ~TriangleMeshLight() {}
        std::vector<Point3D> getLightSamples();
    private:
        RGBColor light_color;
        TriangleMesh *light_model;
        void sampleLight();
        Vec3D getDirection(HitInfo &info);
        RGBColor incidentRadiance(HitInfo &info);
        Point3D getPos();
        RGBColor getColor();
        bool castShadows();
        bool isExtense();
        Object* getLightModel();
        
        
};

void TriangleMeshLight::sampleLight(void) 
{
    std::vector<Point3D> vertices = this->light_model->vertices;
    std::vector<Point3I> triangles = this->light_model->triangles;
    for (int i = 0; i < this->n_samples; i++) {
        for(int i = 0; i < triangles.size(); i++) {
            Point3D A, B, C, P, sample;
            Vec3D auxAB, auxPC;
            A = vertices[triangles[i].x];
            B = vertices[triangles[i].y];
            C = vertices[triangles[i].z];
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

Object* TriangleMeshLight::getLightModel()
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




#endif