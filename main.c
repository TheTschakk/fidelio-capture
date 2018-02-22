#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define HEIGHT 480
#define WIDTH 720
#define LENGTH 345600

char *dev_name = "/dev/video1";
char cam_id = '0';
const int buffer_size = 150;
static int upfluff = 25;
static int downfluff = 25;

static int fd = -1;
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
int *cpy1dArray(int *list, int *mist, int dim);
int **cpy2dArray(int **arr, int **brr, int rows, int cols);
int *cat1dArrays(int *list1, int *list2, int dim1, int dim2);
int sum1dArray(int *list, int dim);

#include "met.h"
#include "analysis.h"
#include "v4l2.h"
#include "io.h"

int mainloop (time_t exectime) {
    int n=0;
    int found=0;
    int lifetime;

    time_t start = time(NULL);
    time_t end = start + exectime;

    while (time(NULL) < end) {
        printf("frame %i ################################################\n", frm->index);
        wait_for_frame();
        clock_gettime(CLOCK_REALTIME, &(frm->time));

        if (found == 0)
            analyseFrame(frm);

        if ( endOfMeteor(frm, &lifetime, 3) != -1 )
            found = lifetime;

        if (found > 0)
            n++;
        else
            n = 0;

        if (n > upfluff) {
            write_video(frm, (downfluff + found + upfluff));
            n = 0;
            found = 0;
        }

        printImage(frm);

        frm = frm->next;
    }
    return 0;
}


int main(int argc, char* argv[]) {
    int time_int;
    time_t time;

    sscanf(argv[1], "%i", &time_int);
    time = time_int;

    frm = buildBuffer(buffer_size);

    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);

    if (fd == -1) {
        perror("Opening video device");
        return 1;
    }

    if(init_device(fd))
        return 1;

    if(init_mmap(fd))
        return 1;

    if (start_grabbing())
        return 1;

    mainloop(time);

    stop_grabbing();

    uninit_device();

    close(fd);

    freeBuffer(frm);

    return 0;
}
