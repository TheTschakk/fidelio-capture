
void setBit(unsigned int *A, int row, int col, int block) {
    int n = row*block + col/INT;
    int k = col%INT;
    unsigned int flag = 1;
    flag = flag << k;
    A[n] = A[n] | flag;
}

int getBit(unsigned int *A, int k) {
    int i = k/INT;
    int pos = k%INT;

    unsigned int flag = 1;
    flag = flag << pos;

    if (A[i] & flag) return 1;
    
    return 0;
}

void printBits(unsigned int *A, int start, int length) {
    int i, j;

    for (i=0; i<length; i++) {
        printf("%i ", getBit(A, start+i));
    }
    printf("\n");
}

int compareRows(unsigned *arr, int rows, int cols) {
    
}
