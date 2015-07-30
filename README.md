linux_c
=======

linux c study

```conf
越抽象的东西, 把它变具象就好理解了

char p          //点,         一页纸上的一个字
char *p        //线,         一行字
char **p       //面,         一页字   *(*(p+1)+2) //第二行第三列
char ***p      //空间,     一本书

二维数组

char p[2][3];  //2行3列的数组

char (*q)[3];
//q是一个"行指针",并且每行只有3个字符
//q[0]第一行首地址
//q[1]第二行首地址
//q[2]第三行首地址

char p[2][3]= {"123", "456"};
char (*q)[3]
q = p;

printf("%c\n", p[1][2]); //6
printf("%c\n", *(*(q+1)+2)); //6

```

```conf

段错误总结
char *str = "hello"; //str指针,指向了常量区（所以所指向的东西不能改变）
strcpy(str,"world"); //段错误 ,strcpy源码是将str指向的内存中的字符一个一个改变,但是str指向的是常量,是不能改变的,所以报段错误

char* ptr = "abc";
   因为定义的是一个普通指针，并没有定义空间来存放"abc"，所以编译器得帮我们找地方来放"abc"，显然，把这里的"abc"当成常量并把它放到程序 的常量区是编译器 最合适的选择。所以尽管ptr的类型不是const char*，并且ptr[0] = 'x';也能编译 通过，但是执行ptr[0] = 'x';就会发生运行时异常，因为这个语句试图去修改程序 常量区中的东西
   
```



```c
//运算符优先级示例
#include <stdio.h>

int main() {

 int i[] = {100, 200, 300, 400, 500};
 int *p = i;

 printf("...%d\n", *p++);
 printf("*p = %d\n", *p);
 printf("...%d\n", *++p);
 printf("*p = %d\n", *p);
 printf("...%d\n", ++*p);
 printf("*p = %d\n", *p);

 return 0;
}
```

 
```conf
测试结果：

...100
*p = 200
...300
*p = 300
...301
*p = 301

分析如下：

1.

int *p = i;

此时，指针执行数组第一个元素。

2.

printf("...%d\n", *p++);

此时，由于*和++的运算符优先级相同，而且结合性是由右至左，因此*p++其实就是：*(p++)。由于此处的++是后继加，所以先得到*p为100之后，才会处理++的动作。所以这个语句先打印出100，然后将p指向下一地址。

3.

printf("...%d\n", *++p);

同理，这个语句相当于： *(++p)。由于此处的++是前置加，所以p指针先移到了下一地址，再打印出*p的值300。

4.

printf("...%d\n", ++*p);

这个语句相当于： ++(*p)。所以++是对*p的值加1。此语句等同于：

*p = *p + 1;

将*p （此时为300）加1，再放入*p中。

最后，*p的运算结果为301。


*p++按照从右往左结合是*(p++),而p++运算本身是先返回p用于外层计算，然后再执行p++的。所以整个运算过程看上去是*p ; p++，容易令人以为结合顺序是从左往右的
```

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void change(char * p){ 
    char s[] = "test";
    p = s;
}

int main(int argc, const char *argv[])
{
    char p[] = "123";                                                                                  
    change(p);
    printf("%s\n", p); 
    return 0;
}
```
![c_func_params_pass](https://raw.githubusercontent.com/tetang1230/linux_c/master/pic/func_params_pass.jpg)

```conf
上面的程序用图说明一切,函数的参数都是值传递。
```
