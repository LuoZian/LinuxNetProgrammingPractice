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
#include <errno.h>
void do_sigchld(int x){//while循环回收sigchld到来时所有的僵尸子进程
    pid_t pid;
    int status;
    while((pid = waitpid(-1,&status,WNOHANG))>0)  //此时若没有子进程，则返回-1，有子进程但都不是僵尸，则返回0
        printf("Child process %d has been retreived\n",pid);
    
}
int main(){
    int ret;
    struct sigaction my_sigact,old_sigact;
    my_sigact.sa_handler = do_sigchld;
    my_sigact.sa_flags = 0;
    ret = sigaction(SIGCHLD,&my_sigact,&old_sigact);
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_addr,client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);
    ret = inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr.s_addr);
    if(ret == -1){
        perror("convert IP addr fail");
        exit(1);
    }
    ret = bind(lfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret == -1){
        perror("bind fail");
        exit(1);
    }
    listen(lfd,128);
    socklen_t client_addr_len = sizeof(client_addr);
    pid_t pid;int cfd;char buffer[BUFSIZ];
    while(1){
        cfd = accept(lfd,(struct sockaddr*)&client_addr,&client_addr_len);
        if(cfd == -1&&errno == EINTR) //检查accept是不是被SIGCHLD打断的
            continue;
        printf("%s\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,buffer,BUFSIZ));
        pid = fork();
        if(pid == 0)
            break;
        close(cfd);
    }
    if(pid == 0){
        close(lfd);
        while(1){
            int n = read(cfd,buffer,sizeof(buffer));
            if(n == 0){
                printf("A client has quited\n");
                return 0;
            }
            else if(n == -1)
                if(errno == EINTR)
                    continue;
                else{
                    perror("read error");
                    exit(1);
                }
            for(int i = 0;i<n;i++)
                buffer[i] = toupper(buffer[i]);
            write(cfd,buffer,n);
        }
    }
    return 0;
}