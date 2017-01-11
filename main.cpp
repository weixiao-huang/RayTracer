// expanded smallppm (code is exactly the same as smallppm.cpp but with more comments)

#include <stdlib.h> // originally smallpt, a path tracer by Kevin Beason, 2008
#include "Tracer.h"


int main(int argc, char *argv[]) {
	// samps * 1000 photon paths will be traced
	int w = 1920, h = 1080, samps = (argc==2) ? MAX(atoi(argv[1])/1000, 1) : 1000;
    Tracer* tracer = new Tracer(samps, w, h);
    tracer->run();
}
