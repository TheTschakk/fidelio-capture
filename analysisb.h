
//#define INT 8*(int)sizeof(unsigned int)
int blockL;
int blockS;

static int limit = 50;
static int cutoff = 10;
static int cdepth = 3;

int left = 10;
int right = 710;
int top = 10;
int bottom = 470;

#include "bits.h"
#include "pixelb.h"
#include "arrayb.h"
#include "graphb.h"

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
    
    blockL = img->Nlght / INT + (img->Nlght % INT != 0);
    blockS = img->Nshdw / INT + (img->Nshdw % INT != 0);

    //printf("nl%i ns%i\n", img->Nlght, img->Nshdw);
    //printf("bl%i bs%i\n", blockL, blockS);

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("identifyPix %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    if ( (img->Nlght + img->Nshdw) > MAXPIX) {
        printf("exceeding MAXPIX (%i): %i\n", MAXPIX, img->Nlght + img->Nshdw);
        return 1;
    } else if ( !(img->Nlght && img->Nshdw) ) {
        printf("Nlght or Nshdw == 0\n");
        return 1;
    }

    buildAdj(img, cutoff);

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("buildAdj %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    cluster(img);

    //print1dArray(img->met[0]->lght, img->met[0]->Nlght);
    //print1dArray(img->met[0]->shdw, img->met[0]->Nshdw);
    //print1dArray(img->met[0]->vtc, img->met[0]->Nvtc);

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("cluster %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    analyseMeteors(img);

    clock_gettime(CLOCK_REALTIME, &systime);
    printf("analyseMeteor %f sec\n", (float) ((systime.tv_nsec - reftime.tv_nsec)/1000) / 1000000);

    return 0;
}
