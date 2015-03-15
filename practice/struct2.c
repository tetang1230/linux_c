#include <stdio.h>
 
struct student
{
    int num;
    char name[20];
    char sex;
    int age;
 
};
/*指向结构体类型的数组的初始化*/
 struct student stu[3]={{10101,"Li Lin",'M',18},{10102,"Zhang Fun",'M',19},
     {10104,"Wang Min",'F',20}};
 void main()
 {
     struct student *p;/*指向结构体类型的指针变量*/
     printf("NO  Name  Sex  Age\n");
     for(p=stu;p<stu+3;p++)/*循环地址*/
     {
/*         printf("%5d%-20s %2c %4d\n",p->num,p->name,p->sex,p->age); 等价于*/
   printf("%5d%-20s %2c %4d\n",(*p).num,(*p).name,(*p).sex,(*p).age);
     }
     system("pause");
 }
