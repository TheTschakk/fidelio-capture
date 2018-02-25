
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define HEIGHT 480
#define WIDTH 720
#define LENGTH HEIGHT*WIDTH

#define INT 32
#define MAXPIX 1024*32
#define SIZE MAXPIX/INT
#define MAXMET 10

static char *input;
char cam_id = '0';
const int buffer_size = 150;
static int upfluff = 25;
static int downfluff = 25;

static struct image *frm = NULL;

void switchEle(int *list, int item1, int item2);
void print1dArray(int *list, int dim);

#include "metb.h"
#include "analysisb.h"
#include "io.h"

// main loop works similar to actual 
int mainloop(void) {
	int n=0;
	int found=0;
	int lifetime;

	while (1) {
		printf("frame %i ################################################\n", frm->index);
		analyseFrame(frm);

                /*
		if ( endOfMeteor(frm, &lifetime, 3) != -1 )
		    found = lifetime;
                */

		if (found > 0)
		    n++;
		else
		    n = 0;

		if (n > upfluff) {
		    //write_video(frm, (downfluff + found + upfluff));
		    n = 0;
		    found = 0;
		    sleep(5);
		}

		//printImage(frm);

		frm = frm->next;

		if (frm->index == 150)
		    return 1;

	}
	return 0;
}


int main(int argc,char* argv[]){
    input = argv[1];
	frm = buildBuffer(buffer_size); // generate cyclicalc buffer of size "buffer_size" frames
	read_video(input); // invoke the load_file() function in order to fill generated buffer with frames from "input"

	mainloop(); // start the main loop
	
	if(frm->index == 150) {
	    freeBuffer(frm);
	}
	return 0;
}

