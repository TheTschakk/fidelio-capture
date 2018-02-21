
void timeStamp(time_t t0) {
    time_t t;
    t = time(NULL);
    clock_t c; 
    c = clock();
    printf("%f sec (%ld, %ld)\n", difftime(t, t0), t, c);
}
