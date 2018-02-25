
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define HEIGHT 480
#define WIDTH 720
#define LENGTH 345600
int nlimit = 1024*32;

static char *input;
char cam_id = '0';
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

		printImage(frm);

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

