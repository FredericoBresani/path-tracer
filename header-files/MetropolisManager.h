#ifndef __METROPOLISMANAGER__
#define __METROPOLISMANAGER__

#include <iostream>
#include <vector>
#include "Light.h"


class MetropolisManager {
    public:
        double energy;
        std::vector<Light*> goodPath;

        MetropolisManager() {
            this->energy = 0;
        }
        MetropolisManager(MetropolisManager &metro) {
            energy = metro.energy;
            goodPath = metro.goodPath;
        }

};




#endif