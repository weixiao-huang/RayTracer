//
// Created by Mike Huang on 2016/6/24.
//

#include <cmath>
#include "MeshObj.h"

MeshObj::MeshObj(const char *objFile, Vec3 rotation, double scale, Vec3 translation,
                 Color c, Refl_t refl)
        : GeometricObj(c, refl) {
    SimpleOBJ::CSimpleObject simpleObject;
    simpleObject.LoadFromObj(objFile);

    vector<Triangle> triangles;

    rotation = rotation * PI / 180;
    for (int i = 0; i < simpleObject.m_nTriangles; ++i) {
        Vec3 p[3];
        for (int j = 0; j < 3; ++j) {
            int vId = simpleObject.m_pTriangleList[i][j];
            Vec3 v1 = simpleObject.m_pVertexList[vId], v2;
			
			v2.x = v1.x;
			v2.y = v1.y * cos(rotation.x) - v1.z * sin(rotation.x);
			v2.z = v1.y * sin(rotation.x) + v1.z * cos(rotation.x);
			
			v1.x = v2.x * cos(rotation.y) - v2.z * sin(rotation.y);
			v1.y = v2.y;
			v1.z = v2.x * sin(rotation.y) + v2.z * cos(rotation.y);

			v2.x = v1.x * cos(rotation.z) - v1.y * sin(rotation.z);
			v2.y = v1.x * sin(rotation.z) + v1.y * cos(rotation.z);
			v2.z = v1.z;

            p[j] = v2 * scale + translation;
        }
        triangles.push_back(Triangle(p[0], p[1], p[2], c, refl));
    }
    root = new KdTreeNode(triangles);

}

IntersectPoint MeshObj::intersect(const Ray &ray) const {
    if (root) {
        IntersectPoint inter = root->intersect(ray);
		return inter;
    }
    return IntersectPoint();
}


KdTreeNode::KdTreeNode(const vector<Triangle> &triangles) {
    leftChild = NULL;
    rightChild = NULL;
    Vec3 max, min;
    for (int i = 0; i < 3; ++i) {
        max[i] = -M_MAX;
        min[i] = M_MAX;
    }

    // for all triangles, find the max and min vector for each coordinate
    for (const auto& tri : triangles) {
        for (int i = 0; i < 3; ++i) {
            if (tri.max[i] > max[i]) max[i] = tri.max[i];
            if (tri.min[i] < min[i]) min[i] = tri.min[i];
        }
    }

    createNode(min, max, triangles, 0);
}

KdTreeNode::KdTreeNode(Vec3 min, Vec3 max, vector<Triangle> triangles, int depth) {
    createNode(min, max, triangles, depth);
}

void KdTreeNode::createNode(Vec3 min, Vec3 max, vector<Triangle> triangles, int depth) {
    this->depth = depth;
    v[0] = min; v[1] = max;

    if (triangles.size() < 3 || depth > 100) {
        this->triangles = std::move(triangles);
        leftChild = rightChild = NULL;
        return;
    }

    Vec3 tmpMin = min, tmpMax = max;
    if (depth % 3 == 0) {
        std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
            return a.max[0] < b.max[0];
        });
        tmpMax[0] = (triangles.begin() + triangles.size() / 2 - 1)->max[0];
    } else if (depth % 3 == 1) {
        std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
            return a.max[1] < b.max[1];
        });
        tmpMax[1] = (triangles.begin() + triangles.size() / 2 - 1)->max[1];
    } else if (depth % 3 == 2) {
        std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
            return a.max[2] < b.max[2];
        });
        tmpMax[2] = (triangles.begin() + triangles.size() / 2 - 1)->max[2];
    }

    vector<Triangle> leftTriangles(triangles.begin(), triangles.begin() + triangles.size() / 2);
    vector<Triangle> rightTriangles(triangles.begin() + triangles.size() / 2, triangles.end());

    double tMin = M_MAX;
    for (int i = 0; i < 3; ++i) {
        if (depth % 3 == i) {
            for (const auto& tri : rightTriangles) {
                double tmp = tri.min[i];
                if (tmp < tMin) tMin = tmp;
            }
            tmpMin[i] = tMin;
            break;
        }
    }

    leftChild = new KdTreeNode(min, tmpMax, leftTriangles, depth + 1);
    rightChild = new KdTreeNode(tmpMin, max, rightTriangles, depth + 1);
}

IntersectPoint KdTreeNode::intersect(const Ray &ray) const {
    if (!Box::intersect(ray).isHit()) return IntersectPoint();
    if (leftChild == NULL && rightChild == NULL) {
		double min = M_MAX;
        IntersectPoint inter = IntersectPoint();
        for (const auto& tri : triangles) {
            IntersectPoint tmpInter = tri.intersect(ray);
            if (tmpInter.isHit() && tmpInter.t < min) {
				min = tmpInter.t;
                inter = tmpInter;
            }
        }
        return inter;
    }
    IntersectPoint leftP = IntersectPoint();
    if (leftChild->Box::intersect(ray).isHit()) {
        leftP = leftChild->intersect(ray);
    }
    IntersectPoint rightP = IntersectPoint();
    if (rightChild->Box::intersect(ray).isHit()) {
        rightP = rightChild->intersect(ray);
    }

    return leftP.t < rightP.t ? leftP : rightP;
}
