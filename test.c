
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define HEIGHT 480
#define WIDTH 720
#define LENGTH 345600

static char *input;
const int buffer_size = 150;
static int upfluff = 25;
static int downfluff = 25;

static struct image *frm = NULL;

int *initList(int *list);
int *addToList(int *list, int item, int size);
int *rmFromList(int *list, int item, int cols);
void switchEle(int *list, int item1, int item2);
int **alloc2dArray(int **arr, int rows, int cols);
int **free2dArray(int **arr, int rows);
int **rmRow(int **arr, int row0, int *rows);
int **rmCol(int **arr, int col0, int rows, int *cols);
void switchRows(int **arr, int row1, int row2, int cols);
void switchCols(int **arr, int col1, int col2, int rows);
void print1dArray(int *list, int dim);
void print2dArray(int **arr, int rows, int cols);

#include "met.h"
#include "analysis.h"

// write video file to frm-buffer
int loadFile(void) {
	FILE *fp;
	int i;

	fp = fopen(input, "rb");
	
	// write each frame of input to data array of cyclical buffer, one at a time ("buffer_size" times)
	for (i = 0; i < buffer_size; i++) {
		fread(frm->data, LENGTH, 1, fp); // write to current frame
		frm = frm->next; // jump to next frame in "frm"
	}

	fclose(fp); // close input
	return 0;
}

// write video to file 
int writeVideo(struct image *last, int nfrms) {
	int i;
	FILE *outfd = fopen("positiv.bwv", "a"); // open file in append mode
	printf("%i \n", nfrms);

	// go back nfrms frames from last frame to first frame
	for (i=0; i<nfrms; i++) {
	    last = last->prev;
	}

	// append frames to file
	for (i=0; i<nfrms; i++) {
		fwrite(last->data, LENGTH, 1, outfd); // append current frame
		last = last->next; // jump to next frame
		printf("%i \n", i);
	}

	fclose(outfd); // close file
	return 0;
}

// main loop works similar to actual 
int mainloop(void) {
	int n=0;
	int found=0;
	int lifetime;

	while (1) {
		analyseFrame(frm);

		if ( endOfMeteor(frm, &lifetime, 3) != -1 )
		    found = lifetime;

		printf("found %i\n", found);
		if (found > 0)
		    n++;
		else
		    n = 0;

		if (n > upfluff) {
		    writeVideo(frm, (downfluff + found + upfluff));
		    n = 0;
		    found = 0;
		    return 1;
		}

		printf("frame %i ################################################\n", frm->index);
		printImage(frm);

		frm = frm->next;

		/*
		if (frm->index == 150)
		    return 1;
		*/

	}
	return 0;
}


int main(int argc,char* argv[]){
    input = argv[1];
	frm = buildBuffer(buffer_size); // generate cyclicalc buffer of size "buffer_size" frames
	loadFile(); // invoke the load_file() function in order to fill generated buffer with frames from "input"

	mainloop(); // start the main loop
	
	if(frm->index == 150) {
	    freeBuffer(frm);
	}
	return 0;
}

