/*
 ** listener.c -- a datagram sockets "server" demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "4950"

#define MAXBUFFLEN 100

// get socket address
void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void){
    struct addrinfo hints, *servinfo, *p;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my ip
    
    int rv = getaddrinfo(NULL,PORT,&hints,&servinfo);
    if(rv != 0){
        fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
        return 1;
    }

    int sock_fd;
    //loop through all the resutls and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next){
        sock_fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if(sock_fd == -1){
            perror("listener: socket");
            continue;
        }
        if(bind(sock_fd,p->ai_addr,p->ai_addrlen) == -1){
            close(sock_fd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if(p == NULL){
        fprintf(stderr,"listener: failed to bind socket");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...");
    struct sockaddr_storage their_addr;

    socklen_t addr_len = sizeof(their_addr);
    char buf[MAXBUFFLEN];
    int numbytes = recvfrom(sock_fd,buf,MAXBUFFLEN - 1,0,(struct sockaddr *)&their_addr,&addr_len);
    if(numbytes == -1){
        perror("recvfrom");
        exit(1);
    }
    char s[INET6_ADDRSTRLEN];
    printf("listener: got packet from %s\n",inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s)));
    close(sock_fd);

    return 0;
}
