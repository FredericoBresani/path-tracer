#ifndef __SCREENTHREAD__
#define __SCREENTHREAD__

#include <thread>
#include <mutex>
#include <vector>

#include "RGBColor.h"
#include "Light.h"
#include "Vectors.h"
#include "World.h"
#include "Ray.h"
#include "Object.h"
#include "Camera.h"
#include "Ambient.h"
#include "Points.h"

std::mutex lock;

class ScreenThread {
    public:
        int id, minIndex, maxIndex;
        RGBColor copySumColor;

        ScreenThread(int i, int min, int max): id(i), minIndex(min), maxIndex(max) {}
        ~ScreenThread() {}

        void operator()(std::vector<RGBColor> &pixels, Vec3D &toPixel, std::vector<Object*> &objetos, Camera &camera, std::vector<Light*> &lights, Ambient &ambient, Vec3D &lightX, Vec3D &lightNormal, Vec3D &lightZ) {
            
            Vec3D down;
            Vec3D dir;
    
            for (int i = minIndex; i < maxIndex; i++) {
                down = camera.getIUP() * (-1.0) * (i / camera.getPixelsH());
                dir = toPixel + down + (camera.getRight() * (i % camera.getPixelsH())); 
                
                RGBColor sumColor;
                
                for (int j = 0; j < camera.getNPaths(); j++) {
                    sumColor = sumColor + trace(Ray(camera.getPos(), dir), objetos, camera, lights, &ambient, ambient.depth, lightX, lightNormal, lightZ, j);
                }
                
                lock.lock();
                sumColor = sumColor/(double)camera.getNPaths();
                pixels[i] = sumColor;
                lock.unlock();
            }
            std::cout << "Thread-" << this->id << " finished rendering!" << "\n"; 
        }
};



#endif