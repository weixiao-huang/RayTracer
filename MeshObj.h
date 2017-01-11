//
// Created by Mike Huang on 2016/6/24.
//

#ifndef SMALLPPM_MESHOBJ_H
#define SMALLPPM_MESHOBJ_H


#include "core/GeometricObj.h"
#include "core/SimpleObject.h"

class KdTreeNode : public Box {
    void createNode(Vec3 min, Vec3 max, vector<Triangle> triangles, int depth);
public:
    KdTreeNode *leftChild, *rightChild;
    int depth;
    vector<Triangle> triangles;

    KdTreeNode(const vector<Triangle>& triangles);
    KdTreeNode(Vec3 min, Vec3 max, vector<Triangle> triangles, int depth);

    IntersectPoint intersect(const Ray& ray) const;

};

class MeshObj : public GeometricObj {
    KdTreeNode* root;
public:
    MeshObj(const char* objFile, Vec3 rotation, double scale, Vec3 translation, Color c, Refl_t refl);
    ~MeshObj() { delete root; }

    IntersectPoint intersect(const Ray& ray) const;
	Color getTextureColor(const Vec3&) const {
		return Color();
	}
};


#endif //SMALLPPM_MESHOBJ_H
