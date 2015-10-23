#include<stdio.h>
#include<stdlib.h>

typedef struct node {
	int data;
	struct node * pnext;
}NODE, * PNODE;

typedef struct stack{
	PNODE pTop;
	PNODE pBottom;
}STACK, * PSTACK;


void * cus_malloc(size_t cnt){
	void * p;
	p = malloc(cnt);

	if(NULL == p){
		exit(-1);
	}

	return p;
}

PSTACK init(){
	PSTACK ps = (PSTACK)cus_malloc(sizeof(STACK));
	PNODE pn = (PNODE)cus_malloc(sizeof(NODE));
	
	pn->data = 0;
	pn->pnext = NULL;	

	ps->pTop = ps->pBottom = pn;

	return ps;
}

void insert(PSTACK ps, int data){
	PNODE p = (PNODE)cus_malloc(sizeof(NODE));
	p->data = data;
	p->pnext = ps->pTop;
	ps->pTop = p;
}

int out(PSTACK ps){

	int ret;
	PNODE p;

	if(ps->pTop == ps->pBottom){
		printf("");
		exit(-2);
	}

	ret = ps->pTop->data;
	p = ps->pTop;

	ps->pTop = ps->pTop->pnext;

	free(p);

	return ret;	

}


int main(int argc, char ** argv){
	PSTACK ps;
	int var;
	ps = init();

	insert(ps, 12);
	insert(ps, 34);
	
	var = out(ps);
	printf("now var is %d\n", var);
	
	
	var = out(ps);
	printf("now var is %d\n", var);
	return 0;	 	
}
