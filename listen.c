#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in my_addr;
	unsigned int myport, lisnum;

	if (argv[1])
	  myport = atoi(argv[1]);
	else
	  myport = 7838;

	if (argv[2])
	  lisnum = atoi(argv[2]);
	else
	  lisnum = 2;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	else printf("socket created\n");

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = PF_INET;
	my_addr.sin_port = htons(myport);
	if(argv[3]) my_addr.sin_addr.s_addr = inet_addr(argv[3]);
	else my_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}
	else printf("binded\n");

	if (listen(sockfd, lisnum) == -1) {
		perror("listen");
		exit(1);
	}
	else printf("begin listen\n");

	sleep(100);
	close(sockfd);
	return 0;
}

// ./script port num adress

// netstat -an | grep port
// you can see the status for the port is LISTEN
