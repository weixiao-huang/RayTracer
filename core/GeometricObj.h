//
// Created by Mike Huang on 2016/5/11.
//

#ifndef SMALLPPM_GEOMETRICOBJ_H
#define SMALLPPM_GEOMETRICOBJ_H

#include <vector>
#include <algorithm>
#include "Color.h"
#include "Vec3.h"

using std::vector;

enum Refl_t {DIFF, SPEC, REFR};

struct IntersectPoint {
    double t;
    Vec3 normal;

    IntersectPoint(double t = M_MAX, Vec3 normal = Vec3(), Color color = Color())
            : t(t), normal(normal.getNormalize()) {}

    bool isHit() { return t < M_MAX; }
};

class GeometricObj {
	void loadTexture(const char* textureFile);
public:
    Color c;
    Refl_t refl;
	vector<Color> texture;
	std::pair<int, int> textureSize;

    GeometricObj(Color c, Refl_t refl, const char* textureFile = NULL)
            : c(c), refl(refl) {
		textureSize = { 0, 0 };
		if (textureFile) loadTexture(textureFile);
	}
    virtual ~GeometricObj() {}

    virtual IntersectPoint intersect(const Ray& ray) const = 0;
	virtual Color getTextureColor(const Vec3&) const = 0;
};


class Sphere : public GeometricObj {
public:
    double rad;
    Vec3 p;

    Sphere(double radius, Vec3 p, Color c, Refl_t refl,
		const char* textureFile = NULL)
            : rad(radius), p(p), GeometricObj(c, refl, textureFile){}
    ~Sphere() {}

    IntersectPoint intersect(const Ray& ray) const;
	Color getTextureColor(const Vec3&) const;
};


class Plane : public GeometricObj {
	Vec3 dx, dy;
public:
    Vec3 p, n;

	Plane(Vec3 p, Vec3 n, Color c, Refl_t refl,
		const char* textureFile = NULL, Vec3 textureDir = Vec3())
            : p(p), n(n.getNormalize()), GeometricObj(c, refl, textureFile) {
		this->dx = (textureDir - this->n * this->n.dot(textureDir)).getNormalize();
		this->dy = (this->dx * this->n).getNormalize();
	}
    ~Plane() {}

    IntersectPoint intersect(const Ray& ray) const;
	Color getTextureColor(const Vec3&) const;
};


class Triangle : public GeometricObj {
public:
    Vec3 v[3];

    Triangle(Vec3 v1 = Vec3(), Vec3 v2 = Vec3(), Vec3 v3 = Vec3(),
             Color c = Color(), Refl_t refl = DIFF)
            : GeometricObj(c, refl) {
        v[0] = v1; v[1] = v2; v[2] = v3;
        for (int i = 0; i < 3; ++i) {
            max[i] = std::max(v[0][i], std::max(v[1][i], v[2][i]));
            min[i] = std::min(v[0][i], std::min(v[1][i], v[2][i]));
        }
    }
    ~Triangle() {}

	IntersectPoint intersect(const Ray& ray) const;
	Color getTextureColor(const Vec3&) const {
		return Color();
	}

    Vec3 max, min;
};

class Box : public GeometricObj {
protected:
    Vec3 v[2];
public:
    Box(Vec3 v1 = Vec3(), Vec3 v2 = Vec3(), Color c = Color(), Refl_t refl = DIFF)
            : GeometricObj(c, refl) {
        v[0] = v1; v[1] = v2;
    }
    Box(Vec3 v[2], Color c, Refl_t refl) : GeometricObj(c, refl) {
        this->v[0] = v[0]; this->v[1] = v[1];
    }
    ~Box() {}

	IntersectPoint intersect(const Ray& ray) const;
	Color getTextureColor(const Vec3&) const {
		return Color();
	}
};

#endif //SMALLPPM_GEOMETRICOBJ_H
