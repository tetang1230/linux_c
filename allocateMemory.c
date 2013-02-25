#include<stdio.h>
#include<malloc.h>
#include <strings.h>
#include <string.h>
//test

void AllocateMemory(char **, int, char *); 

int main(void){
	char *s = 0;
	char *d = "test";
	int l = strlen(d);

	AllocateMemory(&s,l,d);
	/*  
	 *      int l = strlen(d);
	 *          
	 *		s = malloc(l + 1);
	 *      bzero(s, l + 1);
	 *      memcpy(s, d, l);
	 *
	 */

	printf("now *s is %s\n", s); 
}


void AllocateMemory(char **s, int l, char *d) 
{
	*s = malloc(l + 1); 
	bzero(*s, l + 1); 
	memcpy(*s, d, l); 
}
