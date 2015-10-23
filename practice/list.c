/**
 * 链表的简单实现
 */

#include<stdio.h>
#include<stdlib.h>

#define LEN 5

typedef struct node{
	int data;
	struct node * pnext;
}NODE, * PNODE;

/**
 * 自定义一个malloc函数
 * 省去判断每次内存是否
 * 申请成功.
 */
void * cus_malloc(size_t size){

	void * p;


	p = malloc(size);

	if(p == NULL){
		printf("cus_malloc fail!\n");
		exit(-1);
	}

	return p;

}


PNODE init(){

	PNODE pHead,pTail;
	int i;

	pHead = (PNODE)cus_malloc(sizeof(NODE));
	pTail = pHead;

	pTail->pnext = NULL;

	for(i=0; i<LEN; i++){
		
		PNODE pNew = (PNODE)cus_malloc(sizeof(NODE));
		pNew->data = i + 1;
		pNew->pnext = NULL;

		pTail->pnext = pNew;
		pTail = pNew;
	}

	return pHead;
	
}

void traverse(PNODE pHead){
	
	PNODE p = pHead;
	int i = 0;
	
	while((p = p->pnext) != NULL){
		printf("data is %d\n", p->data);
	}

	/**
	 * 以下是错误的写法
	 * 由于我pHead初始化的时候
	 * data域没有赋值,所以写的
	 * 时候需要(p = p->pnext) != NULL
	 * 否则下面的代码会把pHead的data域
	 * 打印出来(此时是脏数据)
	 */
    
	/*
	while( p->pnext != NULL){
		printf("data is %d\n", p->data);
		p = p->pnext;
	}
	*/

}

PNODE insert_node(PNODE pHead, int pos, int data){
	
	int i = 1; //计数器

	PNODE p = pHead;
	PNODE pNew = (PNODE)cus_malloc(sizeof(NODE));
	pNew->data = data;
	
	if(pos>LEN){
		printf("the insert position is wrong.\n");
		exit(-1);
	}

	while(i<pos){
		p = p->pnext;
		++i;
	}

	pNew->pnext = p->pnext;
	p->pnext = pNew;

	return pHead;

}

void del_node(PNODE pHead, int pos){

	int i = 1;
	PNODE p = pHead;
	PNODE pTmp;
	
	if(pos>LEN){
		printf("the insert position is wrong.\n");
		exit(-1);
	}

	while(i<pos){
		p = p->pnext;
		++i;
	}

	pTmp = p->pnext;
	p->pnext = p->pnext->pnext;

	free(pTmp);

	return;
}
int main(int argc, char ** argv){
	
	PNODE p;
	p = init();

	traverse(p);
	//del_node(p, 5);
	p = insert_node(p, 4, 40);
	traverse(p);
	return 0;
}


