
/**
 * 1. 循环队列里面就是数学问题
 * 2. 队头和队尾相等时,既可以判断是队列已满,也可以判断队列为空
 * 为了解决这个问题,队列中最后一个元素只占空间,不存数据,这样的话
 * front == rear的时候为空队列,  (rear + 1) % LEN = front的时候
 * 队列就满
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define LEN 5

typedef struct _node {
       int data;
       struct _node * pnext;
} node, * pnode;

typedef struct _QUEUE{
	int front;
	int rear;
	//int * arr;
	pnode pn;
} QUEUE, * PQ;

void * cus_malloc(size_t s){
	
	void * p;
	p = malloc(s);

	if(p == NULL){
		printf("malloc failed!");
		exit(-1);
	}

	return p;
}

PQ init(){
	
	PQ pq = (PQ)cus_malloc(sizeof(QUEUE));
	pq->front = pq->rear = 0;
	//pq->arr = (int *)cus_malloc(LEN*sizeof(int));
	pq->pn = (pnode)cus_malloc(sizeof(node)*LEN);



	return pq;

}

bool is_full(PQ p){

	if((p->rear+1)%LEN == p->front){
		return true;
	}

	return false;

}

bool is_empty(PQ p){

	if (p->front == p->rear) {
		return true;
	}

	return false;
}

void en_q(PQ p, int i){

	if(is_full(p)){
		printf("队列已满!");
		exit(-2);
	}

	//p->arr[p->rear] = i;
	p->pn[p->rear].data = i;  
		
	//p->rear++;
	//错误的写法,一直入队列,迟早要
	//超过最大数LEN的,下面的写法正
	//确

	p->rear = (p->rear + 1) % LEN;
}

void out_q(PQ p){
	
	int data;
	
	if(!is_empty(p)){
		data = p->pn[p->front].data;
		p->front = (p->front + 1) % LEN;
	}else{
		printf("队列为空！");
		exit(-3);
	}
}

void tranverse(PQ p){

	int i;
	int s = p->front;
	int e = p->rear;
	
	for (i = s; i < e; i++) {
		printf("%d\n", p->pn[i].data);
	}

}

void destory_queue(PQ p){
    
    if(p->pn)
      free(p->pn);
    p->pn = NULL;

    //free掉内存以后,指针还继续指向动态分配的内存
    //需要将指针指向null,这样安全

    p->front = p->rear = 0;

}

int main(int argc, const char *argv[])
{

	PQ p = init();
	int i;

	en_q(p,1);
	en_q(p,100);
	en_q(p,3);
	out_q(p);

	tranverse(p);

	return 0;
}
