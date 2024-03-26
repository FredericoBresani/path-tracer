#ifndef __PINHOLE__
#define __PINHOLE__


#include <vector>
#include "Camera.h"
#include "World.h"
#include "Points.h"
#include "Vectors.h"
#include <iostream>
#include "Light.h"


class PinholeCamera: public Camera {
    public:
        PinholeCamera(int _hr, int _vr, double d, const Vec3D& _up, const Point3D& pos, const Point3D &_lookAt, float p, int s, int pths) {
            h_res = _hr;
            v_res = _vr;
            distance = d;
            up = _up;
            camera_pos = pos;
            look_at = _lookAt;
            pixel_size = p;
            n_samples = s;
            paths = pths;
        }
        ~PinholeCamera() {}
        void render(std::vector<Object*> objetos, std::vector<Light*>& lights, Ambient& ambient);
};

void PinholeCamera::render(std::vector<Object*> objetos, std::vector<Light*>& lights, Ambient& ambient)
{
    Vec3D toPixel = w*distance + right*(-pixel_qtn_h/2.0) + iup*(pixel_qtn_v/2.0) - (this->iup/2.0) + (this->right/2.0); //while using anti-aliasing there is no need to be in the center of the pixel
    Vec3D down;
    Vec3D dir;
    std::vector<RGBColor> pixels;
    Vec3D lightX, lightNormal, lightZ;
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
    for (int i = 0; i < pixel_qtn_h*pixel_qtn_v; i++)
    {
        if ((i) % (int)pixel_qtn_h == 0)
        {
            down = down - iup;
            dir = toPixel + down;
        } else {
            dir = dir + right;
        }

        std::vector<Light*> lightPath = {};
        RGBColor sumColor;
        double energy = 0;
        for (int t = 0; t < this->paths; t++) {
            sumColor = sumColor + trace(Ray(camera_pos, dir), objetos, (*this), lights, &ambient, ambient.depth, lightX, lightNormal, lightZ, &lightPath, &energy);
        } 
        sumColor = sumColor/(double)this->paths;  
        pixels.push_back(sumColor);
    }
    std::ofstream pixelOutput("./image.ppm", std::ios::out | std::ios::binary);
    pixelOutput << "P6\n" << pixel_qtn_h << " " << pixel_qtn_v << "\n255\n";
    for (int i = 0; i < pixel_qtn_h*pixel_qtn_v; i++)
    {
        pixelOutput <<(unsigned char)(std::max(double(1), pixels[i].r)) <<
            (unsigned char)(std::max(double(1), pixels[i].g)) <<
            (unsigned char)(std::max(double(1), pixels[i].b));
    }
    pixelOutput.close();
}

#endif