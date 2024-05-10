#ifndef __SCREENTHREAD__
#define __SCREENTHREAD__

#include <thread>
#include <mutex>
#include <string>
#include <iostream>
#include <memory>
#include <chrono>

#include "World.h"
#include "Screen.h"


class ScreenThread {
    public:
        int id, minIndex, maxIndex, barLength = 30;
        RGBColor copySumColor;
        std::shared_ptr<Screen> screen;
        std::shared_ptr<MetropolisManager> metropolis_manager;

        ScreenThread(int i, int min, int max, std::shared_ptr<Screen> scr): id(i), minIndex(min), maxIndex(max), screen(scr) {
            this->metropolis_manager = std::shared_ptr<MetropolisManager>(new MetropolisManager());
        }
        ~ScreenThread() {}

        void operator()(std::mutex &lock, Vec3D toPixel, std::vector<Object*> &objects, Camera &camera, std::vector<Light*> &lights, Ambient &ambient, Vec3D lightX, Vec3D lightNormal, Vec3D lightZ) {
            
            Vec3D down;
            Vec3D dir;
    
            for (int i = minIndex; i < maxIndex; i++) {
                down = camera.getIUP() * (-1.0) * (i / camera.getPixelsH());
                dir = toPixel + down + (camera.getRight() * (i % camera.getPixelsH())); 
                
                RGBColor sumColor;
                int invalidCount = 0;
                metropolis_manager->energy = 0;
                for (auto point : metropolis_manager->goodPath) {
                    delete point;
                }
                metropolis_manager->goodPath = {};
                for (uint32_t j = 0; j < camera.getNPaths(); j++) {
                    auto temp = trace(Ray(camera.getPos(), dir), objects, lights, ambient, ambient.depth, lightX, lightNormal, lightZ, metropolis_manager, i);
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
                std::cout.flush();
                std::cout << " [" << bar << "] " << (int)(progress*100) << "%\r";
                lock.unlock();
            } 
        }
};

#endif