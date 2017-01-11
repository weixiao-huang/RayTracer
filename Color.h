//
// Created by Mike Huang on 2016/5/10.
//

#ifndef PHOTON_MAPPING_COLOR_H
#define PHOTON_MAPPING_COLOR_H


class Color
{
public:
    float r, g, b;

    Color(float x = 0.0f, float y = 0.0f, float z = 0.0f)
            : r(x), g(y), b(z) {};
    ~Color() {};

    Color operator+(const Color&) const;
    Color operator-(const Color&) const;
    Color operator*(const float&) const;
    Color operator/(const float&) const;

	Color mult(const Color&) const;
    float clamp(float x) { return x < 0 ? 0 : x > 1 ? 1 : x; }
    int toInt(float x);
};


#endif //PHOTON_MAPPING_COLOR_H
