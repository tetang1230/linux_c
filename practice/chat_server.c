///////////////////////////////////////////////////////////////
//////服务端tcp_select.c//////////////////////////////////
///////////////////////////////////////////////////////////////
/*******select.c*********/

/*******Using select() for I/O multiplexing*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* port we're listening on */
#define PORT 8888

// socket node struct

typedef struct _t_node node;

struct _t_node {
    int sd_num;
    node* next;
    node* prev;
};

//双向链表

typedef struct _dul_list dul_list;

struct _dul_list {
    node* head;
    node* tail;
};

dul_list* init_dul_list() {

    node* pnode = (node*) malloc(sizeof(node));
    if (pnode == NULL)
        exit(-1);

    //初始化node节点
    memset(pnode, 0, sizeof(node));

    dul_list* p_dul_list = (dul_list*) malloc(sizeof(dul_list));
    p_dul_list->head = p_dul_list->tail = pnode;

    return p_dul_list;

}

node* find_data_in_double_link(const dul_list* p_dul_list, int data) {
    node* pnode = NULL;
    if (NULL == p_dul_list->head) {
        printf("Head node is null.");
        return NULL;
    }
    pnode = p_dul_list->head;
    while (NULL != pnode) {
        if (data == pnode->sd_num)
            return pnode;
        pnode = pnode->next;
    }

    return NULL;
}

void* push_to_dul_list(dul_list* p_dul_list, int data) {

    if (NULL == p_dul_list->tail) {
        printf("Tail node is null.");
        return NULL;
    }

    if (p_dul_list->tail->sd_num == 0) {
        p_dul_list->tail->sd_num = data;
    } else {
        node* pnode = (node*) malloc(sizeof(node));
        if (pnode == NULL){
            printf("malloc fail!\n");
            exit(-1);
        }
        //初始化node节点
        memset(pnode, 0, sizeof(node));

        pnode->sd_num = data;


        //严重错误 fuck
        //pnode->prev = p_dul_list->tail;
        //pnode = p_dul_list->tail->next;

        //更改之后
        pnode->prev = p_dul_list->tail;
        p_dul_list->tail->next = pnode;
        //更改之后

        p_dul_list->tail = pnode;
        printf("push_to_dul_list data is %d\n", data);
    }
    return NULL;
}



void* delete_node(dul_list* p_dul_list, int data) {

    node* pnode;
    if (NULL == p_dul_list || p_dul_list->head == NULL)
        exit(-1);

    pnode = find_data_in_double_link(p_dul_list, data);

    if (NULL == pnode)
        return NULL;

    if (pnode == p_dul_list->head) {
        if (NULL == pnode->next) {
            p_dul_list->head = NULL;
        } else {
            p_dul_list->head = pnode->next;
            p_dul_list->head->prev = NULL;
        }

    } else {
        if (pnode->next){
            pnode->next->prev = pnode->prev;
            pnode->prev->next = pnode->next;
        }else{
           p_dul_list->tail = pnode->prev;
           pnode->prev->next = NULL;
        }
    }

    free(pnode);

    return NULL;
}

void* tranvers_dul_list(dul_list* p) {
    node* pnode = (node*) (p->head);
    while (NULL != pnode) {
        printf("tranvers: %d\n", pnode->sd_num);
        pnode = pnode->next;
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    /* master file descriptor list */
    fd_set master;

    /* temp file descriptor list for select() */
    fd_set read_fds;

    /* server address */
    struct sockaddr_in serveraddr;

    /* client address */
    struct sockaddr_in clientaddr;

    /* maximum file descriptor number */
    int fdmax;

    /* listening socket descriptor */
    int listener;

    /* newly accept()ed socket descriptor */
    int newfd;

    /* buffer for client data */
    char buf[1024];
    int nbytes;

    /* for setsockopt() SO_REUSEADDR, below */
    int yes = 1;
    int addrlen;
    int i, j;

    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* get the listener */

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server-socket() error lol!");
        /*just exit lol!*/
        exit(1);
    }

    printf("Server-socket() is OK...\n");

    /*"address already in use" error message */
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Server-setsockopt() error lol!");
        exit(1);
    }

    printf("Server-setsockopt() is OK...\n");

    /* bind */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) {
        perror("Server-bind() error lol!");
        exit(1);
    }

    printf("Server-bind() is OK...\n");

    /* listen */
    if (listen(listener, 10) == -1) {
        perror("Server-listen() error lol!");
        exit(1);
    }

    printf("Server-listen() is OK...\n");

    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/

    dul_list* pdul_list = init_dul_list();
    //初始化双向链表

    /* add the listener to the master set */
    FD_SET(listener, &master);
    push_to_dul_list(pdul_list, listener);
    //将主socket添加到双向链表

    // loop
    for (;;) {
        //copy it
        read_fds = master;
        printf("fdmax is %d\n", fdmax);
        printf("before select, test if select is blocked\n");
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Server-select() error lol!");
            exit(1);
        }

        printf("Server-select() is OK...\n");
        sleep(2);

        node* p_tmp_node = pdul_list->head; //初始化临时node节点,后续用

        while ( NULL != p_tmp_node) {

            tranvers_dul_list(pdul_list);

            if (FD_ISSET(p_tmp_node->sd_num, &read_fds)) {
                // we got one....

                if (p_tmp_node->sd_num == listener) {
                    // handle new connections
                    addrlen = sizeof(clientaddr);

                    if ((newfd = accept(listener,(struct sockaddr*) &clientaddr, &addrlen)) == -1) {
                        perror("Server-accept() error lol!");
                    } else {
                        printf("Server-accept() is OK...\n");
                        FD_SET(newfd, &master); //add to master set

                        push_to_dul_list(pdul_list, newfd);
                        //将newfd添加到双向链表

                        if (newfd > fdmax) { //keep track of the maximum
                            fdmax = newfd;
                        }
                        printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), newfd);
                    }

                }else{
                    // handle data from a client
                     if ((nbytes = recv(p_tmp_node->sd_num, buf, sizeof(buf), 0)) <= 0) {
                         //got error or connection closed by client
                         if (nbytes == 0)
                             //connection closed
                             printf("%s: socket %d hung up\n", argv[0], i);
                         else
                             perror("recv() error lol!");

                         // close it
                         close(p_tmp_node->sd_num);

                         //remove from master set
                         FD_CLR(p_tmp_node->sd_num, &master);
                         delete_node(pdul_list, p_tmp_node->sd_num);
                         //将这个socket从双向链表移除
                     }else {

                         printf("this is debug!\n");

                         node* p_loop_node = pdul_list->head;
                         //we got some data from a client
                         while(NULL != p_loop_node){
                             //send to everyone!
                             if (FD_ISSET(p_loop_node->sd_num, &master)) {
                                 //except the listener and ourselves
                                 if (p_loop_node->sd_num != listener && p_loop_node->sd_num != p_tmp_node->sd_num) {
                                      if (send(p_loop_node->sd_num, buf, nbytes, 0) == -1)
                                          perror("send() error lol!");
                                  }
                             }
                             p_loop_node = p_loop_node->next;
                         }
                     }
                }
            }

            p_tmp_node = p_tmp_node->next;
        }
    }
    return 0;
}

