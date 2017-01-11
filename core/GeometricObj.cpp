//
// Created by Mike Huang on 2016/5/11.
//

#include <cmath>
#include <cstdio>
#include <cstring>
#include "GeometricObj.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

void GeometricObj::loadTexture(const char* textureFile) {
	int w, h, dim;
	unsigned char* data = stbi_load(textureFile, &w, &h, &dim, 0);
//	texture = new Color[w * h];
	//tmpColor = new Color[w * h];
	int id = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			texture.push_back(Color((float)data[3 * id] / 255.f,
				(float)data[3 * id + 1] / 255.f,
				(float)data[3 * id + 2] / 255.f));
			id++;
			//tmpColor[id++] = Color();
		}
	}
	textureSize = std::make_pair(w, h);
}

IntersectPoint Sphere::intersect(const Ray &ray) const {
    // ray-sphere intersection returns distance
    double b = (p - ray.o).dot(ray.d);
    double c = (ray.o - p).dot(ray.o - p) - rad * rad;
    double det = b*b - c, t;

    if (det < 0.0) {
        return IntersectPoint();
    } else {
        det = sqrt(det);
        return (t = b-det) > EPS ?
               IntersectPoint(t, ray.o + ray.d * t - this->p) :
               ((t = b + det) > EPS ? IntersectPoint(t, ray.o + ray.d * t - this->p) : IntersectPoint());
    }
}

Color Sphere::getTextureColor(const Vec3& p) const {
	if (textureSize.first == 0 && textureSize.second == 0) {
		return Color();
	}
	Vec3 normal = (p - this->p).getNormalize();
	double a = acos(-normal.z);
	double b = acos(std::min(std::max(normal.y / sin(a), -1.0), 1.0));
	double u = a / PI;
	double v = b / PI;
	u = u - (int)u;
	v = v - (int)v;
	u = u < 0 ? u + 1 : u;
	v = v < 0 ? v + 1 : v;
	int i = (int)(u * textureSize.first), j = (int)(v * textureSize.second);
	//tmpColor[i * textureSize.second + j] = texture[i * textureSize.second + j];
	return texture[i * textureSize.second + j];
}

IntersectPoint Plane::intersect(const Ray &ray) const {
    double t = (p - ray.o).dot(n) / ray.d.dot(n);
    return t > EPS ? IntersectPoint(t, n) : IntersectPoint();
}

Color Plane::getTextureColor(const Vec3& p) const {
	if (textureSize.first == 0 && textureSize.second == 0) {
		return Color();
	}
	double scale = 150;
	double u = dx.dot(p) * textureSize.first / scale;
	double v = dy.dot(p) * textureSize.second / scale;
	int i = (int)u % textureSize.first, j = (int)v % textureSize.second;
	i = i < 0 ? i + textureSize.first : i;
	j = j < 0 ? j + textureSize.second : j;
	//tmpColor[i * textureSize.second + j] = texture[i * textureSize.second + j];
	return texture[i * textureSize.second + j];

	/*	u = u - (int)u;
	v = v - (int)v;
	u = u < 0 ? u + 1 : u;
	v = v < 0 ? v + 1 : v;
	int i = (int)(u * textureSize.first), j = (int)(v * textureSize.second);*/
}

IntersectPoint Triangle::intersect(const Ray &ray) const {
    double u, v, t;

    Vec3 E1 = this->v[0] - this->v[1], E2 = this->v[0] - this->v[2];
    Vec3 S = this->v[0] - ray.o;
    double det = ray.d.dot(E1 * E2);

    if (fabs(det) < EPS) return M_MAX;

    t = S.dot(E1 * E2);
    u = ray.d.dot(S * E2);
    v = ray.d.dot(E1 * S);
    det = 1. / det;
    t *= det;
    u *= det;
    v *= det;
    return t > EPS && u >= 0 && v >= 0 && u <= 1 && v <= 1 && u + v <= 1 ?
           IntersectPoint(t, (this->v[1] - this->v[0]) * (this->v[2] - this->v[1])) : IntersectPoint();
//    return t > 0 && u > 0 && v > 0 && u < 1 && v < 1 && u + v < 1 ? (refl == REFR ? t + EPS : t - EPS): M_MAX;
//    return t > EPS && u > EPS && v > EPS && u < 1+EPS && v < 1+EPS && u + v < 1+EPS ? (refl == REFR ? t + EPS : t - EPS): M_MAX;
}

IntersectPoint Box::intersect(const Ray &ray) const {
    Vec3 v, tv;
    Vec3 dir = this->v[1] - this->v[0];
    double t = -M_MAX;
    int dim = 0;
    for (int i = 0; i < 3; ++i) {
        if (ray.d[i] * dir[i] >= 0) {
            v[i] = this->v[0][i];
        } else {
            v[i] = this->v[1][i];
        }
        tv[i] = (v[i] - ray.o[i]) / ray.d[i];
        if (t < tv[i]){
            t = tv[i];
            dim = i;
        }
    }
    if (t < EPS) return IntersectPoint();

    // whether the intersect point is on the face
    Vec3 intersectPoint = ray.o + ray.d * t;
    Vec3 dir2D = intersectPoint - v;
    if (dir2D.dot(dir) < 0) dir2D = dir2D * (-1);
    for (int i = 0; i < 2; ++i) {
        int j = (dim + i + 1) % 3;
        if (fabs(dir2D[j]) > fabs(dir[j])) {
            return IntersectPoint();
        }
    }
    Vec3 normal[3] = {
            Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)
    };
    for (int i = 0; i < 2; ++i) {
        dir = this->v[i] - this->v[(i + 1) % 2];
        Vec3 tmp = intersectPoint - this->v[i];
        for (int j = 0; j < 3; ++j) {
            if (fabs(tmp[j]) < EPS) {
                return IntersectPoint(t, normal[j] * (dir[j] / fabs(dir[j])));
            }
        }
    }
    return IntersectPoint();
}
