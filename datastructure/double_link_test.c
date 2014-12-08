#include <stdio.h>
#include "double_link.h"
#include "double_link.c"
/**
 * C 语言实现的双向链表的测试程序。
 *
 * 从网上找的一个双向链表,运行了下,没问题
 * 结构也比较清晰,学习下
 * 其中比较秒的地方就是将节点中数据域设置
 * 成了void * p 类型, 导致了所有类型通用
 * 返回的数据只要用各种类型强制转换下就OK
 * 了,本例测试中,分别用了int,string,struct
 * 就是因为void * p的缘故。
 *
 * (01) int_test()
 *      演示向双向链表操作“int数据”。
 * (02) string_test()
 *      演示向双向链表操作“字符串数据”。
 * (03) object_test()
 *      演示向双向链表操作“对象”。
 *
 * @author skywang
 * @date 2013/11/07
 */

// 双向链表操作int数据
void int_test()
{
    int iarr[4] = {10, 20, 30, 40};

    printf("\n----%s----\n", __func__);
    create_dlink();        // 创建双向链表

    dlink_insert(0, &iarr[0]);    // 向双向链表的表头插入数据
    dlink_insert(0, &iarr[1]);    // 向双向链表的表头插入数据
    dlink_insert(0, &iarr[2]);    // 向双向链表的表头插入数据

    printf("dlink_is_empty()=%d\n", dlink_is_empty());    // 双向链表是否为空
    printf("dlink_size()=%d\n", dlink_size());            // 双向链表的大小

    // 打印双向链表中的全部数据
    int i;
    int *p;
    int sz = dlink_size();
    for (i=0; i<sz; i++)
    {
        p = (int *)dlink_get(i);
        printf("dlink_get(%d)=%d\n", i, *p);
    }

    destroy_dlink();
}

void string_test()
{
    char* sarr[4] = {"ten", "twenty", "thirty", "forty"};

    printf("\n----%s----\n", __func__);
    create_dlink();        // 创建双向链表

    dlink_insert(0, sarr[0]);    // 向双向链表的表头插入数据
    dlink_insert(0, sarr[1]);    // 向双向链表的表头插入数据
    dlink_insert(0, sarr[2]);    // 向双向链表的表头插入数据

    printf("dlink_is_empty()=%d\n", dlink_is_empty());    // 双向链表是否为空
    printf("dlink_size()=%d\n", dlink_size());            // 双向链表的大小

    // 打印双向链表中的全部数据
    int i;
    char *p;
    int sz = dlink_size();
    for (i=0; i<sz; i++)
    {
        p = (char *)dlink_get(i);
        printf("dlink_get(%d)=%s\n", i, p);
    }

    destroy_dlink();
}

typedef struct tag_stu
{
    int id;
    char name[20];
}stu;

static stu arr_stu[] = 
{
    {10, "sky"},
    {20, "jody"},
    {30, "vic"},
    {40, "dan"},
};
#define ARR_STU_SIZE ( (sizeof(arr_stu)) / (sizeof(arr_stu[0])) )

void object_test()
{
    printf("\n----%s----\n", __func__);
    create_dlink();    // 创建双向链表

    dlink_insert(0, &arr_stu[0]);    // 向双向链表的表头插入数据
    dlink_insert(0, &arr_stu[1]);    // 向双向链表的表头插入数据
    dlink_insert(0, &arr_stu[2]);    // 向双向链表的表头插入数据

    printf("dlink_is_empty()=%d\n", dlink_is_empty());    // 双向链表是否为空
    printf("dlink_size()=%d\n", dlink_size());            // 双向链表的大小

    // 打印双向链表中的全部数据
    int i;
    int sz = dlink_size();
    stu *p;
    for (i=0; i<sz; i++)
    {
        p = (stu *)dlink_get(i);
        printf("dlink_get(%d)=[%d, %s]\n", i, p->id, p->name);
    }

    destroy_dlink();
}

int main()
{
    int_test();        // 演示向双向链表操作“int数据”。
    string_test();    // 演示向双向链表操作“字符串数据”。
    object_test();    // 演示向双向链表操作“对象”。

    return 0;
}
