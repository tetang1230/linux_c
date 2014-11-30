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

void en_q(PQ p, int i){

	if(is_full(p)){
		printf("队列已满!");
		exit(-2);
	}

	//p->arr[p->rear] = i;
	p->pn[p->rear].data = i;  
	p->rear++;

}


int main(int argc, const char *argv[])
{

	PQ p = init();
	int i;

	en_q(p,1);
	en_q(p,100);
	en_q(p,3);
	
	for (i = 0; i < 3; i++) {
		printf("%d\n", p->pn[i].data);
	}
	return 0;
}
