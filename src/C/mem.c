void memcpy(void* dest, void* src, int size){
    char *destb = (char *) dest;
    char *srcb = (char *) src;

    for(int i = 0; i < size; i++){
        destb[i] = srcb[i];
    }
}