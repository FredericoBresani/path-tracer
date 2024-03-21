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
#include "PointLight.h"
#include "TriangleMesh.h"

bool inShadow(Ray ray, std::vector<Object*> &objects, float lightDistance, HitInfo &info)
{
    double t = infinity, tmin = infinity;
    HitInfo *shadowHit = new HitInfo();
    for (int i = 0; i < objects.size(); i++)
    {
    
            if (objects[i]->getCastShadows() && objects[i]->rayObjectIntersect(ray, &t, *shadowHit)) {
                if (t < tmin) {
                    tmin = t;
                    if (objects[i]->getKr() > 0) {
                        info.reflexive = true;
                    } else {
                        info.reflexive = false;
                    }
                    if (objects[i]->getKt() > 0) {
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

RGBColor traceLight(const Ray &ray, std::vector<Object*> &objects, Light &light, Ambient *ambient, int depth, std::vector<PointLight*> *path)
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

    if (hInfo->hit_object) {
        double reflectiveness = 1.0 + ambient->ir;
        double distanceToIntersection = Vec3D::norma(ray.origin - hInfo->hit_location);
        bool safeLight = false;
        RGBColor resultingColor, mixedColor, specularColor;
        Point3D hitPoint = hInfo->hit_location + hInfo->normal*0.001;
        
        hInfo->toCamera = Vec3D::normalize(ray.origin - hInfo->hit_location);
        hInfo->viewerReflex = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toCamera)) - hInfo->toCamera);

        Vec3D auxVec = Vec3D::normalize(hInfo->viewerReflex^hInfo->normal);        
        Vec3D auxNormal = Vec3D::normalize(auxVec^hInfo->normal);
        Vec3D auxReflex = Vec3D::normalize(auxVec^hInfo->viewerReflex);
        Vec3D difuseDirection = (hInfo->normal*((double)std::rand()/(double)RAND_MAX)) + (auxVec*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd) + (auxNormal*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxNormal*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd);
        Vec3D reflexDirection = (hInfo->viewerReflex*ks) + (auxVec*((double)std::rand()/(double)RAND_MAX)) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)) + (auxReflex*((double)std::rand()/(double)RAND_MAX)) + ((auxReflex*(-1.0))*((double)std::rand()/(double)RAND_MAX));
        
        mixedColor = ((light.getColor()^objectColor)*reflectiveness)/255.0;
        resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toLight, 0.0);
        flatColor = (ambient->color*ka + resultingColor*kd)/2.0;
        color = flatColor;
        
        if (kd > 0) {
            color = (color + traceLight(Ray(hInfo->hit_location, difuseDirection), objects, light, ambient, depth - 1, path))/2.0;
        }
        if (ks > 0) {
            color = (color + traceLight(Ray(hitPoint, reflexDirection), objects, light, ambient, depth - 1, path))*(ks/2.0);
        }
        if (kr > 0) {
            color = (color + traceLight(Ray(hitPoint, hInfo->viewerReflex), objects, light, ambient, depth - 1, path))*(kr);
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
            color = (color + traceLight(Ray(hitPoint, hInfo->refraction), objects, light, ambient, depth - 1, path))*(kt);
            color = RGBColor(std::min(color.r, 255.0), std::min(color.g, 255.0), std::min(color.b, 255.0));
        }
        path->push_back(new PointLight(hInfo->hit_location, color, 1, 0));
        return color;
    }
    return RGBColor(0.0, 0.0, 0.0);
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
        Vec3D auxReflex = Vec3D::normalize(auxVec^hInfo->viewerReflex);
        Vec3D difuseDirection = (hInfo->normal*((double)std::rand()/(double)RAND_MAX)) + (auxVec*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd) + (auxNormal*((double)std::rand()/(double)RAND_MAX)*kd) + ((auxNormal*(-1.0))*((double)std::rand()/(double)RAND_MAX)*kd);
        Vec3D reflexDirection = (hInfo->viewerReflex*ks) + (auxVec*((double)std::rand()/(double)RAND_MAX)) + ((auxVec*(-1.0))*((double)std::rand()/(double)RAND_MAX)) + (auxReflex*((double)std::rand()/(double)RAND_MAX)) + ((auxReflex*(-1.0))*((double)std::rand()/(double)RAND_MAX));

        while (!safeLight) {
            int index = (int)rand() % lights.size();
            if (!lights[index]->isExtense()) {
                l = lights[index];
                safeLight = true;
            }
        }

        std::vector<std::vector<PointLight*>> lightPaths;


        for (int i = 0; i < camera.getNPaths(); i++) {
            Vec3D lightNormal, lightX, lightZ;
            std::vector<PointLight*> path;
            for (int j = 0; j < lights.size(); j++) {  
                if (lights[j]->isExtense() && lights[j]->getLightModel()->getObjectType() == 't') {
                    Point3D A, B, C;
                    std::vector<Point3D> vertices = lights[j]->getMeshControlPoints();
                    A = vertices[0];
                    B = vertices[1];
                    C = vertices[2];
                    lightX = Vec3D::normalize(B - A);
                    lightNormal = Vec3D::normalize((B - A) ^ (C - A));
                    lightZ = Vec3D::normalize(lightX ^ lightNormal);
                    j = lights.size();
                }
            }
            Vec3D randomLightDirection = (lightNormal*((double)std::rand()/(double)RAND_MAX)) + (lightNormal*(-1.0)*((double)std::rand()/(double)RAND_MAX)) + (lightX*((double)std::rand()/(double)RAND_MAX)) + (lightX*(-1.0)*((double)std::rand()/(double)RAND_MAX)) + (lightZ*((double)std::rand()/(double)RAND_MAX)) + (lightZ*(-1.0)*((double)std::rand()/(double)RAND_MAX)); 
            path.push_back(new PointLight(l->getPos(), l->getColor(), 1, 0));
            traceLight(Ray(l->getPos(), randomLightDirection), objects, *l, ambient, ambient->depth - 1, &path);
            lightPaths.push_back(path);
        }

        double lightDistance = Vec3D::norma(l->getPos() - hInfo->hit_location);

        hInfo->toLight = l->getDirection((*hInfo));
        hInfo->reflection = Vec3D::normalize(((hInfo->normal*2)*(hInfo->normal*hInfo->toLight)) - hInfo->toLight);

        if(l->castShadows() && getShadows) {
            if (!inShadow(Ray(hInfo->hit_location, hInfo->toLight), objects, lightDistance, *hInfo)) // tem um caminho para a luz
            {   // Se o hit_location consegue encontrar um caminho sample vindo da luz sem objetos opacos obstruindo
                // Retorne a cor do caminho sample
                mixedColor = ((l->getColor()^objectColor)*reflectiveness)/255.0;
                resultingColor = resultingColor + mixedColor*std::max(hInfo->normal*hInfo->toLight, 0.0);
                flatColor = (ambient->color*ka + resultingColor*kd)/2.0;
                color = flatColor;
            }
        }
        if (kd > 0) {
            color = (color + trace(Ray(hInfo->hit_location, difuseDirection), objects, camera, lights, ambient, depth - 1))/2.0;
        }
        if (ks > 0) {
            color = (color + trace(Ray(hitPoint, reflexDirection), objects, camera, lights, ambient, depth - 1))*(ks/2.0);
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
    }
    return RGBColor(0.0, 0.0, 0.0);
}

#endif