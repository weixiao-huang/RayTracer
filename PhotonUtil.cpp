//
// Created by Mike Huang on 2016/5/11.
//

#include <cstdlib>
#include <cmath>
#include "PhotonUtil.h"

const int PhotonUtil::primes[61]={
        2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,
        83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,
        191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283
};


double PhotonUtil::hal(const int b, int j) {
    const int p = PhotonUtil::primes[b];
    double h = 0.0, f = 1.0 / (double)p, fct = f;
    while (j > 0) {
        h += rev(j % p, p) * fct;
		j /= p;
		fct *= f;
    }
    return h;
}

PhotonUtil::PhotonUtil() {
    hitpoints = NULL;
}

void PhotonUtil::build_hash_grid(const int w, const int h) {
    // find the bounding box of all the measurement points
    hpbbox.reset();
    List *lst = hitpoints;
    while (lst != NULL) {
        HPoint *hp = lst->id;
        lst=lst->next;
        hpbbox.fit(hp->pos);
    }

    // heuristic for initial radius
    Vec3 ssize = hpbbox.max - hpbbox.min;
    double irad = ((ssize.x + ssize.y + ssize.z) / 3.0) / ((w + h) / 2.0) * 2.0;

    // determine hash table size
    // we now find the bounding box of all the measurement points inflated by the initial radius
    hpbbox.reset();
    lst = hitpoints;
    int vphoton = 0;
    while (lst != NULL) {
        HPoint *hp = lst->id;
        lst = lst->next;
        hp->r2 = irad * irad;
        hp->n = 0;
        hp->flux = Color();
        vphoton++;
        hpbbox.fit(hp->pos - irad);
        hpbbox.fit(hp->pos + irad);
    }

    // make each grid cell two times larger than the initial radius
    hash_s = 1.0 / (irad * 2.0);
    num_hash = vphoton;

    // build the hash table
    hash_grid=new List*[num_hash];
    for (unsigned int i=0; i<num_hash; i++) hash_grid[i] = NULL;
    lst = hitpoints;
    while (lst != NULL)
    {
        HPoint *hp = lst->id;
        lst = lst->next;
        Vec3 BMin = ((hp->pos - irad) - hpbbox.min) * hash_s;
        Vec3 BMax = ((hp->pos + irad) - hpbbox.min) * hash_s;
        for (int iz = abs(int(BMin.z)); iz <= abs(int(BMax.z)); iz++)
        {
            for (int iy = abs(int(BMin.y)); iy <= abs(int(BMax.y)); iy++)
            {
                for (int ix = abs(int(BMin.x)); ix <= abs(int(BMax.x)); ix++)
                {
                    int hv=hash(ix,iy,iz);
                    hash_grid[hv]=ListAdd(hp,hash_grid[hv]);
                }
            }
        }
    }
}

void PhotonUtil::genp(Ray *pr, Color *f, int i, Vec3 position, Color flux)  {
    *f = flux * (PI * 4.0f); // flux
    double p = 2. * PI * hal(0, i);
    double t = 2. * acos(sqrt(1. - hal(1,i)));
    double st = sin(t);
    pr->d = Vec3(cos(p) * st, cos(t), sin(p) * st);
	pr->o = position;
//    pr->o = Vec3(50, 90, 80);
//    pr->o = Vec3(75, 50, 150);
}

List *PhotonUtil::ListAdd(HPoint *i, List *h) {
    List* p = new List;
    p->id = i;
    p->next = h;
    return p;
}