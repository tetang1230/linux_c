#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

#define BACKLOG 15 					// listen hosts
#define STDIN 0
#define port "9999"				
#define webroot "/home/chester"			// Directory to put in your files you want to host.. This is the pathname the software uses to 
							//	read files from  cant be larger that 100 chars
#define PATH_LEN 100					// 	the webroot's length cannot be larger that 100 chars 

char *DEFAULT_PAGE_NAME = "index.html";			// 	The default page name
