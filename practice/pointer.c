#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{

    char p[2][3]= {"123", "456"};
    char (*q)[3]; //行指针, 一行3个字符
    q = p;

    printf("%c\n", p[1][2]); //6
    printf("%c\n", *(*(q+1)+2)); //6

    char *p1[2] = {"ab", "cdef"};
    char **q1;
    q1 = p1;
    
    printf("%s\n", p1[0]);
    printf("%s\n", p1[1]);
    
    printf("%s\n", q1[0]);
    printf("%s\n", q1[1]);
    
    printf("%c\n", q1[0][1]);
    return 0;
}
