/*
 ** talker.c -- a datagram sockets "client" demo
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

int main(int argc, char *argv[]){
    int sock_fd;
    struct addrinfo hints,*servinfo,*p;
    if(argc != 3){
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    int rv = getaddrinfo(argv[1],PORT,&hints,&servinfo);
    if(rv != 0){
        fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
        return 1;
    }

    //loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sock_fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
            perror("talker: socket");
            continue;
        }
        break;
    }

    if(p == NULL){
        fprintf(stderr,"talkder: failed to create socket\n");
        return 2;
    }

    int numbytes = sendto(sock_fd,argv[2],strlen(argv[2]),0,p->ai_addr,p->ai_addrlen);
    if(numbytes == -1){
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);
    printf("talker: sent %d bytes to %s\n",numbytes,argv[1]);
    close(sock_fd);

    return 0;
}
