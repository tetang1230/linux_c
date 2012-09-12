#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in dest, mine;
	char buffer[MAXBUF];

	if (argc != 5) {
		printf
			("invoke error, please try ./script 127.0.0.1 22 127.0.0.1 9999");
		exit(0);
	}
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket");
		exit(errno);
	}

	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) {
		perror(argv[1]);
		exit(errno);
	}

	bzero(&mine, sizeof(mine));
	mine.sin_family = AF_INET;
	mine.sin_port = htons(atoi(argv[4]));
	if (inet_aton(argv[3], (struct in_addr *) &mine.sin_addr.s_addr) == 0) {
		perror(argv[3]);
		exit(errno);
	}

	if (bind(sockfd, (struct sockaddr *) &mine, sizeof(struct sockaddr)) == -1) {
		perror(argv[3]);
		exit(errno);
	}

	if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
		perror("Connect ");
		exit(errno);
	}

	bzero(buffer, MAXBUF);
	recv(sockfd, buffer, sizeof(buffer), 0);
	printf("%s", buffer);
	sleep(10);
	
	close(sockfd);
	return 0;
}


// ./script 127.0.0.1 22 127.0.0.1 9999
//
// netstat -an | grep 9999
// you will see the status of ESTABLISHED
