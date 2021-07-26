#include "csapp.h"

void sigchld_handler(int sig) {

    while (waitpid(-1, NULL, WNOHANG) > 0) {         // 8.
        ;  // do nothing
    }
}

void echo(int connfd) {
    int b;
    char buf[MAXLINE];
    
    while ((b = Read(connfd, buf, MAXLINE)) > 0) {
        printf("server received %d bytes\n", b);
        Write(connfd, buf, b);
    }
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    
    Signal(SIGCHLD, sigchld_handler);                // 9.
    listenfd = Open_listenfd(argv[1]);               // 10.
    
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd,
                (SA *) &clientaddr, &clientlen);     // 11.
        
        if (Fork() == 0) {
            Close(listenfd);                         // 10.
            echo(connfd);
            Close(connfd);                           // 11.
            exit(0);                                 // 12.
        }
        
        Close(connfd);                               // 11.
    }
}