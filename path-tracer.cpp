#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <chrono>

#include "./header-files/Ambient.h"
#include "./header-files/Camera.h"
#include "./header-files/Definitions.h"
#include "./header-files/HitInfo.h"
#include "./header-files/Light.h"
#include "./header-files/Material.h"
#include "./header-files/Object.h"
#include "./header-files/pinholeCamera.h"
#include "./header-files/Plane.h"
#include "./header-files/PointLight.h"
#include "./header-files/Points.h"
#include "./header-files/Ray.h"
#include "./header-files/RGBColor.h"
#include "./header-files/Sphere.h"
#include "./header-files/TriangleMesh.h"
#include "./header-files/TriangleMeshLight.h"
#include "./header-files/Vectors.h"
#include "./header-files/World.h"

void render(std::vector<Object*> objects, std::vector<Light*> lights, Camera *camera, Ambient *ambient)
{   
    camera->render(objects, lights, (*ambient));
}

int main() {
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    Camera *camera;
    Ambient *ambient;
    char objectType;
    auto read = true;
    float _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18;
    while (scanf("%c %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", &objectType, &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11, &_12, &_13, &_14, &_15, &_16, &_17, &_18) != EOF && read) 
    {
        switch(objectType)
        {
            case 's': 
            {
                auto mater = new Material{
                    RGBColor(_5, _6, _7), _8, _9, _10, _11, _12, _13, (bool)_15, _16
                };
                objects.push_back(new Sphere(Point3D(_1, _2, _3), _4, mater, (bool)_14));
                break;    
            }
            case 't':
            {
                auto mater = new Material{
                    RGBColor(_3, _4, _5), _6, _7, _8, _9, _10, _11, (bool)_13, _14
                };
                auto mesh = new TriangleMesh((int)_1, (int)_2, mater, (bool)_12);

                float v1, v2, v3;
                int i1, i2, i3;
                while(_2 > 0)
                {
                    scanf("%f %f %f\n", &v1, &v2, &v3);
                    mesh->vertices.push_back(Point3D(v1, v2, v3));
                    _2--;
                }

                while(_1 > 0)
                {
                    scanf("%i %i %i\n", &i1, &i2, &i3);
                    mesh->triangles.push_back(Point3I(i1, i2, i3));
                    _1--;
                }
                
                objects.push_back(mesh);
                break;
            }
            case 'l':
            {
                if (_8 > 0 && _9 > 0) {
                    auto mater = new Material(RGBColor(_4, _5, _6), 0, 0, 0, 0, 0, 0, _13, 0);
                    auto mesh = new TriangleMesh((int)_8, (int)_9, mater, _12); 
                    float v1, v2, v3;
                    int i1, i2, i3;
                    while (_9 > 0) {
                        scanf("%f %f %f\n", &v1, &v2, &v3);
                        mesh->vertices.push_back(Point3D(v1, v2, v3));
                        _9--;
                    }
                    while(_8 > 0) {
                        scanf("%i %i %i\n", &i1, &i2, &i3);
                        mesh->triangles.push_back(Point3I(i1, i2, i3));
                        _8--;
                    }
                    auto l = new TriangleMeshLight(mater->color, mesh, (int)_14);
                    for (int i = 0; i < l->getLightSamples().size(); i++) {
                        lights.push_back(new PointLight(l->getLightSamples()[i], mater->color, (bool)_7, (int)_14));
                    }
                    lights.push_back(l);
                } else {
                    lights.push_back(new PointLight(Point3D(_1, _2, _3), RGBColor(_4, _5, _6), (bool)_7, (int)_14));
                }
                break;
            }
            case 'c':
            {
                camera = new PinholeCamera(_1, _2, _3, Vec3D(_4, _5, _6), Point3D(_7, _8, _9), Point3D(_10, _11, _12), _13, _14, _15);
                camera->makeCamera();
                break;
            } 
            case 'a':
            {
                ambient = new Ambient(RGBColor(_1, _2, _3), _4, _5, _6);
                break;
            }
            default:
            {
                read = false;
                break;
            }
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    render(objects, lights, camera, ambient);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "Time elapsed: " << duration.count() << "s" << std::endl;

    return 0;
}