#ifndef __HITINFO__
#define __HITINFO__

#include "Points.h"
#include "Vectors.h"
#include <memory>

class Material;


class HitInfo {
    public:
        bool hit_object = false;
        bool hit_light = false;
        bool transparent, reflexive;
        Point3D hit_location;
        Vec3D normal, toLight, toCamera, reflection, refraction, viewerReflex; 
        std::shared_ptr<Material> material_pointer;
        HitInfo() {}
        ~HitInfo() {}
};

#endif