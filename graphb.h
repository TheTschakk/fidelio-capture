
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
    int fl=1, fs=1;
    int zero;

    img->num = -1;

    do {
        if ( (nl == l) && (ns == s) ) {
            zero = 0;
            for (j=0; j<blockL; j++) {
                if (img->adjL[l*blockL + j]) {
                    zero += 1;
                    break;
                }
            }

            for (j=0; j<blockS; j++) {
                if (img->adjS[s*blockS + j]) {
                    zero += 2;
                    break;
                }
            }

            switch(zero) {
                case 0 :
                    l++; s++; nl++; ns++; continue;
                case 1 :
                    s++; ns++; continue;
                case 2 :
                    l++; nl++; continue;
                case 3 :
                    img->num++;
                    img->met[img->num]->lght[img->met[img->num]->Nlght] = img->lght[l];
                    img->met[img->num]->vtc[img->met[img->num]->Nvtc] = img->lght[l];
                    img->met[img->num]->Nlght++;
                    img->met[img->num]->Nvtc++;
                    img->met[img->num]->shdw[img->met[img->num]->Nshdw] = img->shdw[s];
                    img->met[img->num]->vtc[img->met[img->num]->Nvtc] = img->shdw[s];
                    img->met[img->num]->Nshdw++;
                    img->met[img->num]->Nvtc++;
                    nl++; ns++;
            }
        }
        if (nl > l) {
            for (i=nl; i<(img->Nlght); i++) {
                for (j=0; j<blockL; j++) {
                    if ( img->adjL[l*blockL + j] & img->adjL[i*blockL + j] ) {
                        switchRow(img->adjL, nl, i, blockL);
                        switchEle(img->lght, nl, i);
                        img->met[img->num]->lght[img->met[img->num]->Nlght] = img->lght[nl];
                        img->met[img->num]->Nlght++;
                        img->met[img->num]->vtc[img->met[img->num]->Nvtc] = img->lght[nl];
                        img->met[img->num]->Nvtc++;
                        nl++;
                        break;
                    }
                }
            }
            l++;
        }
        else if (ns > s) {
            for (i=ns; i<(img->Nshdw); i++) {
                for (j=0; j<blockS; j++) {
                    if ( img->adjS[s*blockS + j] & img->adjS[i*blockS + j] ) {
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
            }
            s++;
        }
    } while ( (nl < img->Nlght) && (ns < img->Nshdw) );
    img->num++;
}
