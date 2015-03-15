#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, const char *argv[])
{
    struct stu{
        char name[10];
        int score;
    };  

    struct stu student[2],*p;
    p = student;

    strcpy(student[0].name, "test");
    student[0].score = 10; 

    strcpy(student[1].name, "test1");
    student[1].score = 11; 

    printf("%s\n", p[0].name);
    printf("%s\n", p->name);                                                                                                                 

    printf("%s\n", p[1].name);
    printf("%s\n", (p+1)->name);

    return 0;
}
