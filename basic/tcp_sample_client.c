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
#include <sys/socket.h>

#if 0
int main(int argc, char *argv[])
{
	int sockfd,new_fd;
	int sockaddr_size;
	int ret;
	int len;
	struct sockaddr_in server_addr;   //结构体sockaddr_in 和 aockaddr 是兼容的
	 struct hostent *host;
	int sin_size,portnumber;
	char buf[1024];

	fd_set rfds;
	struct timeval tv;
	int maxfd;
	FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

	 if((host=gethostbyname(argv[1])) == 0)
	{
		printf("server ip %s error\n",argv[1]);
		exit(1);
	}

	if((portnumber = atoi(argv[2]))<0)
	{
		printf("port number error, will exit\n");
		exit(1);
	}
	/*  客户程序开始建立  sockfd 描述符    */
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)    //AF_INET也可以用PF_INET代替，表明针对錓neternet，可以建立远程连接。SOCK_STREAM表明使用的是TCP协议
	{   													//而 SOCK_DGRAM 表明我们用的是 UDP 协议
		printf("socket, will exit\n");
		exit(1);
	}
	/*  客户程序填充服务端的资料              */
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(portnumber);
	server_addr.sin_addr=*((struct in_addr *)host->h_addr);

	/*  客户程序发起连接请求                  */
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
	{
		printf("connect will exit\n");
		exit(1);
	}
	printf("start talk...\n");

	FD_SET(sockfd, &rfds);
		//进行会话
		while(1)
		{

			FD_ZERO(&rfds);
			FD_SET(STDIN_FILENO, &rfds);
			FD_SET(sockfd, &rfds);

			//maxfd = new_fd > STDIN_FILENO ? newfd ：STDIN_FILENO;
			//maxfd = maxfd + 1;
			maxfd = sockfd + 1;

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
					//printf("Client	:");
					fgets(buf, 1024, stdin);
					if (!strncasecmp(buf, "quit", 4))
					{
						printf("current talk exit\n");
						FD_CLR(sockfd, &rfds);
						close(sockfd);
						break;
					}
					//printf("Client	:%s\n",buf);
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
					len = send(sockfd,buf,strlen(buf)-1,0);
					if(len < 0)
					{
						printf("send fail\n");
					}



			}
			else if(FD_ISSET(sockfd, &rfds))
			{

				bzero(buf,1024);
				len = recv(sockfd,buf,strlen(buf)-1,0);
				if(len < 0)
				{
					printf("receive fail\n");
				}
				if(len == 0)//当服务器终止运行的时候，会不断检测到sockfd的状态变化，但是收到的信息长度为0
				{
					printf("Server has already exit, will exit\n");
					FD_CLR(sockfd, &rfds);
					close(sockfd);
					break;

				}
				if (!strncasecmp(buf, "quit", 4))
				{
					printf("current talk exit\n");
					FD_CLR(sockfd, &rfds);
					close(sockfd);
					break;
				}
				buf[len+1]='\0';
				printf("		Server:	%s\n",buf);

			}

		}



}
#endif

int main(int argc, char *argv[])
{
	char tmp[200];
	char str[10];
	while(scanf("%s",&str)!=EOF)
	{
		sprintf(tmp,"echo %s | nc localhost 10001",str);
		system(tmp);
	}
	return 0;
//	system("echo \"中国人民\" | nc localhost 10001");
}
