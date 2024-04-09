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
#include "Screen.h"


class ScreenThread {
    public:
        int id, minIndex, maxIndex;
        RGBColor copySumColor;
        Screen *screen;

        ScreenThread(int i, int min, int max, Screen *scr): id(i), minIndex(min), maxIndex(max), screen(scr) {}
        ~ScreenThread() {}

        void operator()(std::mutex &lock, Vec3D toPixel, std::vector<Object*> objetos, Camera camera, std::vector<Light*> lights, Ambient ambient, Vec3D lightX, Vec3D lightNormal, Vec3D lightZ) {
            
            Vec3D down;
            Vec3D dir;
    
            for (int i = minIndex; i < maxIndex; i++) {
                down = camera.getIUP() * (-1.0) * (i / camera.getPixelsH());
                dir = toPixel + down + (camera.getRight() * (i % camera.getPixelsH())); 
                
                RGBColor sumColor;
                int invalidCount = 0;
                for (int j = 0; j < camera.getNPaths(); j++) {
                    RGBColor temp = trace(Ray(camera.getPos(), dir), objetos, camera, lights, ambient, ambient.depth, lightX, lightNormal, lightZ, j);
                    bool invalidPath = (std::isnan(temp.r) || std::isnan(temp.g) || std::isnan(temp.b));
                    sumColor = sumColor + (invalidPath ? RGBColor() : temp);
                    if (invalidPath) invalidCount++;
                }
                
                sumColor = sumColor/((double)camera.getNPaths() - invalidCount);
                lock.lock();
                screen->pixels[i] = sumColor;
                screen->checkPixel();
                float progress = (float)screen->getDonePixelsQtn()/(float)screen->getPixelQtn();
                std::cout << (int)(progress * 100) << "%\r";
                std::cout.flush();
                lock.unlock();
            }
            std::cout << "Thread-" << this->id << " finished rendering!" << "\n"; 
        }
};



#endif