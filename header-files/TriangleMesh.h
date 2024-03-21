#ifndef __TMESH__
#define __TMESH__

#include <vector>
#include "Object.h"
#include "RGBColor.h"
#include "Material.h"
#include "Plane.h"
#include "Points.h"
#include "Ray.h"
#include "HitInfo.h"
#include "Vectors.h"

class TriangleMesh: public Object {
    public:
        Material *material;
        bool castShadows;
        int nTriangles, nVertices, triangleIndice = 0;
        std::vector<Point3D> vertices;
        std::vector<Point3I> triangles;
        std::vector<Vec3D> triangleNormals;
        std::vector<Vec3D> verticesNormals;
        TriangleMesh(int n, int v, Material *m, bool s): nTriangles(n), nVertices(n), material(m), castShadows(s) {}
        ~TriangleMesh() {}
    private:
        bool rayObjectIntersect(const Ray &ray, double *tmin, HitInfo &info);
        RGBColor getColor();
        double getKd();
        double getKs();
        double getKa();
        double getKr();
        double getKt();
        double getPhongExp();
        double getIor();
        Vec3D getNormal(const Point3D &hit, const Ray &ray);
        bool getShadows();
        bool getCastShadows();
        std::vector<Point3D> sampleObject();
        char getObjectType();
};

bool TriangleMesh::rayObjectIntersect(const Ray &ray, double *tmin, HitInfo &info) 
{
    double min = infinity;
    bool hit = false;
    for (int i = 0; i < triangles.size(); i++)
    {
        Point3D A, B, C;
        A = vertices[triangles[i].x];
        B = vertices[triangles[i].y];
        C = vertices[triangles[i].z]; 
        Vec3D tPlaneNormal = (A - B) ^ (A - C);
        if (((ray.direction * (-1)) * tPlaneNormal) <= 0)
        {
            tPlaneNormal = (A - C) ^ (A - B);
        }
        Material *tempMaterial = new Material{
            RGBColor(), 0, 0, 0, 0, 0, 0, false, 0
        };
        Plane *tPlane = new Plane(tPlaneNormal, A, tempMaterial, true);
        Point3D pHit;
        if (tPlane->rayObjectIntersect(ray, tmin, info)) // to-do: the intersecion fails when 3 respective coordinates on
        {   // diferent points, equals to 0
            pHit = ray.origin + ray.direction*(*tmin);      
            //|A.x B.x C.x||a|   |X|
            //|A.y B.y C.y||b| = |Y|
            //|A.z B.z C.z||g|   |Z|
            // gamma = 1 - alpha - beta
            double det = A.x*((B.y*C.z) - (C.y*B.z))  + B.x*((C.y*A.z) - (A.y*C.z)) + C.x*((A.y*B.z) - (B.y*A.z)); 

            double alpha = (pHit.x*((B.y*C.z) - (C.y*B.z))  + B.x*((C.y*pHit.z) - (pHit.y*C.z)) + C.x*((pHit.y*B.z) - (B.y*pHit.z)))/det;
            // if (alpha > 1 || alpha < 0) return false;

            double beta = (A.x*((pHit.y*C.z) - (C.y*pHit.z))  + pHit.x*((C.y*A.z) - (A.y*C.z)) + C.x*((A.y*pHit.z) - (pHit.y*A.z)))/det;

            
            if (beta > 0 && alpha > 0 && (beta + alpha < 1))
            {
                hit = true;
                if ((*tmin) < min)
                {
                    info.hit_object = true;
                    this->triangleIndice = i;
                    min = (*tmin);
                }
            }
            // double gama = (A.x*B.y*pHit.z - A.x*pHit.y*B.z - B.x*A.y*pHit.z + B.x*pHit.y*A.z + pHit.x*A.y*B.z - pHit.x*B.y*A.z)/det; 
        }
        free(tPlane);
        free(tempMaterial);
    }

    if (hit)
    {
        (*tmin) = min;
        return true;
    }
    return false;
}
RGBColor TriangleMesh::getColor()
{
    return this->material->color;
}
double TriangleMesh::getKd()
{
    return this->material->kd;
}
double TriangleMesh::getKs()
{
    return this->material->ks;
}
double TriangleMesh::getKa()
{
    return this->material->ka;
}
double TriangleMesh::getKr()
{
    return this->material->kr;
}
double TriangleMesh::getKt()
{
    return this->material->kt;
}
double TriangleMesh::getPhongExp()
{
    return this->material->phong;
}
double TriangleMesh::getIor()
{
    return this->material->ior;
}
Vec3D TriangleMesh::getNormal(const Point3D &hit, const Ray &ray)
{
    Point3D A, B, C;
    A = vertices[triangles[triangleIndice].x];
    B = vertices[triangles[triangleIndice].y];
    C = vertices[triangles[triangleIndice].z];
    Vec3D tPlaneNormal = (A - B) ^ (A - C);
    if (((ray.direction * (-1)) * tPlaneNormal) <= 0)
    {
        tPlaneNormal = (A - C) ^ (A - B);
    }
    return Vec3D::normalize(tPlaneNormal);
}
bool TriangleMesh::getShadows()
{
    return this->material->getShadows;
}
bool TriangleMesh::getCastShadows()
{
    return this->castShadows;
}
std::vector<Point3D> TriangleMesh::sampleObject()
{
    std::vector<Point3D> samples = {Point3D()};
    return samples;
}
char TriangleMesh::getObjectType()
{
    return 't';
}



#endif