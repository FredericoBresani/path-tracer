#ifndef __WORLD__
#define __WORLD__

#include "Ray.h"
#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Ambient.h"
#include <fstream>
#include <stdlib.h>
#include <math.h>

bool inShadow(Ray ray, std::vector<Object*> &objects, float lightDistance)
{
    double t = infinity;
    HitInfo *shadowHit = new HitInfo();
    for (int i = 0; i < objects.size(); i++)
    {
    
            if (objects[i]->getCastShadows() && objects[i]->rayObjectIntersect(ray, &t, *shadowHit)) {
                if (t < lightDistance) {
                    return true;
                }
            }
        
    }
    return false;
}

RGBColor trace(const Ray &ray, std::vector<Object*> &objects, Camera &camera, std::vector<Light*> lights, Ambient *ambient, int depth)
{
    double t = infinity;
    double tmin = infinity;
    double kd, ks, ka, kr, kt, phongExp, ior;
    bool getShadows; 
    HitInfo *hInfo = new HitInfo();
    RGBColor color, objectColor, flatColor, difuseColor, specularColor, reflectiveColor, transparentColor;
    if (depth == 0) {
        return RGBColor(0, 0, 0);
    }
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->rayObjectIntersect(ray, &t, *hInfo))
        {
            if (t < tmin)
            {
                tmin = t;
                hInfo->hit_location = ray.origin + ray.direction*tmin;
                hInfo->normal = objects[i]->getNormal(hInfo->hit_location, ray);
                objectColor = objects[i]->getColor();
                kd = objects[i]->getKd();
                ks = objects[i]->getKs();
                ka = objects[i]->getKa();
                kr = objects[i]->getKr();
                kt = objects[i]->getKt();
                ior = objects[i]->getIor();
                phongExp = objects[i]->getPhongExp();
                hInfo->material_pointer = objects[i]->material;
                getShadows = objects[i]->getShadows();
                hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
                // color = setPixelColorNormal(hit.normal);
                // color = setPixelColorCoordinates(hit.hit_location);
            }
        }
    }

    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]->isExtense())
        {
            if(lights[i]->getLightModel()->rayObjectIntersect(ray, &t, *hInfo))
            {
                if (t < tmin) 
                {
                    tmin = t;
                    objectColor = lights[i]->getColor();
                    hInfo->hit_light = true;
                }
            }
        }
    }
    if (hInfo->hit_light) {
        return objectColor;
    }
    if (hInfo->hit_object) {
        double reflectiveness = 1.0 + ambient->ir;
        double distanceToIntersection = Vec3D::norma(ray.origin - hInfo->hit_location);
        bool safeLight = false;
        RGBColor resultingColor, mixedColor, specularColor;
        Point3D hitPoint = hInfo->hit_location + hInfo->normal*0.001;
        Light *l;
        
        hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
        hInfo->viewerReflex = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toCamera)) - hInfo->toCamera);

        Vec3D auxVec = Vec3D::normalize(hInfo->viewerReflex^hInfo->normal);        
        Vec3D auxNormal = Vec3D::normalize(auxVec^hInfo->normal);
        Vec3D difuseDirection = (hInfo->normal*((double)std::rand()/(double)RAND_MAX)) + (auxVec*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd) + (auxNormal*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxNormal*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd);
        
        while (!safeLight) {
            int index = (int)rand() % lights.size();
            if (!lights[index]->isExtense()) {
                l = lights[index];
                safeLight = true;
            }
        }

        double lightDistance = Vec3D::norma(l->getPos() - hInfo->hit_location);

        hInfo->toLight = l->getDirection((*hInfo));
        hInfo->reflection = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toLight)) - hInfo->toLight);

        if(l->castShadows() && getShadows) {
            if (!inShadow(Ray(hInfo->hit_location, hInfo->toLight), objects, lightDistance)) 
            {
                mixedColor = ((l->getColor()^objectColor)*reflectiveness)/255.0;
                specularColor = l->getColor()*(ks*pow(std::max(hInfo->reflection*hInfo->toCamera, 0.0), phongExp));
                resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toLight, 0.0) + specularColor;
            }
        } else {
            mixedColor = ((l->getColor()^objectColor)*reflectiveness)/255.0;
            specularColor = l->getColor()*(ks*pow(std::max(hInfo->reflection*hInfo->toCamera, 0.0), phongExp));
            resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toLight, 0.0) + specularColor;
        }

        flatColor = (ambient->color*ka + resultingColor*kd)/2.0;
        color = flatColor;
        
        if (kd > 0) {
            color = (color + trace(Ray(hitPoint, difuseDirection), objects, camera, lights, ambient, depth - 1))/2.0;
        }
        if (kr > 0) {
            color = (color + trace(Ray(hitPoint, hInfo->viewerReflex), objects, camera, lights, ambient, depth - 1))*(kr);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        if (kt > 0) { 
            double eta = ior;
            double cos = hInfo->normal*hInfo->toCamera;

            if (cos < 0.0)
            {
                cos = -cos;
                eta = 1/eta;
                hInfo->normal = hInfo->normal*(-1);
            }
            double temp = 1.0 - ((1.0 - cos*cos) / (eta*eta));
            double cos2 = sqrt(temp);
            hInfo->refraction = hInfo->toCamera*(-1)/eta - (hInfo->normal*(cos2 - cos/eta));
            hitPoint = hitPoint + hInfo->refraction*0.015;
            color = (color + trace(Ray(hitPoint, hInfo->refraction), objects, camera, lights, ambient, depth - 1))*(kt);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        return color;
    } else {
        return RGBColor(0.0, 0.0, 0.0);
    }
    
}

#endif