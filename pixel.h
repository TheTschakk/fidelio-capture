
int getX(int index) {
    return index % WIDTH;
}

int getY(int index) {
    return index / WIDTH;
}

int inFrame(int index) {
    int x = getX(index);
    int y = getY(index);

    if ( (x > left) && (x < right) && (y > top) && (y < bottom) )
	return 1;
    return 0;
}

int identifyPix(struct image *img, int lim) {
    int i;
    int diff;

    for (i=0; i<LENGTH; i++) {
	diff = img->data[i] - img->prev->data[i];

	if ( (diff > lim) && inFrame(i) ) {
	    img->lght = addToList(img->lght, i, img->Nlght);
	    img->Nlght++;
	}
	else if ( (diff < -lim) && inFrame(i) ) {
	    img->shdw = addToList(img->shdw, i, img->Nshdw);
	    img->Nshdw++;
	}
    }
    return (img->Nlght + img->Nshdw);
}

int squareDist(int i1, int i2) {
    return (getX(i1)-getX(i2)) * (getX(i1)-getX(i2)) + (getY(i1)-getY(i2)) * (getY(i1)-getY(i2));
}

void getPosition(struct meteor *met) {
    int i;
    float meanX=0, meanY=0;
    float meanX2=0, meanY2=0;

    int N = met->Nvtc;
    
    for (i=0; i<N; i++) {
	meanX += (float) getX(met->vtc[i]);
	meanX2 += (float) getX(met->vtc[i]) * getX(met->vtc[i]);
	meanY += (float) getY(met->vtc[i]);
	meanY2 += (float) getY(met->vtc[i]) * getY(met->vtc[i]);
    }
    met->posX = meanX/N;
    met->posY = meanY/N;
    met->posVar = (meanX2/N - meanX*meanX/(N*N)) * (meanY2/N - meanY*meanY/(N*N));
}
