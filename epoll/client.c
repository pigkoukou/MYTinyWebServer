// client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket error");
        exit(0);
    }

    // 2. 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);   // 大端端口
    inet_pton(AF_INET, "10.0.4.17", &addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect error");
        exit(0);
    }

    // 3. 和服务器端通信
    int number = 0;
    char buf[1024]={0};
    while(number<200)
    {
        // 发送数据
        
        sprintf(buf, "你好, 服务器...%d\n", number++);
        printf("%s\n",buf);
        write(fd, buf, strlen(buf)+1);
        recv(fd,buf,sizeof(buf),0);
        printf("recv msg:%s\n",buf);
       
        usleep(100000);   
    }

    close(fd);

    return 0;
}