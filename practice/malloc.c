#include <stdio.h>
#include <stdlib.h>

/**
 *
 * 试验野指针
 *
 */
int main(int argc, const char *argv[])
{
    char * p = (char *)malloc(10*sizeof(char));

    free(p);

    p[100] = 'a';
    p--;

    p[0] = 'c';

    printf("p is %s\n", p);


    return 0;
}
