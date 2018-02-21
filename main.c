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

const char *dev_name = "/dev/video0";
const int buffer_size = 150;
const int n_elapsed = 50;

struct buffer {
	void *start;
	size_t length;
};

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

struct buffer *buffers;
static unsigned int n_buffers;

static struct image *frm = NULL;

static int fd = -1;

static int xioctl(int fd, int request, void *arg) {
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

int init_device(int fd) {

	struct v4l2_capability caps;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps)){
		perror("Querying Capabilities");
		return 1;
	}

	if (!(caps.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("dev is no video capture device\n");
	}

	if (!(caps.capabilities & V4L2_CAP_STREAMING)) {
		printf("dev does not support streaming\n");
	}

	struct v4l2_format fmt;
	CLEAR(fmt);
	
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = WIDTH;
	fmt.fmt.pix.height = HEIGHT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	fmt.fmt.pix.bytesperline = 0;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
		perror("Setting Pixel Format");
		return 1;
	}
	
	return 0;
}


int init_mmap(int fd) {

	struct v4l2_requestbuffers req;
	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	
	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		perror("Requesting Buffer");
		return 1;
	}

	buffers = calloc(req.count, sizeof(*buffers));

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
	
		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
			perror("Querying Buffer 1");
			return 1;
		}

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
	}

	return 0;
}


int start_grabbing(void) {

	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;
		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
			perror("Queue Buffer");
			return 1;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
		perror("Stream ON");
		return 1;
	}
	
	return 0;
}

int process_frame(unsigned char *yuyv) {
	unsigned int i;

	for (i = 0; i < LENGTH; i++) {
		frm->data[i] = yuyv[2*i];
	}

	frm = frm->next;
}	

int read_frame(void) {

	struct v4l2_buffer buf;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
		case EAGAIN:
			printf("EAGAIN");
		case EIO:
			printf("EIO");
		default:
			printf("DQBUF");
		}
	}
	
	assert(buf.index < n_buffers);

	process_frame(buffers[buf.index].start);

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
		perror("Query Buffer 2");
		return 1;
	}
	return 1;
}

int write_video(void) {
	int i;
	FILE *outfd = fopen("positive.bwv", "a");

	for (i = 0; i < buffer_size; i++) {
		fwrite(frm->data, LENGTH, 1, outfd);
		frm = frm->next;
	}

	fclose(outfd);
	return 0;
}

int mainloop (time_t exectime) {
	int n=0;
	int found=0;

	time_t start = time(NULL);
	time_t end = start + exectime;
	
	while (time(NULL) < end) {
		while (1) {	
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			struct timeval tv = {0};
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			int r = select(fd+1, &fds, NULL, NULL, &tv);
		
			if (-1 == r) {
				perror("Waiting for Frame");
				return 1;
			}

			if (read_frame())
				break;
		}

		analyseFrame(frm);

		if (endOfMeteor(frm, 3) == 1)
			found = 1;

		if (found == 1)
			n++;

		if (n > n_elapsed) {
			n = 0;
			found = 0;
			write_video();
		}
	}
	return 0;
}

int stop_grabbing(void) {
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
		perror("Stop Capture");
		return 1;
	}
}

int uninit_device(void) {
	unsigned int i;

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap(buffers[i].start, buffers[i].length)) {
			perror("munmap");
			return 1;
		}

	free(buffers);
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
