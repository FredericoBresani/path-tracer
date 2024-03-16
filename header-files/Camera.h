#ifndef __CAMERA__
#define __CAMERA__

#include "Vectors.h"
#include "Points.h"
#include <vector>
#include "Object.h"
#include "Ambient.h"
#include "Light.h"

class Camera {
    public:
        void makeCamera();
        Point3D getPos();
        int getHr();
        int getVr();
        virtual void render(std::vector<Object*> objects, std::vector<Light*> &lights, Ambient &ambient);
        
        
    protected:
        int h_res, v_res, n_samples, pixel_qtn_h, pixel_qtn_v;
        double distance, pixel_size, focal_distance, fish_eye_angle;
        Vec3D up, u, v, w, right, iup;
        Point3D camera_pos, look_at;

};

#endif