void memcpy(void* dest, void* src, int size){
    char *destb = (char *) dest;
    char *srcb = (char *) src;

    for(int i = 0; i < size; i++){
        destb[i] = srcb[i];
    }
}

void *memset(void *ptr, int value, int num){
    unsigned char byte = (unsigned char)value;
    for (int i = 0; i < num; i++)
    {
        *((unsigned char*)ptr) = byte;
    }
}