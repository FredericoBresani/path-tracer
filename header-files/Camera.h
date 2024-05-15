#ifndef __CAMERA__
#define __CAMERA__

#include "Points.h"
#include "Object.h"
#include "Light.h"
#include "Sampler.h"
#include "JitteredSampler.h"

class Camera {
    public:
        void makeCamera();
        Point3D getPos();
        int getHr();
        int getVr();
        int getNPaths();
        Vec3D getIUP();
        Vec3D getRight();
        int getPixelsH();
        virtual void render(std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient);
        virtual void setSampler() = 0;
        Sampler* getSampler();
        
        
    protected:
        Sampler *sampler_ptr;
        int h_res, v_res, n_samples_aliasing, pixel_qtn_h, pixel_qtn_v, paths;
        double distance, pixel_size, focal_distance, fish_eye_angle;
        Vec3D up, u, v, w, right, iup;
        Point3D camera_pos, look_at;

};

void Camera::render(std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient) {}

Sampler* Camera::getSampler() {
    return sampler_ptr;
}

void Camera::makeCamera()
{
    // Tha camera base should follow this order {w = z, v = y, u = x}
    pixel_qtn_h = (double)h_res/pixel_size;
    pixel_qtn_v = (double)v_res/pixel_size;
    auto toScreen = Vec3D::normalize(look_at - camera_pos);
    w = toScreen;
    auto WUP = w ^ up;
    if (WUP.x == 0.0 && WUP.y == 0.0 && WUP.x == 0.0)
    {
        up = Vec3D(1.0, 0.0, 0.0);
    }
    v = Vec3D::normalize(up - (w*((up*w)/(w*w))));
    u = v ^ w;
    if (pixel_qtn_h <= pixel_qtn_v) {
        right =  u*(2.0/pixel_qtn_h);
        iup = v*(2.0/pixel_qtn_h); 
    } else {
        right =  u*(2.0/pixel_qtn_v);
        iup = v*(2.0/pixel_qtn_v); 
    }
    this->setSampler();
}

int Camera::getHr()
{
    return this->h_res;
}

int Camera::getVr()
{
    return this->v_res;
}

Point3D Camera::getPos()
{
    return camera_pos;
}

int Camera::getNPaths()
{
    return this->paths;
}

Vec3D Camera::getIUP() 
{
    return this->iup;
}

Vec3D Camera::getRight()
{
    return this->right;
}

int Camera::getPixelsH()
{
    return this->pixel_qtn_h;
}

void Camera::setSampler() {
    sampler_ptr = new JitteredSampler(n_samples_aliasing);
}

#endif