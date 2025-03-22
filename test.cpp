#include <iostream>
#include <vector>


class Something
{
    public:
        float d;
        Something(float dd): d(dd) {}

        float getNumber() {
            return this->d;
        }
};

class MetroManager {
    public:
        double energy;
        std::vector<Something*> goodPath;

        MetroManager() {
            this->energy = 0;
        }
};



int main() {
    MetroManager *metro = new MetroManager();
    metro->goodPath = { new Something(1.0), new Something(2.0), new Something(3.0) };
    std::vector<Something*> lightPath = { new Something(5.0), new Something(6.0), new Something(7.0) };
    
    

    for (auto point: metro->goodPath) {
        delete point;
        // point = nullptr;
    }

    metro->goodPath = {};

    for (auto point : lightPath) {
        metro->goodPath.push_back(point);
    }

    for (auto point : metro->goodPath) {
        if (point) std::cout << point->d << std::endl;
    }

    return 0;
}