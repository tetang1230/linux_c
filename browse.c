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
#define DEFAULTPORT "80"
#define DEFAULTBACK "10"
#define DEFAULTDIR "/home"
#define DEFAULTLOG "/tmp/chester_ftp.log"

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

char *dir_up(char *dirpath)
{
	static char Path[MAXPATH];
	int len;

	strcpy(Path, dirpath);
	len = strlen(Path);
	if (len > 1 && Path[len - 1] == '/')
	  len--;
	while (Path[len - 1] != '/' && len > 1)
	  len--;
	Path[len] = 0;
	return Path;
}

void AllocateMemory(char **s, int l, char *d)
{
	*s = malloc(l + 1);
	bzero(*s, l + 1);
	memcpy(*s, d, l);
}

void GiveResponse(FILE * client_sock, char *Path)
{
	struct dirent *dirent;
	struct stat info;
	char Filename[MAXPATH];
	DIR *dir;
	int fd, len, ret;
	char *p, *realPath, *realFilename, *nport;

	len = strlen(dirroot) + strlen(Path) + 1;
	realPath = malloc(len + 1);
	bzero(realPath, len + 1);
	sprintf(realPath, "%s/%s", dirroot, Path);

	len = strlen(port) + 1;
	nport = malloc(len + 1);
	bzero(nport, len + 1);
    sprintf(nport, ":%s", port);
	
	if (stat(realPath, &info)) {
		fprintf(client_sock,
					"HTTP/1.1 200 OK\r\nServer: Chester Test FTP \r\nConnection: close\r\n\r\n<html><head><title>%d - %s</title><meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" /></head>"
					"<body><font size=+4>FTP Directory</font><br><hr width=\"100%%\"><br><center>"
					"<table border cols=3 width=\"100%%\">", errno,
					strerror(errno));
		fprintf(client_sock,
					"</table><font color=\"CC0000\" size=+2>Server Error ：\n%s %s</font></body></html>",
					Path, strerror(errno));
		goto out;
	}
	
	if (S_ISREG(info.st_mode)) {
		fd = open(realPath, O_RDONLY);
		len = lseek(fd, 0, SEEK_END);
		p = (char *) malloc(len + 1);
		bzero(p, len + 1);
		lseek(fd, 0, SEEK_SET);
		ret = read(fd, p, len);
		close(fd);
		fprintf(client_sock,
					"HTTP/1.1 200 OK\r\nServer: Chester Test FTP \r\nConnection: keep-alive\r\nContent-type: application/*\r\nContent-Length:%d\r\n\r\n",
					len);
		fwrite(p, len, 1, client_sock);
		free(p);
	} else if (S_ISDIR(info.st_mode)) {
		
		dir = opendir(realPath);
		fprintf(client_sock,
					"HTTP/1.1 200 OK\r\nServer: Chester Test FTP\r\nConnection: close\r\n\r\n<html><head><title>%s</title><meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" /></head>"
					"<body><font size=+4>FTP Directory</font><br><hr width=\"100%%\"><br><center>"
					"<table border cols=3 width=\"100%%\">", Path);
		fprintf(client_sock,
					"<caption><font size=+3>Dir %s</font></caption>\n",
					Path);
		fprintf(client_sock,
					"<tr><td>name</td><td>size</td><td>Modification time</td></tr>\n");
		if (dir == 0) {
			fprintf(client_sock,
						"</table><font color=\"CC0000\" size=+2>%s</font></body></html>",
						strerror(errno));
			return;
		}
		
		while ((dirent = readdir(dir)) != 0) {
			if (strcmp(Path, "/") == 0)
			  sprintf(Filename, "/%s", dirent->d_name);
			else
			  sprintf(Filename, "%s/%s", Path, dirent->d_name);
			fprintf(client_sock, "<tr>");
			len = strlen(dirroot) + strlen(Filename) + 1;
			realFilename = malloc(len + 1);
			bzero(realFilename, len + 1);
			sprintf(realFilename, "%s/%s", dirroot, Filename);
			if (stat(realFilename, &info) == 0) {
				if (strcmp(dirent->d_name, "..") == 0)
				  fprintf(client_sock,
							  "<td><a href=\"http://%s%s%s\">(parent)</a></td>",
							  host, atoi(port) == 80 ? "" : nport,
							  dir_up(Path));
				else
				  fprintf(client_sock,
							  "<td><a href=\"http://%s%s%s\">%s</a></td>",
							  host, atoi(port) == 80 ? "" : nport, Filename,
							  dirent->d_name);
	            if (S_ISDIR(info.st_mode))
				  fprintf(client_sock, "<td>dir</td>");
				else if (S_ISREG(info.st_mode))
				  fprintf(client_sock, "<td>%d</td>", info.st_size);
				else if (S_ISLNK(info.st_mode))
				  fprintf(client_sock, "<td>link</td>");
				else if (S_ISCHR(info.st_mode))
				  fprintf(client_sock, "<td>char device</td>");
				else if (S_ISBLK(info.st_mode))
				  fprintf(client_sock, "<td>device</td>");
				else if (S_ISFIFO(info.st_mode))
				  fprintf(client_sock, "<td>FIFO</td>");
				else if (S_ISSOCK(info.st_mode))
				  fprintf(client_sock, "<td>Socket</td>");
				else
				  fprintf(client_sock, "<td>(unknown)</td>");
				fprintf(client_sock, "<td>%s</td>", ctime(&info.st_ctime));
			}
			fprintf(client_sock, "</tr>\n");
			free(realFilename);
		}
		fprintf(client_sock, "</table></center></body></html>");
	} else {
		
		fprintf(client_sock,
					"HTTP/1.1 200 OK\r\nServer: Chester Test Server\r\nConnection: close\r\n\r\n<html><head><title>permission denied</title></head>"
					"<body><font size=+4>FTP Directory</font><br><hr width=\"100%%\"><br><center>"
					"<table border cols=3 width=\"100%%\">");
		fprintf(client_sock,
					"</table><font color=\"CC0000\" size=+2>Forbidden.</font></body></html>",
					Path);
	}
out:
	free(realPath);
	free(nport);
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
		("host=%s port=%s back=%s dirroot=%s logdir=%s %s daemon mode (pID：%d)\n",
		 host, port, back, dirroot, logdir, daemon_y_n?"":"is not", getpid());

	/* fork() twice for daemon  */
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
		int len;
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
		sprintf(buffer, "connection from : %s:%d\n",
					inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		if (!daemon_y_n) {
			prtinfomsg(buffer);
		} else {
			wrtinfomsg(buffer);
		}
		
		if (!fork()) {
			bzero(buffer, MAXBUF + 1);
			if ((len = recv(new_fd, buffer, MAXBUF, 0)) > 0) {
				FILE *ClientFP = fdopen(new_fd, "w");
				if (ClientFP == NULL) {
					if (!daemon_y_n) {
						prterrmsg("fdopen()");
					} else {
						prterrmsg("fdopen()");
					}
				} else {
					char Req[MAXPATH + 1] = "";
					sscanf(buffer, "GET %s HTTP", Req);
					bzero(buffer, MAXBUF + 1);
					sprintf(buffer, "request file : \"%s\"\n", Req);
					if (!daemon_y_n) {
						prtinfomsg(buffer);
					} else {
						wrtinfomsg(buffer);
					}
					
					GiveResponse(ClientFP, Req);
					fclose(ClientFP);
				}
			}
			exit(0);
		}
		close(new_fd);
	}
	close(sock_fd);
	return 0;
}
