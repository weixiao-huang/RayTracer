//
// Created by Mike Huang on 2016/5/10.
//

#ifndef PHOTON_MAPPING_VEC3_H
#define PHOTON_MAPPING_VEC3_H

#include <assert.h>

extern const double EPS;
extern const double PI;
extern const double M_MAX;

class Vec3 {
public:
    double x, y, z;
    Vec3(double x = 0.0, double y = 0.0, double z = 0.0)
            : x(x), y(y), z(z) {};
    virtual ~Vec3() {};

    Vec3 operator+(const Vec3&) const;
    Vec3 operator-(const Vec3&) const;
    Vec3 operator+(double& b) const;
    Vec3 operator-(double& b) const;
    Vec3 operator*(const double&) const;
    Vec3 operator/(const double&) const;
    Vec3 operator*(const Vec3&) const;  // cross product

    inline double& operator[](int index) {
        assert(index >=0 && index < 3);
        return index == 0 ? x : (index == 1 ? y : z);
    }
    inline const double& operator[](int index) const {
        assert(index >=0 && index < 3);
        return index == 0 ? x : (index == 1 ? y : z);
    }

    double dot(const Vec3&) const ;
    double module();
    double module2();
    Vec3 getNormalize();

    Vec3 reflect(Vec3 N);
    Vec3 refract(Vec3 N, double n);

};

struct Ray {
    Vec3 o, d;
    Ray() {}
    Ray(Vec3 o, Vec3 d) : o(o), d(d.getNormalize()) {}
};

#endif //PHOTON_MAPPING_VEC3_H
