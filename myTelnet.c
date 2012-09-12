#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#define DEFAULTIP "127.0.0.1"
#define DEFAULTPORT "23"
#define DEFAULTBACK "10"
#define DEFAULTDIR "/tmp"
#define DEFAULTLOG "/tmp/telnet-server.log"

void prterrmsg(char *msg);
#define prterrmsg(msg)        { perror(msg); abort(); }
void wrterrmsg(char *msg);
#define wrterrmsg(msg)        { fputs(msg, logfp); fputs(strerror(errno), logfp);fflush(logfp); abort(); }

void prtinfomsg(char *msg);
#define prtinfomsg(msg)        { fputs(msg, stdout);  }
void wrtinfomsg(char *msg);
#define wrtinfomsg(msg)        {  fputs(msg, logfp); fflush(logfp);}

#define MAXBUF        1024

char buffer[MAXBUF + 1];
char *host = 0;
char *port = 0;
char *back = 0;
char *dirroot = 0;
char *logdir = 0;
unsigned char daemon_y_n = 0;
FILE *logfp;

#define MAXPATH        150


void AllocateMemory(char **s, int l, char *d)
{
	*s = malloc(l + 1);
	bzero(*s, l + 1);
	memcpy(*s, d, l);
}

void getoption(int argc, char **argv)
{
	int c, len;
	char *p = 0;

	opterr = 0;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"host", 1, 0, 0},
			{"port", 1, 0, 0},
			{"back", 1, 0, 0},
			{"dir", 1, 0, 0},
			{"log", 1, 0, 0},
			{"daemon", 0, 0, 0},
			{0, 0, 0, 0}
		};
		
		c = getopt_long(argc, argv, "H:P:B:D:L",
					long_options, &option_index);
		if (c == -1 || c == '?')
		  break;

		if(optarg)        len = strlen(optarg);
		else        len = 0;

		if ((!c && !(strcasecmp(long_options[option_index].name, "host")))
					|| c == 'H')
		  p = host = malloc(len + 1);
		else if ((!c
						&&
						!(strcasecmp(long_options[option_index].name, "port")))
					|| c == 'P')
		  p = port = malloc(len + 1);
		else if ((!c
						&&
						!(strcasecmp(long_options[option_index].name, "back")))
					|| c == 'B')
		  p = back = malloc(len + 1);
		else if ((!c
						&& !(strcasecmp(long_options[option_index].name, "dir")))
					|| c == 'D')
		  p = dirroot = malloc(len + 1);
		else if ((!c
						&& !(strcasecmp(long_options[option_index].name, "log")))
					|| c == 'L')
		  p = logdir = malloc(len + 1);
		else if ((!c
						&&
						!(strcasecmp
							(long_options[option_index].name, "daemon")))) {
			daemon_y_n = 1;
			continue;
		}
		else
		  break;
		bzero(p, len + 1);
		memcpy(p, optarg, len);
	}
}

int main(int argc, char **argv)
{
	struct sockaddr_in addr;
	int sock_fd, addrlen;

	getoption(argc, argv);

	if (!host) {
		addrlen = strlen(DEFAULTIP);
		AllocateMemory(&host, addrlen, DEFAULTIP);
	}
	if (!port) {
		addrlen = strlen(DEFAULTPORT);
		AllocateMemory(&port, addrlen, DEFAULTPORT);
	}
	if (!back) {
		addrlen = strlen(DEFAULTBACK);
		AllocateMemory(&back, addrlen, DEFAULTBACK);
	}
	if (!dirroot) {
		addrlen = strlen(DEFAULTDIR);
		AllocateMemory(&dirroot, addrlen, DEFAULTDIR);
	}
	if (!logdir) {
		addrlen = strlen(DEFAULTLOG);
		AllocateMemory(&logdir, addrlen, DEFAULTLOG);
	}

	printf
		("host=%s port=%s back=%s dirroot=%s logdir=%s %s daemon mode(Proccess ID：%d)\n",
		 host, port, back, dirroot, logdir, daemon_y_n?"":"is not", getpid());

	if (daemon_y_n) {
		if (fork())
		  exit(0);
		if (fork())
		  exit(0);
		close(0), close(1), close(2);
		logfp = fopen(logdir, "a+");
		if (!logfp)
		  exit(0);
	}

	signal(SIGCHLD, SIG_IGN);

	if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		if (!daemon_y_n) {
			prterrmsg("socket()");
		} else {
			wrterrmsg("socket()");
		}
	}

	addrlen = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen,
				sizeof(addrlen));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(host);
	addrlen = sizeof(struct sockaddr_in);
	
	if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0) {
		if (!daemon_y_n) {
			prterrmsg("bind()");
		} else {
			wrterrmsg("bind()");
		}
	}

	if (listen(sock_fd, atoi(back)) < 0) {
		if (!daemon_y_n) {
			prterrmsg("listen()");
		} else {
			wrterrmsg("listen()");
		}
	}
	
	while (1) {
		int new_fd;
		addrlen = sizeof(struct sockaddr_in);
		
		new_fd = accept(sock_fd, (struct sockaddr *) &addr, &addrlen);
		if (new_fd < 0) {
			if (!daemon_y_n) {
				prterrmsg("accept()");
			} else {
				wrterrmsg("accept()");
			}
			break;
		}
		bzero(buffer, MAXBUF + 1);
		sprintf(buffer, "The connection is from : %s:%d\n",
					inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		if (!daemon_y_n) {
			prtinfomsg(buffer);
		} else {
			wrtinfomsg(buffer);
		}
		
		if (!fork()) {
			dup2(new_fd, 0);
			dup2(new_fd, 1);
			dup2(new_fd, 2);
			chdir(dirroot);
			
			execl("/bin/bash", "-l", "--login", "-i", "-r", "-s", (char *)NULL);
		}
		close(new_fd);
	}
	close(sock_fd);
	return 0;
}
