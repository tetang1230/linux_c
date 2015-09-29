#include <stdio.h>
#include <stdlib.h>

struct node_t{
    int a;
    char b;
    char c;
};


int main(int argc, const char *argv[])
{
    struct node_t node;
    unsigned long offset;

    offset = (unsigned long)&(((struct node_t *)0)->c);

    printf("offset is %lu\n", offset);
    
    return 0;
}
