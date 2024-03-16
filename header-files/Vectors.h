#ifndef __VECTORS__
#define __VECTORS__


template <typename T> class Vec2 
{
    public:
        T x, y;
        Vec2(): x(T(0)), y(T(0)) {}
        Vec2(T _x): x(T(_x)), y(T(_x)) {}
        Vec2(T _x, T _y): x(T(_x)), y(T(_y)) {}
        T operator * (const Vec2<T> &v) const { return (x*v.x) + (y*v.y); }
        Vec2<T> operator + (const Vec2<T> &v) const { return Vec2<T>(x + v.x, y + v.y); }
        Vec2<T> operator - (const Vec2<T> &v) const { return Vec2<T>(x - v.x, y - v.y); }
        Vec2<T> operator * (const Vec2<T> &v) const { return Vec2<T>(x*v.x, y*v.y); }
        static T norma(const Vec2<T> &v)
        {
            return pow((v.x*v.x) + (v.y*v.y), 0.5);
        }
        static VVec2<T> normalize(const Vec2<T> &v)
        {
            const T norma = v->norma();
            return Vec2<T>(v / norma);
        }
};

template <typename T> class Vec3 
{
    public:
        T x, y, z;
        Vec3(): x(T(0)), y(T(0)), z(T(0)) {}
        Vec3(T _x): x(T(_x)), y(T(_x)), z(T(_x)) {}
        Vec3(T _x, T _y, T _x): x(T(_x)), y(T(_y)), z(T(_z)) {}
        T operator * (const Vec3<T> &v) const { return (x*v.x) + (y*v.y) + (z*v.z); }
        Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
        Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
        Vec3<T> operator * (const T &t) const { return Vec3<T>(x*t, y*t, z*t); }
        Vec3<T> operator / (const T &t) const { return Vec3<T>(x/t, y/t, z/t); }
        Vec3<T> operator ^ (const Vec3<T>& v) const { return Vec3<T>(y*v.z - (z*v.y), z*v.x - (x*v.z), x*v.y - (y*v.x)); }
        static T norma(const Vec3<T> &v) 
        {
            return pow((v.x*v.x) + (v.y*v.y) + (v.z*v.z), 0.5);
        }
        static Vec3<T> normalize(const Vec3<T> &v)
        {
            const T norma = v->norma();
            return Vec3<T>(v / norma);
        }
};

typedef Vec2<double> Vec2D;
typedef Vec3<double> Vec3D;


#endif