#ifndef __WORLD__
#define __WORLD__

#include "Ray.h"
#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Ambient.h"
#include <fstream>

bool inShadow(Ray ray, std::vector<Object*> &objects, float lightDistance);

RGBColor trace(const Ray &ray, std::vector<Object*> &objects, Camera &camera, std::vector<Light*> lights, Ambient *ambient, int depth);

#endif