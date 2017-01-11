//
// Created by Mike Huang on 2016/5/11.
//

#ifndef SMALLPPM_SCENE_H
#define SMALLPPM_SCENE_H

#include <vector>
#include "GeometricObj.h"
#include "MeshObj.h"

using std::vector;

struct Point2D {
    double x, y;
    Point2D(double x = 0., double y = 0.) : x(x), y(y) {}
};

struct Camera {
    Vec3 eye, lookat, up;
    Vec3 cu, cv, cw;
    double FOV;
    inline Camera(Vec3 eye, Vec3 lookat, Vec3 up, double FOV)
            : eye(eye), lookat(lookat), up(up), FOV(FOV) {
        cw = (eye - lookat).getNormalize();
        cu = (up * cw).getNormalize();
        cv = (cw * cu).getNormalize();
    }
};


class Scene {
public:
    int width, height;
    Color background;
    vector<GeometricObj*> obj;
    Camera* camera;

    Scene(int width, int height);
    ~Scene();

   // void loadObj(const char *filename, Color c, Refl_t refl, double scale = 1.0, Vec3 translation = Vec3(), Vec3 rotation = Vec3());
    IntersectPoint intersect(const Ray& ray, int&id);
    void createBox(double x1, double x2, double y1, double y2, double z1, double z2);
    Ray createRay(Point2D*);
};

#endif //SMALLPPM_SCENE_H
