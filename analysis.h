
#include "test.h"

static int limit = 50;
static int cutoff = 10;
static int cdepth = 3;
static int nlimit = 100;

int left = 10;
int right = 710;
int top = 10;
int bottom = 470;

#include "pixel.h"
#include "array.h"
#include "graph.h"

int analyseMeteors(struct image *img) {
    int i;
    
    for (i=0; i<(img->num); i++) {
	getPosition(img->met[i]);
	assignContinuity(img, img->met[i], 20, cdepth);
	getVelocity(img->met[i]);
    }

    return 0;
}

int analyseFrame(struct image *img) {
    clock_t t0 = clock();

    initFrame(img);

    identifyPix(img, limit); // build lists of bright (>0) and dark (<0) pixels from sub
    	printf("identifyPix "); printf("clock %ld\n", clock() - t0);

    if ( (img->Nlght + img->Nshdw) > nlimit) {
        printf("skipping frame because number of pixels exceeds maximal allowed (%i): %i\n", nlimit, img->Nlght + img->Nshdw);
        return 1;
    }

    if ( buildAdj(img, cutoff) == 1 ) return 1; // build adjacency matrix between bright (>0) and dark (<0) pixels
    	printf("buildAdj "); printf("clock %ld\n", clock() - t0);

    cluster(img); // sort vv-matrix to VV-matrix
    	printf("cluster "); printf("clock %ld\n", clock() - t0);

    analyseMeteors(img);
    	printf("analyseMeteors "); printf("clock %ld\n", clock() - t0);

    return 0;
}

