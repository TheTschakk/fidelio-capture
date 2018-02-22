
int read_video(char *input) {
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

int write_video(struct image *last, int nfrms) {
	int i;
	char name[50];
    char timestamp[20];
    struct timespec systime;

    clock_gettime(CLOCK_REALTIME, &systime);
    strftime(timestamp, 100, "%Y%m%d_%H%M%S", gmtime(&(systime.tv_sec)));
    sprintf(name, "video%c_%s_%i",cam_id, timestamp, systime.tv_nsec / 1000000);
	
	FILE *outfd = fopen(name, "a"); // open file in append mode

	// go back nfrms frames from last frame to first frame
	for (i=0; i<nfrms; i++) {
	    last = last->prev;
	}

	// append frames to file
	for (i=0; i<nfrms; i++) {
		fwrite(last->data, LENGTH, 1, outfd); // append current frame
		last = last->next; // jump to next frame
	}

	fclose(outfd); // close file
	return 0;
}

