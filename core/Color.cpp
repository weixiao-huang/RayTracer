//
// Created by Mike Huang on 2016/5/10.
//

#include "Color.h"
#include <cmath>

Color Color::operator+(const Color& V) const {
    return Color(V.r + r, V.g + g, V.b + b);
}
Color Color::operator-(const Color& V) const {
    return Color(r - V.r, g - V.g, b - V.b);
}
Color Color::operator*(const float& f) const {
    return Color(r * f, g * f, b * f);
}
Color Color::operator/(const float& f) const {
    return Color(r / f, g / f, b / f);
}

Color Color::mult(const Color& f) const {
	return Color(r * f.r, g * f.g, b * f.b);
}

int Color::toInt(float x) {
    return (int)(powf(clamp(x), 1.f / 2.2f) * 255.f + 0.5f);
}