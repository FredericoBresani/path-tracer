#ifndef __SCREENTHREAD__
#define __SCREENTHREAD__

#include <thread>
#include <mutex>
#include <string>
#include <iostream>
#include <memory>

#include "Light.h"
#include "World.h"
#include "Object.h"
#include "Camera.h"
#include "Ambient.h"
#include "Screen.h"


class ScreenThread {
    public:
        int id, minIndex, maxIndex, barLength = 30;
        RGBColor copySumColor;
        std::shared_ptr<Screen> screen;

        ScreenThread(int i, int min, int max, std::shared_ptr<Screen> scr): id(i), minIndex(min), maxIndex(max), screen(scr) {}
        ~ScreenThread() {}

        void operator()(std::mutex &lock, Vec3D toPixel, std::vector<Object*> objetos, Camera camera, std::vector<Light*> lights, Ambient ambient, Vec3D lightX, Vec3D lightNormal, Vec3D lightZ) {
            
            Vec3D down;
            Vec3D dir;
    
            for (int i = minIndex; i < maxIndex; i++) {
                down = camera.getIUP() * (-1.0) * (i / camera.getPixelsH());
                dir = toPixel + down + (camera.getRight() * (i % camera.getPixelsH())); 
                
                RGBColor sumColor;
                int invalidCount = 0;
                for (uint32_t j = 0; j < camera.getNPaths(); j++) {
                    auto temp = trace(Ray(camera.getPos(), dir), objetos, camera, lights, ambient, ambient.depth, lightX, lightNormal, lightZ, j);
                    bool invalidPath = (std::isnan(temp.r) || std::isnan(temp.g) || std::isnan(temp.b));
                    sumColor = sumColor + (invalidPath ? RGBColor() : temp);
                    if (invalidPath) invalidCount++;
                }
                
                sumColor = sumColor/((double)camera.getNPaths() - invalidCount);
                lock.lock();
                screen->pixels[i] = sumColor;
                screen->checkPixel();
                auto progress = (float)screen->getDonePixelsQtn()/(float)screen->getPixelQtn();
                auto bars = (int)(((float)this->barLength)*progress);
                std::string bar = "";
                for (uint32_t t = 0; t < bars; t++) {
                    bar += "=";
                }
                for (uint32_t t = bars; t < this->barLength; t++) {
                    bar += " ";
                }
                std::cout << " [" << bar << "] " << (int)(progress*100) << "%\r";
                std::cout.flush();
                lock.unlock();
            } 
        }
};

#endif