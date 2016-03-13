// Winsock_IOmodel_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <WinSock2.h>  
#include <stdio.h>  
#define SERVER_ADDRESS "127.0.0.1"  
#define PORT       8000  
#define MSGSIZE    1024  
#pragma comment(lib, "ws2_32.lib")  
int main()  
{  
	//����WSAStartup()�����ĵڶ�������������Windows Socketsʵ�ֵ�ϸ�ڡ�  
	WSADATA wsaData;  
	//�����������socket����ͨ�ŵĿͻ���socket��  
	SOCKET sClient;  
	//�������÷������ĵ�ַ��Ϣ��  
	SOCKADDR_IN server;  
	char szMessage[MSGSIZE];  
	int ret;   
	//��һ������ʼ��Winsock��  
	WSAStartup(0x0202, &wsaData);  
	//�ڶ������������������������ͨ�ŵĿͻ���  
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
	//�����������������˵ĵ�ַ��Ϣ������SOCKADDR_IN���͵ı���sever��  
	memset(&server, 0, sizeof(SOCKADDR_IN));  
	server.sin_family = AF_INET;  
	server.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);  
	server.sin_port = htons(PORT);  
	//���Ĳ���ͨ��connect������������������ӡ�  
	connect(sClient, (struct sockaddr *)&server, sizeof(SOCKADDR_IN));  
	while (TRUE)  
	{  
		//���ӷ������ɹ��󣬿ͻ��˿���̨���ڽ���ʾSend:  
		printf("Send:");  
		//���û���������ݱ��浽szMessage��  
		gets(szMessage);  
		//������Ϣ��szMessage�е�����ͨ��sClient����������  
		send(sClient, szMessage, strlen(szMessage), 0);  
		//�����յ������ݷ���szMessage��  
		ret = recv(sClient, szMessage, MSGSIZE, 0);  
		szMessage[ret] = '\0';  
		printf("Received [%d bytes]: '%s'\n", ret, szMessage);  
	}  
	closesocket(sClient);  
	WSACleanup();  
	return 0;  
}  