
void buildAdj(struct image *img, int dist) {
    int i,j;

    for (i=0; i<(img->Nlght); i++) {
        for (j=0; j<(img->Nshdw); j++) {
            if (squareDist(img->lght[i], img->shdw[j]) < dist*dist) {
                setBit(img->adjL, i, j, blockL);
                setBit(img->adjS, j, i, blockS);
            }
        }
    }
}

void cluster(struct image *img) {
    int i,j;
    int l=0, s=0;
    int nl=0, ns=0;
    int zero;
    int zeroL, zeroS;

    img->num = -1;

    do {
        if ( (nl == l) && (ns == s) ) {
            for (i=l; i<(img->Nlght); i++) {
                for (j=0; j<blockL; j++) {
                    if ( zeroL = img->adjL[i*blockL + j] ) {
                        break;
                    }
                }
                if (zeroL) {
                    break;
                } else {
                    nl++; l++; 
                }
            }
            img->met[img->num+1]->lght[0] = img->lght[i];
            img->met[img->num+1]->Nlght++;
            img->met[img->num+1]->vtc[img->met[img->num+1]->Nvtc] = img->lght[i];
            img->met[img->num+1]->Nvtc++;

            for (i=s; i<(img->Nshdw); i++) {
                for (j=0; j<blockS; j++) {
                    if ( zeroS = img->adjS[i*blockS + j] ) {
                        break;
                    }
                }
                if (zeroS) {
                    break;
                } else {
                    ns++; s++; 
                }
            }
            img->met[img->num+1]->shdw[0] = img->shdw[i];
            img->met[img->num+1]->Nshdw++;
            img->met[img->num+1]->vtc[img->met[img->num+1]->Nvtc] = img->shdw[i];
            img->met[img->num+1]->Nvtc++;

            nl++; ns++;
            if ( zeroL && zeroS ) img->num++;
        }
        if (nl > l) {
            for (i=nl; i<(img->Nlght); i++) {
                zero = 0;
                for (j=0; j<blockL; j++) {
                    if ( !(img->adjL[i*blockL + j]) ) zero = 1;
                    if (img->adjL[l*blockL + j] & img->adjL[i*blockL + j]) { // ... & zero
                        switchRow(img->adjL, nl, i, blockL); // if nl = i
                        switchEle(img->lght, nl, i);
                        img->met[img->num]->lght[img->met[img->num]->Nlght] = img->lght[nl];
                        img->met[img->num]->Nlght++;
                        img->met[img->num]->vtc[img->met[img->num]->Nvtc] = img->lght[nl];
                        img->met[img->num]->Nvtc++;
                        nl++;
                        break;
                    }
                }
                if (zero) {
                    switchRow(img->adjL, nl, i, blockL);
                    switchEle(img->lght, nl, i);
                    nl++;
                }
            }
            l++;
        }
        if (ns > s) {
            for (i=ns; i<(img->Nshdw); i++) {
                zero = 0;
                for (j=0; j<blockS; j++) {
                    if ( !(img->adjS[i*blockS + j]) ) zero = 1;
                    if (img->adjS[s*blockS + j] & img->adjS[i*blockS + j]) { // ... & zero
                        switchRow(img->adjS, ns, i, blockS);
                        switchEle(img->shdw, ns, i);
                        img->met[img->num]->shdw[img->met[img->num]->Nshdw] = img->shdw[ns];
                        img->met[img->num]->Nshdw++;
                        img->met[img->num]->vtc[img->met[img->num]->Nvtc] = img->shdw[ns];
                        img->met[img->num]->Nvtc++;
                        ns++;
                        break;
                    }
                }
                if (zero) {
                    switchRow(img->adjS, ns, i, blockS);
                    switchEle(img->shdw, ns, i);
                    ns++;
                }
            }
            s++;
        }
    } while ( (nl < img->Nlght) && (ns < img->Nshdw) );
    img->num++;
}
