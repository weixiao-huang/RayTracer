//
// Created by Mike Huang on 2016/5/11.
//

#ifndef SMALLPPM_PHOTONUTIL_H
#define SMALLPPM_PHOTONUTIL_H

#include "Vec3.h"
#include "Color.h"

#define MAX(x, y) ((x > y) ? x : y)

struct AABB {
	Vec3 min, max; // axis aligned bounding box
    inline void fit(const Vec3 &p) {
		for (int i = 0; i < 3; i++)	{
			if (p[i] < min[i]) min[i] = p[i];
			max[i] = MAX(p[i], max[i]);
		}
    }
    inline void reset() {
        min = Vec3( M_MAX,  M_MAX,  M_MAX);
        max = Vec3(-M_MAX, -M_MAX, -M_MAX);
    }
};

struct HPoint {
    Color f, flux;
    Vec3 pos, nrm;
    double r2;
    unsigned int n; // n = N / ALPHA in the paper
    int pix;
};

struct List {
	HPoint *id;
	List *next;
};


class PhotonUtil {
public:
    static const int primes[61]; // Halton sequence with reverse permutation
    PhotonUtil();
    ~PhotonUtil() {};

    inline int rev(const int i,const int p) {
		return i == 0 ? i : p - i;
    }

    // spatial hash function
    inline unsigned int hash(const int ix, const int iy, const int iz) {
        return (unsigned int)((ix * 73856093)^(iy * 19349663)^(iz * 83492791)) % num_hash;
    }


    int num_hash, pixel_index, num_photon;
    double hash_s;
    List **hash_grid;
    List *hitpoints;
    AABB hpbbox;

    double hal(const int b, int j);
    void build_hash_grid(const int w, const int h);
	void genp(Ray* pr, Color* f, int i, Vec3 position, Color flux);  // generate a photon ray from the point light source with QMC
    List* ListAdd(HPoint *i, List* h);
};


#endif //SMALLPPM_PHOTONUTIL_H
