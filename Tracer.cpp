//
// Created by Mike Huang on 2016/5/11.
//

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "Tracer.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define ALPHA ((double)0.7) // the alpha parameter of PPM

void Tracer::trace(const Ray &r, int dpt, bool m, const Color &fl, const Color &adj, int i) {
    int id;
    IntersectPoint inter = scene->intersect(r, id);

    dpt++;
    if(!inter.isHit() || (dpt >= 20))return;

    int d3 = dpt * 3;
    const GeometricObj* obj = scene->obj[id];
    Vec3 intersectPoint = r.o + r.d * inter.t;
    Vec3 n = inter.normal;
    Color f = obj->c + obj->getTextureColor(intersectPoint) + Color(.1f, .1f, .1f);
    Vec3 nl = n.dot(r.d) < 0 ? n : n * -1;
    double p = f.r > f.g && f.r > f.b ? f.r : f.g > f.b ? f.g : f.b;  // get the biggest color component in f

    if (obj->refl == DIFF) {
        // Lambertian

        // use QMC to sample the next direction
        double r1 = 2. * PI * photonUtil->hal(d3-1, i);
        double r2 = photonUtil->hal(d3 + 0, i);
        double r2s = sqrt(r2);
        Vec3 w = nl;
        Vec3 u = ((fabs(w.x) > .1 ? Vec3(0,1) : Vec3(1)) * w).getNormalize();
        Vec3 v = w*u;
        Vec3 d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).getNormalize();

        if (m) {
            // eye ray
            // store the measurment point
            HPoint* hp = new HPoint;
            hp->f = f.mult(adj);
            hp->pos = intersectPoint;
            hp->nrm = n;
            hp->pix = photonUtil->pixel_index;
            photonUtil->hitpoints = photonUtil->ListAdd(hp, photonUtil->hitpoints);
        }
        else
        {
            // photon ray
            // find neighboring measurement points and accumulate flux via progressive density estimation
            Vec3 hh = (intersectPoint - photonUtil->hpbbox.min) * photonUtil->hash_s;
            int ix = abs(int(hh.x)), iy = abs(int(hh.y)), iz = abs(int(hh.z));
            // strictly speaking, we should use #pragma omp critical here.
            // it usually works without an artifact due to the fact that photons are
            // rarely accumulated to the same measurement points at the same time (especially with QMC).
            // it is also significantly faster.
            {
                List* hp = photonUtil->hash_grid[photonUtil->hash(ix, iy, iz)];
                while (hp != NULL) {
                    HPoint *hitpoint = hp->id;
                    hp = hp->next;
                    Vec3 v = hitpoint->pos - intersectPoint;
                    // check normals to be closer than 90 degree (avoids some edge brightning)
                    if ((hitpoint->nrm.dot(n) > 1e-3) && (v.dot(v) <= hitpoint->r2)) {
                        // unlike N in the paper, hitpoint->n stores "N / ALPHA" to make it an integer value
                        double g = (hitpoint->n * ALPHA + ALPHA) / (hitpoint->n * ALPHA + 1.0);
                        hitpoint->r2 = hitpoint->r2 * g;
                        hitpoint->n++;
                        hitpoint->flux = (hitpoint->flux + hitpoint->f.mult(fl) * (1. / PI)) * g;
                    }
                }
            }
            if (photonUtil->hal(d3 + 1,i) < p) trace(Ray(intersectPoint, d), dpt, m, f.mult(fl) * (1. / p), adj, i);
        }

    } else if (obj->refl == SPEC) {
        // mirror
        trace(Ray(intersectPoint, r.d - n * 2.0 * n.dot(r.d)), dpt, m, f.mult(fl), f.mult(adj),i);

    } else {
        // glass
        Ray lr(intersectPoint,r.d - n * 2.0 * n.dot(r.d));
        bool into = (n.dot(nl) > 0.0);
        double nc = 1.0, nt = 1.2;
        double nnt = into ? nc / nt : nt / nc;
        double ct1 = nl.dot(r.d), cos2t = 1 - nnt * nnt * (1 - ct1 * ct1);

        // total internal reflection
        if (cos2t < 0) return trace(lr, dpt, m, fl, adj, i);

        Vec3 td = (r.d * nnt - n * ( (into ? 1 : -1) * (ct1 * nnt + sqrt(cos2t))) ).getNormalize();
        double a = nt - nc, b = nt + nc, c = 1 - (into ? -ct1 : td.dot(n));
        double R0 = a * a / (b * b);
        double Re = R0 + (1 - R0) * c * c * c * c * c, P = Re;
        Ray rr(intersectPoint, td);
        Color fa = f.mult(adj);
        if (m) {
            // eye ray (trace both rays)
            trace(lr, dpt, m, fl, fa * Re, i);
            trace(rr, dpt, m, fl, fa * (1.0 - Re), i);
        } else {
            // photon ray (pick one via Russian roulette)
            (photonUtil->hal(d3-1,i) < P) ? trace(lr, dpt, m, fl, fa, i)
                                           :trace(rr, dpt, m, fl, fa, i);
        }
    }
}

void Tracer::run() {

    // trace eye rays and store measurement points
    int w = scene->width, h = scene->height;
    Color *c = new Color[w * h];
    Color vw;

    for (int y = 0; y < h; y++){
        fprintf(stderr, "\rHitPointPass %5.2f%%", 100.0 * y / (h - 1));
        for (int x = 0; x < w; x++) {
            photonUtil->pixel_index = x + y * w;
            trace(scene->createRay(new Point2D(x, y)), 0, true, Color(), Color(1.f, 1.f, 1.f),0);
        }
    }
    fprintf(stderr,"\n");

    // build the hash table over the measurement points
    photonUtil->build_hash_grid(w,h);

    // trace photon rays with multi-threading
    photonUtil->num_photon = samples;
    vw = Color(1.f, 1.f, 1.f);
#pragma omp parallel for schedule(dynamic, 1)
    for(int i = 0; i < photonUtil->num_photon; i++) {
        double p = 100. * (i + 1) / photonUtil->num_photon;
        fprintf(stderr,"\rPhotonPass %5.2f%%",p);
        int m = 1000 * i;
        Ray r1, r2;
        Color f1, f2;
        for(int j = 0; j < 1000; j++){
			photonUtil->genp(&r1, &f1, m + j, Vec3(75, 50, 150), Color(2500.f, 2500.f, 2500.f));
			photonUtil->genp(&r2, &f2, m + j, Vec3(75, 140, 10), Color(2500.f, 2500.f, 2500.f) * 2);
			trace(r1, 0, false, f1, vw, m + j);
			trace(r2, 0, false, f2, vw, m + j);
        }
    }

    // density estimation
    List* lst = photonUtil->hitpoints;
    while (lst != NULL) {
        HPoint* hp = lst->id;
        lst = lst->next;
        int i = hp->pix;
        c[i] = c[i] + hp->flux * (1.0 / (PI * hp->r2 * photonUtil->num_photon * 1000.0));
    }

//    write_ppm("image.ppm", c);
    write_png("image.png", c, w, h);
	GeometricObj* obj = scene->obj[6];

//	write_png("test.png", obj->tmpColor, obj->textureSize.first, obj->textureSize.second);
//	write_png("origin.png", obj->texture, obj->textureSize.first, obj->textureSize.second);
}

void Tracer::write_ppm(const char *filename, Color *c){
    FILE* f = fopen(filename, "w");
    fprintf(f,"P3\n%d %d\n%d\n",w,h,255);
    for(int i = 0; i< w * h; i++) {
        fprintf(f,"%d %d %d ", c->toInt(c[i].r), c->toInt(c[i].g), c->toInt(c[i].b));
    }
}

void Tracer::write_png(const char *filename, Color *c, int w, int h) {
    unsigned char* pic = new unsigned char[w*h*3];
    int n = 0;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            pic[n++] = (unsigned char)c->toInt(c[i * w + j].r);
            pic[n++] = (unsigned char)c->toInt(c[i * w + j].g);
            pic[n++] = (unsigned char)c->toInt(c[i * w + j].b);
        }
    }
    stbi_write_png(filename, w, h, 3, pic, 3 * w);
    delete [] pic;
}
