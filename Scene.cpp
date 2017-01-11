//
// Created by Mike Huang on 2016/5/11.
//

#include <cmath>
#include <cstdio>
#include <cstring>
#include "Scene.h"

Scene::Scene(int width, int height) : width(width), height(height) {
    background = Color();
    camera = new Camera(Vec3(75, 20, 300), Vec3(75, 40, 0), Vec3(0.0, 1.0, 0.0), 60);

    createBox(0, 150, 0, 150, 0, 300);
/*	obj.push_back(new MeshObj("./resources/obj/kitten.50k.obj", Vec3(0, 0, 0), 0.4, Vec3(120, 0, 140), Color(0.855f, 0.439f, 0.839f), DIFF));
	obj.push_back(new MeshObj("./resources/obj/bunny.fine.obj", Vec3(0, 0, 0), 200, Vec3(40, -8, 130), Color(0.859f, 0.439f, 0.576f), DIFF));
	obj.push_back(new MeshObj("./resources/obj/horse.fine.90k.obj", Vec3(-90, -90, 0), 20, Vec3(80, 16, 150), Color(1.000f, 0.627f, 0.478f), REFR));
	
	obj.push_back(new MeshObj("./resources/obj/fixed.perfect.dragon.100K.0.07.obj", Vec3(0, 0, 0), 30, Vec3(70, 22, 80), Color(0.255f, 0.412f, 0.882f), SPEC)); //
	obj.push_back(new Sphere(7, Vec3(60, 7, 180), Color(1.000f, 0.973f, 0.863f), REFR));//Middle
	obj.push_back(new Sphere(15, Vec3(120, 15, 50), Color(1.000f, 1.f, 1.f) * 0.9, SPEC));//Middle*/
//	obj.push_back(new MeshObj("./resources/obj/Buddha.obj", Vec3(0, 0, 0), 30, Vec3(75, 30, 140), Color(0.847f, 0.749f, 0.847f), DIFF));
//	obj.push_back(new MeshObj("./resources/obj/rocker-arm.18k.obj", Vec3(-90, 45, -45), 40, Vec3(30, 60, 50), Color(1.f, 1.f, 1.f) * 0.9, SPEC));

    // loadObj("./resources/obj/dinosaur.2k.obj",Color(1.f, 1.f, 1.f) * .999f, SPEC, 1., Vec3(50, 20, 50), Vec3(0., 0., 0.));
//    loadObj("./resources/obj/cube.obj", Color(1.f, 1.f, 1.f) * .999f, SPEC, 40, Vec3(30, 10, 80), Vec3(10.0, 10.0, 10.0));
//    obj.push_back(new Box(Vec3(30, 10, 10), Vec3(90, 30, 150), Color(1.f,1.f,1.f)*.999f, DIFF));
	  obj.push_back(new Sphere(15, Vec3(30, 15, 120), Color(1.f, 1.f, 1.f)*.999f, SPEC));//Mirror left
    obj.push_back(new Sphere(15, Vec3(70, 15, 70),       Color(1.f,1.f,1.f)*.999f, REFR));//Glass right
    obj.push_back(new Sphere(10, Vec3(50, 10, 100),       Color(1.f,1.f,1.f)*.999f, DIFF));//Middle

//    obj.push_back(new Plane(Vec3( 0,  0, 100), Vec3(0.0, 0.0, 1.0), Color(.75f,.75f,.75f),REFR));
//    obj.push_back(new Triangle(Vec3(30, 75, 80), Vec3(70, 95, 80), Vec3(50, 50, 30), Color(1.f,1.f,1.f)*.999f, REFR));
//    obj.push_back(new Triangle(Vec3(60, 15, 80), Vec3(30, 75, 30), Vec3(60, 75, 80), Color(1.f,1.f,1.f)*.999f, REFR));
}

Scene::~Scene() {
    for (int i = 0; i < obj.size(); ++i) {
        delete obj[i];
    }
}

// find the closet interection
IntersectPoint Scene::intersect(const Ray& ray, int&id) {
    IntersectPoint ip = IntersectPoint();
    for(int i = 0; i < obj.size(); i++){
        IntersectPoint tmpIp = obj[i]->intersect(ray);
        if(tmpIp.t < ip.t){
            ip = tmpIp;
            id = i;
        }
    }
    return ip;
}

Ray Scene::createRay(Point2D* point2D) {
    double dis = height * tan(camera->FOV * PI / 180.);
    double x = point2D->x - width / 2 + 0.5;
    double y = point2D->y - height / 2 + 0.5;
    Vec3 d = camera->cu * x - camera->cv * y - camera->cw * dis;
    return Ray(camera->eye, d.getNormalize());  // how about + d / dis * 140. ?
}


void Scene::createBox(double x1, double x2, double y1, double y2, double z1, double z2) {
	obj.push_back(new Plane(Vec3(x1, 0, 0), Vec3(x2 - x1, 0.0, 0.0), Color(.75f, .25f, .25f) * 0.f, DIFF,
		"resources/texture/wall4.jpg", Vec3(0, 1, 0))); // Left*/
	obj.push_back(new Plane(Vec3(x2, 0, 0), Vec3(x1 - x2, 0.0, 0.0), Color(.25f, .25f, .75f) * 0.f, DIFF,
		"resources/texture/wall4.jpg", Vec3(0, 1, 0))); // Right
	obj.push_back(new Plane(Vec3(0, y1, 0), Vec3(0.0, y2 - y1, 0.0), Color(.75f, .75f, .75f) * 0.f, DIFF,
		"resources/texture/floor2.jpg", Vec3(0, 0, 1))); // Bottom*/
    obj.push_back(new Plane(Vec3(0, y2, 0), Vec3(0.0, y1 - y2, 0.0), Color(.75f,.75f,.75f),DIFF)); // Top
	obj.push_back(new Plane(Vec3(0, 0, z1), Vec3(0.0, 0.0, z2 - z1), Color(.75f, .75f, .75f) * 0.f, DIFF,
		"resources/texture/wall5.jpg", Vec3(0, -1, 0))); // Back*/
	obj.push_back(new Plane(Vec3(0, 0, z2), Vec3(0.0, 0.0, z1 - z2), Color(), DIFF,
		"resources/texture/marble2.jpg", Vec3(0, -1, 0))); // Front
}
/*
void Scene::loadObj(const char *filename, Color c, Refl_t refl, double scale, Vec3 translation, Vec3 rotation) {
    FILE* file = fopen(filename, "r");
    if (file == NULL){
        printf("Impossible to open the file! \n");
    }
    Triangle* tri;
    vector<Vec3> vertex;
    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "v") == 0) {
            double x, y, z, x1, y1, z1;
            fscanf(file, "%lf %lf %lf\n", &x, &y, &z);
            x1 = x;
            y1 = y * cos(rotation.x * PI / 180) - z * sin(rotation.x * PI / 180);
            z1 = y * sin(rotation.x * PI / 180) + z * sin(rotation.x * PI / 180);

            x = x1 * cos(rotation.y * PI / 180) - z1 * sin(rotation.y * PI / 180);
            y = y1;
            z = x1 * sin(rotation.y * PI / 180) + z1 * cos(rotation.y * PI / 180);

            x1 = x * cos(rotation.z * PI / 180) - y * sin(rotation.z * PI / 180);
            y1 = x * sin(rotation.z * PI / 180) + y * cos(rotation.z * PI / 180);
            z1 = z;

            x1 *= scale; y1 *= scale; z1 *= scale;

            vertex.push_back(Vec3(x1, y1, z1) + translation);
        } else if (strcmp(lineHeader, "f") == 0) {
            Vec3 tmp_tri;
            fscanf(file, "%lf %lf %lf\n", &tmp_tri.x, &tmp_tri.y, &tmp_tri.z);
            tri = new Triangle(vertex[tmp_tri.x - 1], vertex[tmp_tri.y - 1], vertex[tmp_tri.z - 1], c, refl);
            obj.push_back(tri);
        }
    }
}*/