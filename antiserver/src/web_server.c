#include"defines.h"

/* This functions gets the file size of a specified file descriptor is mainly used for allocing the size to the pointer used as a response....*/
int get_file_size(int fd)
{
	struct stat stat_struct;
	if(fstat(fd, &stat_struct) == -1)
		return(1);
	return (int)stat_struct.st_size;
}

void showConnections(struct addrinfo *res)
{
	struct addrinfo *p;
	char ip[INET6_ADDRSTRLEN];
	for(p=res;p!=NULL;p=p->ai_next)
	{
		void *addr;
		
		if(p->ai_family == AF_INET)
		{
			struct sockaddr_in *ip;
			ip = (struct sockaddr_in *)p->ai_addr;
			addr = &(ip->sin_addr);
		}
		if(p->ai_family == AF_INET6)
		{
			struct sockaddr_in6 *ip;
			ip = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ip->sin6_addr);
		}
		
		printf("debug ip is %s\n", addr);
		inet_ntop(p->ai_family,addr,ip,sizeof(ip));
		printf("Got connection from %s\n",ip);
	}	
}

void send_new(int fd,char *msg)
{
	int len = strlen(msg);
	if(send(fd,msg,len,0) == -1)
	{
		printf("Error in send\n");	
	}
}

/* This function is simply a modification to the recv() function  It recieves data untill a end of line is recieved...That is \r\n*/

int recv_new(int fd,char *buffer, int bufsize)		// This function recieves the buffer untill a "End of line" byte is recieved
{
	int i = 0; // used to test if the buffer is full or not
	#define EOL "\r\n"
	#define EOL_SIZE 2

	char *p = buffer;			// we'll be using a pointer to the buffer than to mess with buffer directly
	int eol_matched=0;			// this is used to see that the recieved byte matched the buffer byte or not 

	while(recv(fd,p,1,0) != 0 && i < bufsize)		// start recv bytes 1 by 1 and also check that we dont overflow the buffer
	{
		if( *p == EOL[eol_matched])	// if the byte matched the first eol byte that is '\r'
		{	
			++eol_matched;		

			if(eol_matched==EOL_SIZE)	// if both the bytes matches the EOL
			{
				break;
			}
		}
		else
		{
			eol_matched=0;			
		}
		p++;					// increment the pointer to recv next byte
		i++;
	}
	*(p+1-EOL_SIZE) = '\0';	// End the string
	return(strlen(buffer));	// return the bytes recieved 
}
void connection(int fd)
{
	char request[500],resource[500],*ptr;		
	int fd1,length;

	if(recv_new(fd,request, sizeof(request)) == 0)
	{
		printf("Recieve failed\n");
	}
	printf("\n\nRequest by browser = %s\n\n",request);

	// Checking for a valid browser request

	ptr = strstr(request," HTTP/");
	if(ptr == NULL)
	{
		printf("NOT HTTP!!\n");	
	}
	else
	{
		*ptr=0;
		ptr=NULL;

		if( strncmp(request,"GET ",4) == 0)
		{
			ptr=request+4;
		}
		if(strncmp(request,"HEAD ",5) == 0)
		{
			ptr=request+5;
		}
		if(ptr==NULL)
		{
			printf("Unknown Request !!! \n");
		}
		else
		{
			if( ptr[strlen(ptr) - 1] == '/' )
			{
				strncat(ptr,DEFAULT_PAGE_NAME,sizeof(ptr)-sizeof(DEFAULT_PAGE_NAME)-1);
			}
			printf("\nRequest = %s\n",ptr);
			strncpy(resource,webroot,PATH_LEN);
			strncat(resource,ptr,sizeof(resource) - sizeof(ptr) - 1);
			fd1 = open(resource,O_RDONLY,0);
			printf("Opening \"%s\"\n",resource);
			if(fd1 == -1)
			{		
				printf("404 File not found Error\n");
				send_new(fd,"HTTP/1.0 404 Not Found\r\n");
				send_new(fd,"Server : Aneesh/Private\r\n\r\n");
				send_new(fd,"<html><head><title>404 not found error!! :( </head></title>");
				send_new(fd,"<body><h1>Url not found</h1><br><p>Sorry user the url you were searching for was not found on this server!!</p><br><br><br><h1>Aneesh's Server</h1></body></html>");
			}
			else
			{
				printf("200 OK!!!\n");
				send_new(fd,"HTTP/1.0 200 OK\r\n");
				send_new(fd,"Server : Aneesh/Private\r\n\r\n");
				if( ptr == request+4 ) // if it is a get request
				{
					if( (length = get_file_size(fd1)) == -1 )
						printf("Error getting size \n");
					if( (ptr = (char *)malloc(length) ) == NULL )
						printf("Error allocating memory!!\n");
					read(fd1,ptr,length);
			
					if(send(fd,ptr,length,0) == -1)
					{
						printf("Send err!!\n");
					}
					free(ptr);
				}
			}
			close(fd);
		}
	}
	shutdown(fd,SHUT_RDWR);
}
/*
This function checks if the server
was invoked with the right privileges.
returns 0 if not invoked
and returns 1 if invoked with correct privs
*/
int check_privileges()
{
	uid_t cur_uid = getuid();
	
	if(cur_uid != 0)	// if not root
		return 0;
	else			// if root
		return 1;
}		
int main()
{
	int i ;				// used for looping through the clients
	int fdmax; 			// it is used to keep the track of the biggest file descriptor as needed by select() call
	fd_set readfds;			// temp file descriptor list for select()
	fd_set master;				// master file descriptor list
	
	int sockfd,newfd;  			 // newfd is used for the fd returned by the connect() call and sockfd is socket fd
	int err;				// This is used for checking the error returned by getaddrinfo() below!!
	struct addrinfo *res,*p,hints;		 // These are all used by getaddrinfo() call
	struct sockaddr_storage their_addr;	 // client addr
	socklen_t addr_size;			// addr size of client addr
	int yes=1;				// used for setsockopt SO_REUSEADDR
	char ip[INET6_ADDRSTRLEN];		// client's ip

	memset(&hints,0,sizeof(hints));		// zero out the whole structure
	hints.ai_family=AF_UNSPEC;		// Unspecific to make my server ip version independent
	hints.ai_flags=AI_PASSIVE;		// Fill in my own ip
	hints.ai_socktype=SOCK_STREAM;		// TCP reliable protocol

	FD_ZERO(&readfds);			// empty the sets
	FD_ZERO(&master);			// 
	
	if(!check_privileges())
	{
		printf("Server is using Port 80 which requires root privileges\n\n+++++++++++++++++++++++++++++++++++++++++++++++++++++\n\tInvoke the server as root\n+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		return(-1);
	}
	
	printf("Server is open for listening on port 80\n");
	if( (err = getaddrinfo(NULL,port,&hints,&res) ) == -1)		// getaddrinfo() returns -1 on error 
	{
		printf("Err in getaddrinfo : %s\n",gai_strerror(err));	// what the error
		return(1);						// die with error
	}

	for(p=res;p!=NULL;p=p->ai_next)					// loop and bind
	{		
		if( ( sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol) ) == -1)
		{
			perror("Socket error");
			continue;
		}
		if( setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) // for avoiding irritating addr in use err 
		{
			
			perror("Setsockopt err!!\n");
			return(1);
		}

		if( bind(sockfd,p->ai_addr,p->ai_addrlen) == -1)
		{
			
			perror("Binding err");
			close(sockfd);		
			continue;
		}
		break;
	}

	if( listen(sockfd,BACKLOG) == -1)					// listen with backlog 15
	{
		perror("Error in listen");
		return(1);
	}
	// lets add the listener i.e the sockfd to the master set

	FD_SET(sockfd,&master);

	fdmax = sockfd; // so far the sockfd is the biggest fd
	while(1)
	{
		readfds = master; // copy the master as we'll be reading from this fd   
                if(select(fdmax+1,&readfds,NULL,NULL,NULL) == -1)
                {
                        perror("select : ");
                        return(-1);
                }
		for(i = 0;i <= fdmax ; i++)
		{
			if(FD_ISSET(i,&readfds)) // we got one connection
			{
				if(i == sockfd)
				{
					addr_size = sizeof(their_addr);
					if( ( newfd = accept(sockfd, (struct sockaddr *)&their_addr,&addr_size) ) == -1)
					{
						perror("Error in accept");
					}
					else
					{
						FD_SET(newfd,&master);
						if(newfd > fdmax)
						{
							fdmax=newfd;
						}
						time_t ti = time(NULL);					// know the time
						char *s = ctime(&ti); 					// convert it into string
						s[strlen(s) - 1] = '\0';        			// remove the new line
					}
					showConnections(res);
				}
				else
				{
					connection(i);
					// we accepted the requests now lets handle data from the clients
					FD_CLR(i , &master); // we handles the request time to say bye!!! remove from the master set..		
					close(i);				
				} // end handling data from client
			}// END got new connection
		} // end the looping through fd's
	}// end the while(1) loop
	freeaddrinfo(res);
	close(newfd);
	close(sockfd);
	return(0);
}
