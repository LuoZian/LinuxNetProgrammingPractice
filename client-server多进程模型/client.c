#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
int main(){
    int ret;
    int cfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_addr,client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);
    ret = inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);
    ret = connect(cfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret == -1){
        perror("connect failed");
        exit(1);
    }
    char buffer[BUFSIZ];
    while(1){
        fgets(buffer,sizeof(buffer),stdin);
        write(cfd,buffer,strlen(buffer));
        int n = read(cfd,buffer,sizeof(buffer));
        printf("%s",buffer);
    }

    close(cfd);
    return 0;
}