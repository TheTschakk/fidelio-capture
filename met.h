
#ifdef math_h
#include <math.h>
#endif

struct image {
    int index;
    struct timespec time;
    unsigned char *data;
    int Nlght;
    int Nshdw;
    int *lght;
    int *shdw;
    int **adj;
    int num;
    struct meteor **met;
    struct image *prev;
    struct image *next;
};

struct meteor {
    int Nlght;
    int Nshdw;
    int Nvtc;
    int *lght;
    int *shdw;
    int *vtc;
    float posX;
    float posY;
    float posVar;
    float vx;
    float vy;
    float v2;
    float R;
    float dir;
    int continuity;
    int duration;
    struct meteor *prev;
    struct meteor *next;
};

struct image *buildBuffer(int size){

	int i;

	struct image *start = malloc(sizeof(struct image));
	struct image *tmp = NULL;
	
	struct image *img = start;

	for (i = 1; i <= size; i++) {

		img->prev = tmp;
		img->index = i;
		img->Nlght = 0;
		img->Nshdw = 0;
		img->lght = NULL;
		img->shdw = NULL;
		img->adj = NULL;
		img->num = 0;
		img->met = NULL;
		img->data = calloc(LENGTH, sizeof(unsigned char));

		if(i == size){
			img->next = start;
			start->prev = img;
			tmp = NULL;
			break;
		}

		img->next = malloc(sizeof(struct image));
		tmp = img;
		img = img->next;
	}
	return start;
}

void initMeteor(struct meteor *met) {
    free(met->lght);
    free(met->shdw);
    free(met->vtc);
}

void freeBuffer(struct image *img) {

    int i;
    struct image *tmp = img;
    img->prev->next = NULL;

    do {
	free(img->data);
	free(img->lght);
	free(img->shdw);
	if (img->adj != NULL) img->adj = free2dArray(img->adj, img->Nlght);

	for (i=0; i<(img->num); i++) {
	    initMeteor(img->met[i]);
	    free(img->met[i]);
	}

	free(img->met);

	tmp = img->next;
	free(img);
	img = tmp;

    } while (img != NULL);
}

void addMeteor(struct image *img) {
    int N = img->num;
    img->met = realloc(img->met, (N+1) * sizeof(struct meteor *));
    img->met[N] = malloc(sizeof(struct meteor));
    img->met[N]->lght = malloc(sizeof(int));
    img->met[N]->shdw = malloc(sizeof(int));
    img->met[N]->vtc = NULL;
    img->met[N]->Nlght = 1;
    img->met[N]->Nshdw = 1;
    img->met[N]->Nvtc = 2;
    img->met[N]->prev = NULL;
    img->met[N]->next = NULL;
    img->met[N]->continuity = 0;
    img->met[N]->duration = 0;
}
    
void initFrame(struct image *img) {
    int i;

    free(img->lght);
    img->lght = NULL;
    free(img->shdw);
    img->shdw = NULL;
    
    if (img->adj != NULL) img->adj = free2dArray(img->adj, img->Nlght);

    for (i=0; i<(img->num); i++) {
	initMeteor(img->met[i]);
        free(img->met[i]);
    }

    free(img->met);
    img->met = NULL;

    img->Nlght = 0;
    img->Nshdw = 0;
    img->num = 0;
}

void assignContinuity(struct image *img, struct meteor *met, int dist, int depth) {
    int deg, j;

    met->continuity = 0;
    
    for (deg=1; deg<=depth; deg++) {
	img = img->prev;
	for (j=0; j<(img->num); j++) {
	    if ( ((met->posX - img->met[j]->posX) * (met->posX - img->met[j]->posX) +
		  (met->posY - img->met[j]->posY) * (met->posY - img->met[j]->posY)) < (deg*deg * dist*dist) ) {
		met->prev = img->met[j];
		if (img->met[j]->next == NULL) img->met[j]->next = met;
		met->continuity = deg;
		break;
	    }
	}
	if (met->continuity > 0) {
	    met->duration = img->met[j]->duration + met->continuity;
	    break;
	}
    }
}

void getVelocity(struct meteor *met0) {
    float vx, vy;
    float Rx, Ry;

    float tsum=0, t2sum=0;
    float xsum=0, x2sum=0;
    float ysum=0, y2sum=0;
    float xtsum=0, ytsum=0;

    int n=0, t=0;

    struct meteor *met = met0;

    while (met->prev != NULL) {
	tsum += t;
	t2sum += t*t;

	xsum += met->posX;
	x2sum += met->posX * met->posX;

	ysum += met->posY;
	y2sum += met->posY * met->posY;

	xtsum += met->posX * t;
	ytsum += met->posY * t;

	t -= met->continuity;
	n++;
	met = met->prev;
    }

    vx = (n*xtsum - tsum*xsum) / (n*t2sum - tsum*tsum);
    vy = (n*ytsum - tsum*ysum) / (n*t2sum - tsum*tsum);
    Rx = ((n*xtsum-xsum*tsum)*(n*xtsum-xsum*tsum))/((n*x2sum-xsum*xsum)*(n*t2sum-tsum*tsum));
    Ry = ((n*ytsum-ysum*tsum)*(n*ytsum-ysum*tsum))/((n*y2sum-ysum*ysum)*(n*t2sum-tsum*tsum));

    met0->vx = vx;
    met0->vy = vy;
    met0->v2 = vx*vx + vy*vy;
    met0->R = Rx*Ry;

    met0->dir = atan2(vx, vy);
}

int endOfMeteor(struct image *img, int *dur, int depth) {
    int i;
    int num=-1;
    *dur = 0;

    struct image *ref = img;

    for (i=0; i<depth; i++) {
	ref = ref->prev;
    }

    for (i=0; i<(ref->num); i++) {
       if ( (ref->met[i]->next == NULL) && (ref->met[i]->prev != NULL) && (ref->met[i]->duration > *dur) ) {
	   num = i;
	   *dur = ref->met[num]->duration;
       }
    }

    return num;
}
    
int backTraceMeteor(struct meteor *met0) {
    int i=0;
    struct meteor *met;
    met = met0;

    printf("input\t\tX\t\tY\t\txyVar\t\tvelo2\t\tdir\t\tDensity\t\tmDegree\tmWeight\tnum\tduration \n");

    while (met != NULL) {
	//printf("%s\t", input);
	printf("%f\t", met->posX);
	printf("%f\t", met->posY);
	printf("%f\t", met->v2);
	printf("%f\t", met->dir);
	printf("%i\t", met->Nvtc);
	printf("%i\t", met->duration);
	printf("\n");
	met = met->prev;

	i++;
    }
    printf("\n");
    return i;
}

void printImage(struct image *img) {
    int i;
    /*
    printf("\n");
    print2dArray(img->adj, img->Nlght, img->Nshdw);
    printf("\n");
    */

    for (i=0; i<(img->num); i++) {
	printf("meteor =%i= || postion: X = %.2f, Y = %.2f | velocity: vx = %.3f, vy = %.3f, v2 = %.2f (R=%.4f) | continuity = %i | duration = %i\n", i, img->met[i]->posX, img->met[i]->posY, img->met[i]->vx, img->met[i]->vy, img->met[i]->v2, img->met[i]->R, img->met[i]->continuity, img->met[i]->duration);
	/*
	printf("LIGHT: ");
	print1dArray(img->met[i]->lght, img->met[i]->Nlght);
	printf("SHADOW: ");
	print1dArray(img->met[i]->shdw, img->met[i]->Nshdw);
	printf("\n");
	print1dArray(img->met[i]->x, img->met[i]->Nvtc);
	print1dArray(img->met[i]->y, img->met[i]->Nvtc);
	*/
	printf("\n");
    }
}
	
