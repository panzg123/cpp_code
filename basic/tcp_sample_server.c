#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
	int sockfd,new_fd;
	socklen_t sockaddr_size;
	int ret;
	int len;
	struct sockaddr_in server_addr;   //结构体sockaddr_in 和 aockaddr 是兼容的
	struct sockaddr_in client_addr;
	int sin_size,portnumber;
	char buf[1024];

	fd_set rfds;
	struct timeval tv;
	int maxfd;
	FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);


	if((portnumber = atoi(argv[1]))<0)
	{
		printf("port number error, will exit\n");
		exit(1);
	}
	/*  服务器端开始建立 socket 描述符  */
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)    //AF_INET也可以用PF_INET代替，表明针对錓neternet，可以建立远程连接。SOCK_STREAM表明使用的是TCP协议
	{   													//而 SOCK_DGRAM 表明我们用的是 UDP 协议
		printf("socket, will exit\n");
		close(sockfd);
		exit(1);
	}
	/*  服务器端填充  sockaddr 结构    */
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);   //标识可以和任何主机通信
	server_addr.sin_port=htons(portnumber);

	/*  捆绑 sockfd 描述符    */
	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
	{
        printf("bind error, will exit\n");
		close(sockfd);
        exit(1);
	}
	/*  监听 sockfd 描述符    */
	if(listen(sockfd,5)==-1)  //设置客户端请求排队的最大长度，允许最大5个
	{
        printf("listen, will exit\n");
		close(sockfd);
		exit(1);
	}


	while(1)
	{
		 /*  服务器阻塞,直到客户程序建立连接    */
		printf("Will go to talk, wait connecting...\n");
		sockaddr_size = sizeof(struct sockaddr);
		//int accept(int sockfd, struct sockaddr *addr,int *addrlen)
		if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sockaddr_size))==-1)
		{
			printf("accept error, will exit\n");
			close(sockfd);
			exit(1);
		}
		else
		{
            printf("server: get connection from %s, port %d, socket %d\n",
                   (char *)inet_ntoa(client_addr.sin_addr),
				   ntohs(client_addr.sin_port),
				   new_fd);
		}
		//进行会话
		while(1)
		{

			FD_ZERO(&rfds);
			FD_SET(STDIN_FILENO, &rfds);
			FD_SET(new_fd, &rfds);
			//maxfd = new_fd > STDIN_FILENO ? newfd ：STDIN_FILENO;
			//maxfd = maxfd + 1;
			maxfd = new_fd + 1;

			tv.tv_sec = 1;
			tv.tv_usec = 0;//设置等待时间
			ret = select(maxfd, &rfds, NULL, NULL, &tv);
			if(ret<0)
			{
				printf("select error, process will eixt\n");
				exit(0);
			}
			else if(FD_ISSET(STDIN_FILENO, &rfds))//测试是否有数据
			{
					bzero(buf,1024);
					//printf("Server	:");
					fgets(buf, 1024, stdin);
					if (!strncasecmp(buf, "quit", 4))
					{
						printf("current talk exit\n");
						FD_CLR(new_fd, &rfds);
						close(new_fd);
						break;
					}
					//printf("Server	:%s\n",buf);
					//  int recv(int sockfd,void *buf,int len,int flags)
					//  int send(int sockfd,void *buf,int len,int flags)
					//最后一个参数可以是0，也可以是
/*					  _______________________________________________________________
|    MSG_DONTROUTE                |    不查找路由表                                                  |
|    MSG_OOB                            |    接受或者发送带外数据                                    |
|    MSG_PEEK                          |    查看数据,并不从系统缓冲区移走数据            |
|    MSG_WAITALL                    |    等待所有数据                                                  |
|--------------------------------------------------------------|
*/
					len = send(new_fd,buf,strlen(buf)-1,0);
					if(len < 0)
					{
						printf("send fail\n");
					}



			}
			else if(FD_ISSET(new_fd, &rfds))
			{

				bzero(buf,1024);
				len = recv(new_fd,buf,strlen(buf)-1,0);
				if(len < 0)
				{
					printf("receive fail\n");
				}
				if(len == 0)//当客户端终止运行的时候，会不断检测到sockfd的状态变化，但是收到的信息长度为0
				{
					printf("Client has already exit, will exit\n");
					FD_CLR(new_fd, &rfds);
					close(new_fd);
					break;

				}
				if (!strncasecmp(buf, "quit", 4))
				{
					printf("current talk exit\n");
					FD_CLR(new_fd, &rfds);
					close(new_fd);
					break;
				}
				buf[len+1]='\0';
				printf("		Client	:%s\n",buf);

			}

		}

	}

}
