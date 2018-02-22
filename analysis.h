
#include "test.h"

static int limit = 50;
static int cutoff = 10;
static int cdepth = 3;
static int nlimit = 1000;

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
    struct timespec systime, reftime;
    clock_gettime(CLOCK_REALTIME, &reftime);

    initFrame(img);

    identifyPix(img, limit); // build lists of bright (>0) and dark (<0) pixels from sub

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("identifyPix %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    if ( (img->Nlght + img->Nshdw) > nlimit) {
        printf("skipping frame because number of pixels exceeds maximal allowed (%i): %i\n", nlimit, img->Nlght + img->Nshdw);
        return 1;
    }

    if ( buildAdj(img, cutoff) == 1 ) return 1; // build adjacency matrix between bright (>0) and dark (<0) pixels

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("buildAdj %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    cluster(img); // sort vv-matrix to VV-matrix

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("cluster %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    analyseMeteors(img);

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("analyseMeteor %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    return 0;
}

