#ifndef __CAMERA__
#define __CAMERA__

#include "Points.h"
#include <iostream>
#include "Object.h"
#include "Light.h"
#include "Sampler.h"
#include "JitteredSampler.h"
#include "Screen.h"

class Camera {
    public:
        void makeCamera();
        Point3D getPos();
        int getHr();
        int getVr();
        int getNPaths();
        double getFishEyeAngle();
        Vec3D getIUP();
        Vec3D getRight();
        Vec3D getU();
        Vec3D getV();
        Vec3D getW();
        Point2D worldToScreenCoordinates(const Point3D& point);
        Point3D worldToCameraCoordinates(const Point3D& point);
        int getPixelsH();
        virtual void render(std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient, std::string version, std::string light_option);
        virtual void setSampler() = 0;
        void setScreen();
        Sampler* getSampler();
        std::shared_ptr<Screen> screen;
        
        
    protected:
        Sampler *sampler_ptr;
        int h_res, v_res, n_samples_aliasing, pixel_qtn_h, pixel_qtn_v, paths;
        double distance, pixel_size, focal_distance, fish_eye_angle;
        Vec3D up, u, v, w, right, iup;
        Point3D camera_pos, look_at;
        

};

void Camera::render(std::vector<Object*> &objects, std::vector<Light*> &lights, Ambient &ambient, std::string version, std::string light_option) {}

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
    this->setScreen();
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

Vec3D Camera::getU() {
    return u;
}

Vec3D Camera::getV() {
    return v;
}

Vec3D Camera::getW() {
    return w;
}

Point3D Camera::worldToCameraCoordinates(const Point3D& point) {
    Vec3D worldPoint = point - camera_pos; // litle cheating here, using a vector as a point
    double x = (w.x*(worldPoint.x)) + (w.y*(worldPoint.y)) + (w.z*(worldPoint.z));
    double y = (v.x*(worldPoint.x)) + (v.y*(worldPoint.y)) + (v.z*(worldPoint.z));
    double z = (u.x*(worldPoint.x)) + (u.y*(worldPoint.y)) + (u.z*(worldPoint.z));
    return Point3D(z, y, x); // Remenber that the base order is like that
}

Point2D Camera::worldToScreenCoordinates(const Point3D& point) {           //this is a point on the screen already, althoug in world coordinates                                       
    Point3D cameraCoordinates = Camera::worldToCameraCoordinates(point); //that is why no projection is needed
    return Point2D(cameraCoordinates.x, cameraCoordinates.y);
}


Vec3D Camera::getRight()
{
    return this->right;
}

int Camera::getPixelsH()
{
    return this->pixel_qtn_h;
}

double Camera::getFishEyeAngle()
{
    return this->fish_eye_angle;
}

void Camera::setSampler() {
    sampler_ptr = new JitteredSampler(n_samples_aliasing);
}

void Camera::setScreen() {
    screen = std::make_shared<Screen>();
}

#endif