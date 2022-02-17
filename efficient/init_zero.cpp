#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#define TIMEDIFF(s, e) (((e.tv_sec)-(s.tv_sec))*1000000 + (e.tv_usec) - (s.tv_usec))

int main()
{
    struct timeval s, e;
    char a[1024], b[1024*1024], c[1024*1024*4];

    gettimeofday(&s, NULL);
    bzero(a, sizeof(a));
    gettimeofday(&e, NULL);
    printf("bzero 1k: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    bzero(b, sizeof(b));
    gettimeofday(&e, NULL);
    printf("bzero 1m: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    bzero(c, sizeof(c));
    gettimeofday(&e, NULL);
    printf("bzero 4M: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    memset(a, 0, sizeof(a));
    gettimeofday(&e, NULL);
    printf("memset 1k: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    memset(b, 0, sizeof(b));
    gettimeofday(&e, NULL);
    printf("memset 1M: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    memset(c, 0, sizeof(c));
    gettimeofday(&e, NULL);
    printf("memset 4M: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    for(int i=0; i<sizeof(a); ++i)
        a[i]=0;
    gettimeofday(&e, NULL);
    printf("for 1k: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    for(int i=0; i<sizeof(b); ++i)
        b[i]=0;
    gettimeofday(&e, NULL);
    printf("for 1M: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    memset(c, 0, sizeof(c));
    gettimeofday(&e, NULL);
    printf("memset 4M: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    for(int i=0; i<sizeof(a); ++i)
        a[i]=0;
    gettimeofday(&e, NULL);
    printf("for 1k: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    for(int i=0; i<sizeof(b); ++i)
        b[i]=0;
    gettimeofday(&e, NULL);
    printf("for 1M: %d/n", TIMEDIFF(s, e));

    gettimeofday(&s, NULL);
    for(int i=0; i<sizeof(c); ++i)
        c[i]=0;
    gettimeofday(&e, NULL);
    printf("for 4M: %d/n", TIMEDIFF(s, e));
}
