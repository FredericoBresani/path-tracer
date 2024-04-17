#ifndef __PINHOLE__
#define __PINHOLE__


#include "Camera.h"
#include "World.h"
#include <iostream>
#include "Light.h"
#include "ScreenThread.h"
#include "Screen.h"


auto screen = std::make_shared<Screen>();
std::mutex lock;

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
        void render(std::vector<Object*> objects, std::vector<Light*> lights, Ambient ambient);
};

void PinholeCamera::render(std::vector<Object*> objects, std::vector<Light*> lights, Ambient ambient)
{
    auto toPixel = w*distance + right*(-pixel_qtn_h/2.0) + iup*(pixel_qtn_v/2.0) - (this->iup/2.0) + (this->right/2.0); //while using anti-aliasing there is no need to be in the center of the pixel
    Vec3D down;
    Vec3D dir;
    screen->setPixelQtn(pixel_qtn_h*pixel_qtn_v);
    Vec3D lightX, lightNormal, lightZ;
    for (uint32_t j = 0; j < lights.size(); j++) {  
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
    int threadRange = ceil((double)pixel_qtn_h*pixel_qtn_v/(double)ambient.nThreds);
    int start = 0, end = threadRange;
    std::vector<std::thread> threads;
    for (int i = 0; i < ambient.nThreds && start <= pixel_qtn_h*pixel_qtn_v - 1; i++) {
        if (end > pixel_qtn_h*pixel_qtn_v - 1) {
            threads.push_back(
                std::thread(
                    ScreenThread(i, start, pixel_qtn_h*pixel_qtn_v, screen),
                    std::ref(lock), 
                    std::ref(toPixel),
                    std::ref(objects),
                    std::ref((*this)),
                    std::ref(lights),
                    std::ref(ambient),
                    std::ref(lightX),
                    std::ref(lightNormal),
                    std::ref(lightZ)
                )
            );
        } else {
            threads.push_back(
                std::thread(
                    ScreenThread(i, start, end, screen), 
                    std::ref(lock), 
                    std::ref(toPixel),
                    std::ref(objects),
                    std::ref((*this)),
                    std::ref(lights),
                    std::ref(ambient),
                    std::ref(lightX),
                    std::ref(lightNormal),
                    std::ref(lightZ)
                )
            );
        }
        start += threadRange;
        end += threadRange;
    }
    for (uint32_t t = 0; t < threads.size(); t++) {
        threads[t].join();
    }
    std::cout << " [==============================] 100% \n";
    /* testing inconsitency
    std::ofstream MyFile ("output.txt");
    for (int i = 0; i < pixel_qtn_h*pixel_qtn_v; i++) {
        MyFile << "RGBColor(" << pixels[i].r << ", " << pixels[i].g << ", " << pixels[i].b << ");\n";
    }
    MyFile.close();
    */
    std::ofstream pixelOutput("./image.ppm", std::ios::out | std::ios::binary);
    pixelOutput << "P6\n" << pixel_qtn_h << " " << pixel_qtn_v << "\n255\n";
    for (auto pixel : screen->pixels)
    {
        pixelOutput <<  (unsigned char)(std::max(double(1), pixel.r)) <<
                        (unsigned char)(std::max(double(1), pixel.g)) <<
                        (unsigned char)(std::max(double(1), pixel.b));
    }
    pixelOutput.close();
}

#endif