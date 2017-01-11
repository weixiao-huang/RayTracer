//
// Created by Mike Huang on 2016/5/11.
//

#ifndef SMALLPPM_TRACER_H
#define SMALLPPM_TRACER_H

#include "PhotonUtil.h"
#include "Scene.h"

class Tracer {
    PhotonUtil* photonUtil;
    Scene* scene;

    void write_ppm(const char* filename, Color* c); // save the image after tone mapping and gamma correction
    void write_png(const char* filename, Color* c, int w, int h);

public:
    const int w, h, samples;
    Tracer(int samples = 1000000, int w = 512, int h = 512)
            : samples(samples), w(w), h(h) {
        photonUtil = new PhotonUtil();
        scene = new Scene(1024, 768);
    }
    ~Tracer() { delete photonUtil; };

    void trace(const Ray &r,int dpt,bool m,const Color &fl,const Color &adj,int i);
    void run();

};


#endif //SMALLPPM_TRACER_H
