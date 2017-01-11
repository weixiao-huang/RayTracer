//
// Created by Mike Huang on 2016/5/10.
//

#include <cmath>
#include "Vec3.h"

const double EPS = 1e-6;
const double PI = 3.14159265359;
const double M_MAX = 1e20;

Vec3 Vec3::operator+(const Vec3& V) const {
    return Vec3(V.x + x, V.y + y, V.z + z);
}
Vec3 Vec3::operator-(const Vec3& V) const {
    return Vec3(x - V.x, y - V.y, z - V.z);
}
Vec3 Vec3::operator+(double &b) const {
    return Vec3(x + b, y + b, z + b);
}
Vec3 Vec3::operator-(double &b) const {
    return Vec3(x - b, y - b, z - b);
}
Vec3 Vec3::operator*(const double& f) const {
    return Vec3(x * f, y * f, z * f);
}
Vec3 Vec3::operator/(const double& f) const {
    return Vec3(x / f, y / f, z / f);
}
Vec3 Vec3::operator*(const Vec3& V) const {
    return Vec3(y * V.z - z * V.y,
                z * V.x - x * V.z,
                x * V.y - y * V.x);
}

double Vec3::dot(const Vec3& V) const {
    return x * V.x + y * V.y + z * V.z;
}
double Vec3::module() {
    return sqrt(module2());
}
double Vec3::module2() {
    return x * x + y * y + z * z;
}
Vec3 Vec3::getNormalize() {
    return *this / module();
}

Vec3 Vec3::reflect(Vec3 N) {
    return *this - N * (2 * dot(N));
}
Vec3 Vec3::refract(Vec3 N, double n) {
    Vec3 V = getNormalize();
    double cosI = -N.dot(V), cosT2 = 1 - (n * n) * (1 - cosI * cosI);
    if (cosI > EPS) return V * n + N * (n * cosI - sqrt(cosT2));
    return V.reflect(N);
}
