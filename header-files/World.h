#ifndef __WORLD__
#define __WORLD__

#include <fstream>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "Ray.h"
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Ambient.h"
#include "PointLight.h"
#include "TriangleMesh.h"
#include "DirectionalPointLight.h"
#include "MetropolisManager.h"


bool inShadow(Ray ray, std::vector<Object*> objects, float lightDistance, HitInfo &info)
{
    double t = infinity;
    double tmin = infinity;
    std::shared_ptr<HitInfo> shadowHit(new HitInfo());
    for (auto object : objects)
    {
        if (object->getCastShadows() && object->rayObjectIntersect(ray, &t, shadowHit)) {
            if (t < tmin) {
                tmin = t;
                if (object->getKt() > 0) {
                    info.transparent = true;
                } else {
                    info.transparent = false;
                }
            }
        }
    }
    if (tmin < lightDistance) {
        return true;
    }
    return false;
}

void traceLight(const Ray &ray, std::vector<Object*> &objects, Light* &light, Ambient &ambient, int depth, std::vector<Light*> &path)
{
    double t = infinity;
    double tmin = infinity;
    double kd, ks, ka, kr, kt, phongExp, ior;
    bool getShadows; 
    std::shared_ptr<HitInfo> hInfo(new HitInfo());
    RGBColor color, objectColor, flatColor, difuseColor, specularColor, reflectiveColor, transparentColor;
    if (depth == 0) {
        return;
    }
    for (auto object : objects)
    {
        if (object->rayObjectIntersect(ray, &t, hInfo))
        {
            if (t < tmin)
            {
                tmin = t;
                hInfo->hit_location = ray.origin + ray.direction*tmin;
                hInfo->normal = object->getNormal(hInfo->hit_location, ray);
                objectColor = object->getColor();
                kd = object->getKd();
                ks = object->getKs();
                ka = object->getKa();
                kr = object->getKr();
                kt = object->getKt();
                ior = object->getIor();
                phongExp = object->getPhongExp();
                hInfo->material_pointer = object->material;
                getShadows = object->getShadows();
                hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
                // color = setPixelColorNormal(hit.normal);
                // color = setPixelColorCoordinates(hit.hit_location);
            }
        }
    }

    if (hInfo->hit_object) {
        auto reflectiveness = 1.0 + ambient.ir;
        auto distanceToIntersection = Vec3D::norma(ray.origin - hInfo->hit_location);
        auto safeLight = false;
        RGBColor resultingColor, mixedColor, specularColor;
        auto hitPoint = hInfo->hit_location + hInfo->normal*0.001;
        
        hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
        hInfo->viewerReflex = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toCamera)) - hInfo->toCamera);

        auto auxVec = Vec3D::normalize(hInfo->viewerReflex^hInfo->normal);        
        auto auxNormal = Vec3D::normalize(auxVec^hInfo->normal);
        auto auxReflex = Vec3D::normalize(auxVec^hInfo->viewerReflex);
        auto difuseDirection = (hInfo->normal*((double)std::rand()/(double)RAND_MAX)) + (auxVec*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd) + (auxNormal*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxNormal*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd);
        auto newLightColor = light->getColor();
        
        mixedColor = ((light->getColor()^objectColor)*reflectiveness)/255.0;
        resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toCamera, 0.0);
        flatColor = (ambient.color*ka + resultingColor)/2.0;
        color = flatColor;
        light->setColor(color);
        
        if (kd > 0) {
            Light *pathLight = new PointLight(hInfo->hit_location, color*kd, 1, 0);
            path.push_back(pathLight);
            pathLight = nullptr;
            traceLight(Ray(hInfo->hit_location, difuseDirection), objects, light, ambient, depth - 1, path);
        }
        if (kr > 0) {
            auto r = newLightColor*kr;
            light->setColor(r);
            Light* pathLight = new DirectionalPointLight(hInfo->hit_location, color*kr, 1, 0, hInfo->viewerReflex);
            path.push_back(pathLight);
            pathLight = nullptr;
            traceLight(Ray(hitPoint, hInfo->viewerReflex), objects, light, ambient, depth - 1, path);
        }
        if (kt > 0) { 
            auto eta = ior;
            auto cos = hInfo->normal*hInfo->toCamera;

            if (cos < 0.0)
            {
                cos = -cos;
                eta = 1/eta;
                hInfo->normal = hInfo->normal*(-1);
            }
            auto temp = 1.0 - ((1.0 - cos*cos) / (eta*eta));
            auto cos2 = sqrt(temp);
            hInfo->refraction = Vec3D::normalize(hInfo->toCamera*(-1)/eta - (hInfo->normal*(cos2 - cos/eta)));
            hitPoint = hitPoint + hInfo->refraction*0.015;

            auto r = newLightColor*kt;
            light->setColor(r);
            Light* pathLight = new DirectionalPointLight(hitPoint, newLightColor*kt, 1, 0, hInfo->refraction);
            path.push_back(pathLight);
            pathLight = nullptr;
            traceLight(Ray(hitPoint, hInfo->refraction), objects, light, ambient, depth - 1, path);
        }
    }
}

RGBColor trace(const Ray &ray, std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient, int depth)
{
    double t = infinity;
    double tmin = infinity;
    double kd, ks, ka, kr, kt, phongExp, ior;
    bool getShadows; 
    std::shared_ptr<HitInfo> hInfo(new HitInfo());
    RGBColor color, objectColor, flatColor, difuseColor, specularColor, reflectiveColor, transparentColor;

    if (depth == 0) {
        return RGBColor(0, 0, 0);
    }
    for (auto object : objects)
    {
        if (object->rayObjectIntersect(ray, &t, hInfo))
        {
            if (t < tmin)
            {
                tmin = t;
                hInfo->hit_location = ray.origin + ray.direction*tmin;
                hInfo->normal = object->getNormal(hInfo->hit_location, ray);
                objectColor = object->getColor();
                kd = object->getKd();
                ks = object->getKs();
                ka = object->getKa();
                kr = object->getKr();
                kt = object->getKt();
                ior = object->getIor();
                phongExp = object->getPhongExp();
                hInfo->material_pointer = object->material;
                getShadows = object->getShadows();
                hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
                // color = setPixelColorNormal(hit.normal);
                // color = setPixelColorCoordinates(hit.hit_location);
            }
        }
    }

    for (auto light : lights) {
        if (light->isExtense())
        {
            if(light->getLightModel()->rayObjectIntersect(ray, &t, hInfo))
            {
                if (t < tmin) 
                {
                    tmin = t;
                    objectColor = light->getColor();
                    hInfo->hit_light = true;
                }
            }
        }
    }
    if (hInfo->hit_light) {
        return objectColor;
    }
    if (hInfo->hit_object) {
        double reflectiveness = 1.0 + ambient.ir;
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
            if (!inShadow(Ray(hInfo->hit_location, hInfo->toLight), objects, lightDistance, *hInfo)) 
            {
                mixedColor = ((l->getColor()^objectColor)*reflectiveness)/255.0;
                specularColor = l->getColor()*(ks*pow(std::max(hInfo->reflection*hInfo->toCamera, 0.0), phongExp));
                resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toLight, 0.0) + specularColor;
            } else {
                if (hInfo->transparent) {
                    resultingColor = resultingColor + ((objectColor ^ trace(Ray(hitPoint, hInfo->toLight), objects, lights, ambient, depth - 1))/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                }
            }
        } else {
            mixedColor = ((l->getColor()^objectColor)*reflectiveness)/255.0;
            specularColor = l->getColor()*(ks*pow(std::max(hInfo->reflection*hInfo->toCamera, 0.0), phongExp));
            resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toLight, 0.0) + specularColor;
        }

        flatColor = (ambient.color*ka + resultingColor*kd)/2.0;
        color = flatColor;
        
        if (kd > 0) {
            color = (color + trace(Ray(hitPoint, difuseDirection), objects, lights, ambient, depth - 1))/2.0;
        }
        if (kr > 0) {
            color = (color + trace(Ray(hitPoint, hInfo->viewerReflex), objects, lights, ambient, depth - 1))*(kr);
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
            color = (color + trace(Ray(hitPoint, hInfo->refraction), objects, lights, ambient, depth - 1))*(kt);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        return color;
    }
    return RGBColor(0.0, 0.0, 0.0);
}

RGBColor bidirectionalTrace(const Ray &ray, std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient, int depth, Vec3D lightX, Vec3D lightNormal, Vec3D lightZ)
{
    double t = infinity;
    double tmin = infinity;
    double kd, ks, ka, kr, kt, phongExp, ior;
    bool getShadows; 
    std::shared_ptr<HitInfo> hInfo(new HitInfo());
    RGBColor color, objectColor, flatColor, difuseColor, specularColor, reflectiveColor, transparentColor;

    if (depth == 0) {
        return RGBColor(0, 0, 0);
    }
    for (auto object : objects)
    {
        if (object->rayObjectIntersect(ray, &t, hInfo))
        {
            if (t < tmin)
            {
                tmin = t;
                hInfo->hit_location = ray.origin + ray.direction*tmin;
                hInfo->normal = object->getNormal(hInfo->hit_location, ray);
                objectColor = object->getColor();
                kd = object->getKd();
                ks = object->getKs();
                ka = object->getKa();
                kr = object->getKr();
                kt = object->getKt();
                ior = object->getIor();
                phongExp = object->getPhongExp();
                hInfo->material_pointer = object->material;
                getShadows = object->getShadows();
                hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
                // color = setPixelColorNormal(hit.normal);
                // color = setPixelColorCoordinates(hit.hit_location);
            }
        }
    }

    for (auto light : lights) {
        if (light->isExtense())
        {
            if(light->getLightModel()->rayObjectIntersect(ray, &t, hInfo))
            {
                if (t < tmin) 
                {
                    tmin = t;
                    objectColor = light->getColor();
                    hInfo->hit_light = true;
                }
            }
        }
    }
    if (hInfo->hit_light) {
        return objectColor;
    }
    if (hInfo->hit_object) {
        auto reflectiveness = 1.0 + ambient.ir;
        auto distanceToIntersection = Vec3D::norma(ray.origin - hInfo->hit_location);
        auto safeLight = false;
        RGBColor resultingColor, mixedColor, specularColor, resultingGoodPathColor;
        auto hitPoint = hInfo->hit_location + hInfo->normal*0.001;
        Light *l;

        hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
        hInfo->viewerReflex = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toCamera)) - hInfo->toCamera);

        auto auxVec = Vec3D::normalize(hInfo->viewerReflex^hInfo->normal);        
        auto auxNormal = Vec3D::normalize(auxVec^hInfo->normal);
        auto auxReflex = Vec3D::normalize(auxVec^hInfo->viewerReflex);
        auto difuseDirection = (hInfo->normal*((double)std::rand()/(double)RAND_MAX)) + (auxVec*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd) + (auxNormal*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxNormal*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd);
        auto specularDirection = (hInfo->viewerReflex*ks) + (auxVec*((double)std::rand()/(double)RAND_MAX)) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)) + (auxReflex*((double)std::rand()/(double)RAND_MAX)) + ((auxReflex*(-1.0))*((double)std::rand()/(double)RAND_MAX));

        while (!safeLight) {
            int index = (int)rand() % lights.size();
            if (!lights[index]->isExtense()) {
                l = lights[index];
                safeLight = true;
            }
        }

        std::vector<Light*> lightPath = {};
        
        Light *copyL = new PointLight((*l));
        auto randomLightDirection = (lightNormal*((double)std::rand()/(double)RAND_MAX)) + (lightX*((double)std::rand()/(double)RAND_MAX)) + (lightX*(-1.0)*((double)std::rand()/(double)RAND_MAX)) + (lightZ*((double)std::rand()/(double)RAND_MAX)) + (lightZ*(-1.0)*((double)std::rand()/(double)RAND_MAX)); 
        Light *first = new PointLight((*l));
        lightPath.push_back(first);
        traceLight(Ray((*l).getPos(), randomLightDirection), objects, copyL, ambient, ambient.depth, lightPath);

        l = nullptr;
        delete copyL;
        first = nullptr;

        int successfulPaths = 0;
        bool pathFound = false;
        for (auto pathLight : lightPath) {
            auto whereIsTheLight = pathLight->getPos();
            auto lightDistance = Vec3D::norma(pathLight->getPos() - hInfo->hit_location);
            
            hInfo->toLight = Vec3D::normalize(pathLight->getDirection((*hInfo)));

            auto lightReflection = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toLight)) - hInfo->toLight);
            if(pathLight->castShadows() && getShadows) {
                if (!inShadow(Ray(hInfo->hit_location, hInfo->toLight), objects, lightDistance, *hInfo))
                { 
                    mixedColor = (((pathLight->getColor()^objectColor)*reflectiveness)/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                    specularColor = pathLight->getColor()*(ks*pow(std::max(lightReflection*hInfo->toCamera, 0.0), phongExp));
                    if (mixedColor.r > 0 || mixedColor.g > 0 || mixedColor.b > 0) {
                        pathFound = true;
                        resultingColor = resultingColor + mixedColor + specularColor; 
                        successfulPaths++;
                    }      
                } else {
                    if (hInfo->transparent) {
                        resultingColor = resultingColor + ((objectColor ^ bidirectionalTrace(Ray(hInfo->hit_location, hInfo->toLight), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ))/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                    }
                }
            }
        }

        if (pathFound) {
            resultingColor = resultingColor/(double)successfulPaths;
        }

        for (auto point : lightPath) {
            delete point;
        }
        lightPath = {};        

        flatColor = (ambient.color*ka + resultingColor*kd)/2.0;
        color = flatColor;

        if (kd > 0) {
            color = (color + bidirectionalTrace(Ray(hInfo->hit_location, difuseDirection), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ))/2.0;
        }
        if (ks > 0) {
            color = (color + bidirectionalTrace(Ray(hitPoint, specularDirection), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ))*(ks/2.0);
        }
        if (kr > 0) {
            color = (color + bidirectionalTrace(Ray(hitPoint, hInfo->viewerReflex), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ))*(kr);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        if (kt > 0) { 
            auto eta = ior;
            auto cos = hInfo->normal*hInfo->toCamera;

            if (cos < 0.0)
            {
                cos = -cos;
                eta = 1/eta;
                hInfo->normal = hInfo->normal*(-1);
            }
            auto temp = 1.0 - ((1.0 - cos*cos) / (eta*eta));
            auto cos2 = sqrt(temp);
            hInfo->refraction = hInfo->toCamera*(-1)/eta - (hInfo->normal*(cos2 - cos/eta));
            hitPoint = hitPoint + hInfo->refraction*0.015;
            color = (color + bidirectionalTrace(Ray(hitPoint, hInfo->refraction), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ))*(kt);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        return color;
    }
    return RGBColor(0.0, 0.0, 0.0);
}

RGBColor metropolisTrace(const Ray &ray, std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient, int depth, Vec3D lightX, Vec3D lightNormal, Vec3D lightZ, std::shared_ptr<MetropolisManager> &metroManager, int i)
{
    double t = infinity;
    double tmin = infinity;
    double kd, ks, ka, kr, kt, phongExp, ior;
    bool getShadows; 
    std::shared_ptr<HitInfo> hInfo(new HitInfo());
    RGBColor color, objectColor, flatColor, difuseColor, specularColor, reflectiveColor, transparentColor;

    if (depth == 0) {
        return RGBColor(0, 0, 0);
    }
    for (auto object : objects)
    {
        if (object->rayObjectIntersect(ray, &t, hInfo))
        {
            if (t < tmin)
            {
                tmin = t;
                hInfo->hit_location = ray.origin + ray.direction*tmin;
                hInfo->normal = object->getNormal(hInfo->hit_location, ray);
                objectColor = object->getColor();
                kd = object->getKd();
                ks = object->getKs();
                ka = object->getKa();
                kr = object->getKr();
                kt = object->getKt();
                ior = object->getIor();
                phongExp = object->getPhongExp();
                hInfo->material_pointer = object->material;
                getShadows = object->getShadows();
                hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
                // color = setPixelColorNormal(hit.normal);
                // color = setPixelColorCoordinates(hit.hit_location);
            }
        }
    }

    for (auto light : lights) {
        if (light->isExtense())
        {
            if(light->getLightModel()->rayObjectIntersect(ray, &t, hInfo))
            {
                if (t < tmin) 
                {
                    tmin = t;
                    objectColor = light->getColor();
                    hInfo->hit_light = true;
                }
            }
        }
    }
    if (hInfo->hit_light) {
        return objectColor;
    }
    if (hInfo->hit_object) {
        auto reflectiveness = 1.0 + ambient.ir;
        auto distanceToIntersection = Vec3D::norma(ray.origin - hInfo->hit_location);
        auto safeLight = false;
        RGBColor resultingColor, mixedColor, specularColor, resultingGoodPathColor;
        auto hitPoint = hInfo->hit_location + hInfo->normal*0.001;
        Light *l;

        hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
        hInfo->viewerReflex = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toCamera)) - hInfo->toCamera);

        auto auxVec = Vec3D::normalize(hInfo->viewerReflex^hInfo->normal);        
        auto auxNormal = Vec3D::normalize(auxVec^hInfo->normal);
        auto auxReflex = Vec3D::normalize(auxVec^hInfo->viewerReflex);
        auto difuseDirection = (hInfo->normal*((double)std::rand()/(double)RAND_MAX)) + (auxVec*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd) + (auxNormal*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxNormal*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd);
        auto specularDirection = (hInfo->viewerReflex*ks) + (auxVec*((double)std::rand()/(double)RAND_MAX)) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)) + (auxReflex*((double)std::rand()/(double)RAND_MAX)) + ((auxReflex*(-1.0))*((double)std::rand()/(double)RAND_MAX));

        while (!safeLight) {
            int index = (int)rand() % lights.size();
            if (!lights[index]->isExtense()) {
                l = lights[index];
                safeLight = true;
            }
        }

        std::vector<Light*> lightPath = {};
        
        Light *copyL = new PointLight((*l));
        auto randomLightDirection = (lightNormal*((double)std::rand()/(double)RAND_MAX)) + (lightX*((double)std::rand()/(double)RAND_MAX)) + (lightX*(-1.0)*((double)std::rand()/(double)RAND_MAX)) + (lightZ*((double)std::rand()/(double)RAND_MAX)) + (lightZ*(-1.0)*((double)std::rand()/(double)RAND_MAX)); 
        Light *first = new PointLight((*l));
        lightPath.push_back(first);
        traceLight(Ray((*l).getPos(), randomLightDirection), objects, copyL, ambient, ambient.depth, lightPath);

        l = nullptr;
        delete copyL;
        first = nullptr;

        int successfulPaths = 0;
        bool pathFound = false;
        for (auto pathLight : lightPath) {
            auto whereIsTheLight = pathLight->getPos();
            auto lightDistance = Vec3D::norma(pathLight->getPos() - hInfo->hit_location);
            
            hInfo->toLight = Vec3D::normalize(pathLight->getDirection((*hInfo)));

            auto lightReflection = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toLight)) - hInfo->toLight);
            if(pathLight->castShadows() && getShadows) {
                if (!inShadow(Ray(hInfo->hit_location, hInfo->toLight), objects, lightDistance, *hInfo))
                { 
                    mixedColor = (((pathLight->getColor()^objectColor)*reflectiveness)/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                    specularColor = pathLight->getColor()*(ks*pow(std::max(lightReflection*hInfo->toCamera, 0.0), phongExp));
                    if (mixedColor.r > 0 || mixedColor.g > 0 || mixedColor.b > 0) {
                        pathFound = true;
                        resultingColor = resultingColor + mixedColor + specularColor; 
                        successfulPaths++;
                    }      
                } else {
                    if (hInfo->transparent) {
                        resultingColor = resultingColor + ((objectColor ^ metropolisTrace(Ray(hInfo->hit_location, hInfo->toLight), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ, metroManager, i))/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                    }
                }
            }
        }

        bool sameAsLightPath = false;
        if (pathFound) {
            resultingColor = resultingColor/(double)successfulPaths;
            if (resultingColor.r + resultingColor.g + resultingColor.b > 70) {
                sameAsLightPath = true;
                metroManager->energy = resultingColor.r + resultingColor.g + resultingColor.b;
                for (auto point : metroManager->goodPath) {
                    delete point;
                }
                metroManager->goodPath = {};
                for (auto point : lightPath) {
                    metroManager->goodPath.push_back(point);
                    point = nullptr;
                }
                lightPath = {};
            } else {
                for (auto point : lightPath) {
                    delete point;
                }
                lightPath = {};
            }
        } else {
            for (auto point : lightPath) {
                delete point;
            }
            lightPath = {};
        }

        pathFound = false;
        if (!sameAsLightPath && metroManager->goodPath.size() > 0) {
            successfulPaths = 0;
            for (auto pathLight : metroManager->goodPath) {
                auto whereIsTheLight = pathLight->getPos();
                auto lightDistance = Vec3D::norma(pathLight->getPos() - hInfo->hit_location);
                
                hInfo->toLight = Vec3D::normalize(pathLight->getDirection((*hInfo)));

                auto lightReflection = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toLight)) - hInfo->toLight);
                if(pathLight->castShadows() && getShadows) {
                    if (!inShadow(Ray(hInfo->hit_location, hInfo->toLight), objects, lightDistance, *hInfo))
                    { 
                        mixedColor = (((pathLight->getColor()^objectColor)*reflectiveness)/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                        specularColor = pathLight->getColor()*(ks*pow(std::max(lightReflection*hInfo->toCamera, 0.0), phongExp));
                        if (mixedColor.r > 0 || mixedColor.g > 0 || mixedColor.b > 0) {
                            pathFound = true;
                            resultingGoodPathColor = resultingGoodPathColor + mixedColor; 
                            successfulPaths++;
                        }      
                    } else {
                        if (hInfo->transparent) {
                            std::shared_ptr<MetropolisManager> tempManager(new MetropolisManager());
                            resultingGoodPathColor = resultingGoodPathColor + ((objectColor ^ metropolisTrace(Ray(hInfo->hit_location, hInfo->toLight), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ, tempManager, i))/255.0)*std::max(hInfo->normal*hInfo->toLight, 0.0);
                        }
                    }
                }
            }
        }

        if (pathFound) {
            resultingGoodPathColor = resultingGoodPathColor/(double)successfulPaths;
            if (resultingGoodPathColor > resultingColor) {
                resultingColor = resultingGoodPathColor;
            }
        }

        flatColor = (ambient.color*ka + resultingColor*kd)/2.0;
        color = flatColor;

        if (kd > 0) {
            color = (color + metropolisTrace(Ray(hInfo->hit_location, difuseDirection), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ, metroManager, i))/2.0;
        }
        if (ks > 0) {
            color = (color + metropolisTrace(Ray(hitPoint, specularDirection), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ, metroManager, i))*(ks/2.0);
        }
        if (kr > 0) {
            color = (color + metropolisTrace(Ray(hitPoint, hInfo->viewerReflex), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ, metroManager, i))*(kr);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        if (kt > 0) { 
            auto eta = ior;
            auto cos = hInfo->normal*hInfo->toCamera;

            if (cos < 0.0)
            {
                cos = -cos;
                eta = 1/eta;
                hInfo->normal = hInfo->normal*(-1);
            }
            auto temp = 1.0 - ((1.0 - cos*cos) / (eta*eta));
            auto cos2 = sqrt(temp);
            hInfo->refraction = hInfo->toCamera*(-1)/eta - (hInfo->normal*(cos2 - cos/eta));
            hitPoint = hitPoint + hInfo->refraction*0.015;
            color = (color + metropolisTrace(Ray(hitPoint, hInfo->refraction), objects, lights, ambient, depth - 1, lightX, lightNormal, lightZ, metroManager, i))*(kt);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        return color;
    }
    return RGBColor(0.0, 0.0, 0.0);
}

#endif