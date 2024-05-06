#ifndef __METROPOLISMANAGER__
#define __METROPOLISMANAGER__

#include <iostream>
#include <vector>
#include "Light.h"


class MetropolisManager {
    public:
        double energy;
        std::vector<Light*> goodPath;
        std::vector<std::vector<Light*>> foundGoodPaths;

        MetropolisManager() {
            this->energy = 0;
            this->goodPath = {};
            this->foundGoodPaths = {};
        }

};




#endif