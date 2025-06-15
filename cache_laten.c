#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <immintrin.h> // for clflush and lfence

#define ONE p = (char **)*p;
#define FIVE ONE ONE ONE ONE ONE
#define TEN FIVE FIVE
#define FIFTY   TEN TEN TEN TEN TEN
#define HUNDRED FIFTY FIFTY

int main(int argc, char* argv[])
{
  unsigned long i, j, size, tmp;
    unsigned long memsize = 8388608; //18 MiB
    unsigned long count = 1048576; /* memsize / 64 * 16 */
    unsigned int stride = 64; /* skipped amount of memory before the next access */
    unsigned long sec, usec;
    struct timeval tv1, tv2;
    //struct timezone tz;
    unsigned int *indices;

    while (argc-- > 0) {
        if ((*argv)[0] == '-') {  /* look at first char of next */
            switch ((*argv)[1]) {   /* look at second */
                case 'b':
                    argv++;
                    argc--;
                    memsize = atoi(*argv) * 1024;
                    break;

                case 's':
                    argv++;
                    argc--;
                    stride = atoi(*argv);
                  break;
            }
        }
        argv++;
    }


  char* mem = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    size = memsize / stride;
    indices = malloc(size * sizeof(int));

    for (i = 0; i < size; i++)
        indices[i] = i;
    
    for (i = 0; i < size; i++) {
    j = i + rand() % (size - i);
    if (i != j) {
        tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

    for (i = 0; i < size - 1; i++)
        *(char **)&mem[indices[i]*stride]= (char*)&mem[indices[i+1]*stride];
    *(char **)&mem[indices[size-1]*stride]= (char*)&mem[indices[0]*stride];

    // CLFLUSH: Clear cache lines before measurement
    for (i = 0; i < memsize; i += 64) {
        _mm_clflush(mem + i);
    }
    _mm_mfence(); // Ensure clflush completes

    register char **p = (char **) mem;
    tmp = count / 100;

    // LFENCE: Memory barrier before timing
    _mm_lfence();

    gettimeofday (&tv1, NULL);
    for (i = 0; i < tmp; ++i) {
        HUNDRED;  //trick 1
    }
    gettimeofday (&tv2, NULL);
    char **touch = p;

    if (tv2.tv_usec < tv1.tv_usec) {
        usec = 1000000 + tv2.tv_usec - tv1.tv_usec;
        sec = tv2.tv_sec - tv1.tv_sec - 1;
    } else {
        usec = tv2.tv_usec - tv1.tv_usec;
        sec = tv2.tv_sec - tv1.tv_sec;
    }
    printf("%d", count);
    FILE* f = fopen("result.txt","a");

    fprintf( f ,  "%ld,%d.%06d,%.2f\n", 
            memsize/1024,sec,usec, (sec * 1000000  + usec) * 1000.0 / (tmp *100));

    fclose(f);
    munmap(mem, memsize);
    free(indices);
}