#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>
#include"threadpool.h"
//这是利用套接字通信所作的多线程并发服务器端

//子线程所需要的信息
//子线程需要得到客户端的IP地址和端口以及用于通信的文件描述符
struct SocketInfo{
      struct sockaddr_in addr;//存放地址信息
      int fd;//文件描述符
};

typedef struct PoolInfo{//线程池需要的信息
      ThreadPool*p;
      int fd;
}PoolInfo;

struct SocketInfo infos[512];//定义大小为512的数组存放结构体

void acceptConn(void*arg);
void working(void *arg);

//子进程进行通信
void working(void *arg){//子线程需要得到客户端的IP地址和端口以及用于通信的文件描述符
    struct SocketInfo *pinfo=(struct SockInfo*)arg;
    //连接建立成功，输出客户端的IP地址和端口信息
    char ip[32];
    printf("客户端的IP:%s,端口:%d\n",
    inet_ntop(AF_INET,&pinfo->addr.sin_addr.s_addr,ip,sizeof(ip)),
    ntohs(pinfo->addr.sin_port));

    //5 通信
    while(1){
        //接受数据
        char buff[1024];
        int len=recv(pinfo->fd,buff,sizeof(buff),0);
        if(len>0){
           printf("client say: %s\n",buff);
           send(pinfo->fd,buff,len,0);
        }else if(len==0){
          printf("客户端已经断开连接....\n");
          break;
        }else{
            perror("recv error");
            break;
        }
    }
    //6 关闭文件描述符
    close(pinfo->fd);
}

int main(){
    //1 创建监听的套接字
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){
      perror("socket error!");
      return -1;
    }

    //2 绑定本地的IP地址和端口
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;/* 地址族协议: AF_INET */
    saddr.sin_port=htons(9999);//使用端口9999，同时要转换成大端
    saddr.sin_addr.s_addr=INADDR_ANY;//自动绑定本地的IP地址
    int ret= bind(fd,(struct sockaddr*)&saddr,sizeof(saddr));//这里用的sockaddr_in，方便使用
    if(ret==-1){
      perror("bind error");
      return -1;
    }
    
    //3设置监听
    ret=listen(fd,128);
    if(ret==-1){
      perror("listen error");
      return -1;
    }

    //创建线程池
    ThreadPool *pool=threadPoolCreate(3,8,100);
    PoolInfo *info=(PoolInfo*)malloc(sizeof(PoolInfo));
    info->p=pool;
    info->fd=fd;
    threadPoolAdd(pool,acceptConn,info);//检测线程池中有无新任务连接
    pthread_exit(NULL);//主线程退出

    return 0;
}

void acceptConn(void *arg){
    PoolInfo*poolinfo=(PoolInfo*)arg;
    //4 阻塞并等待客户端连接
    int addrlen=sizeof(struct sockaddr_in);
    while(1){
      struct SocketInfo *pinfo;
      pinfo=(struct SocketInfo*)malloc(sizeof(struct SocketInfo));
      int cfd=accept(poolinfo->fd,(struct sockaddr*)&pinfo->addr,&addrlen);
      pinfo->fd=cfd;
      if(cfd==-1){
        perror("accept error");
        exit(0);
      }
      //向任务队列中添加任务
      threadPoolAdd(poolinfo->p,working,pinfo);
    }
    close(poolinfo->fd);//关闭监听文件描述符
}
