// WinsockIO_Select_Echo.cpp : Defines the entry point for the console application.
/*
* author:panzg
*description: winsock I/O , selectģ�͡�
*/

#include "stdafx.h"
#include <WinSock2.h>
#include <assert.h>

#define PORT 8000
#define MSGSIZE 1024
#pragma commet(lib,"ws2_32.lib")
int g_iTotalConn = 0;
//Ĭ�����64����������
SOCKET g_CliSocketArr[FD_SETSIZE];
DWORD WINAPI WorkerThread(LPVOID lpParameter);
int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	SOCKET slisten,sclient;
	SOCKADDR_IN local,client;
	int iaddrSize = sizeof(SOCKADDR_IN);
	DWORD dwThreadId;
	WSAStartup(0x0202,&wsaData);
	slisten = socket(AF_INET,SOCK_STREAM,0);

	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	bind(slisten,(sockaddr*)&local,iaddrSize);
	listen(slisten,5);

	//�����������߳�
	HANDLE hHandle = CreateThread(NULL,0,WorkerThread,NULL,0,&dwThreadId);
	CloseHandle(hHandle);

	while (true)
	{
		sclient = accept(slisten,(sockaddr*)&client,&iaddrSize);
		printf("Accepted client:%s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

		//add socket to fdTotal
		g_CliSocketArr[g_iTotalConn++] = sclient;
	}

	return 0;
}

DWORD WINAPI WorkerThread(LPVOID lpParam)
{
	int i;
	fd_set fdread;
	int ret;
	struct timeval tv={1,0};
	char szMessage[MSGSIZE];
	while(true)
	{
		FD_ZERO(&fdread);
		for (i=0;i<g_iTotalConn;i++)
		{
			FD_SET(g_CliSocketArr[i],&fdread);
		}
		ret = select(0,&fdread,NULL,NULL,&tv);
		if (ret == 0)
		{
			continue;
		}

		for (i=0;i<g_iTotalConn;i++)
		{
			//���������ȡ����
			if (FD_ISSET(g_CliSocketArr[i],&fdread))
			{
				ret = recv(g_CliSocketArr[i],szMessage,MSGSIZE,0);
				//���ret==0�����߻�ȡ��socket_error �����ʾ�ͻ��˹ر�����
				if (ret ==0 || ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
				{
					printf("Client socket %d closed.\n",g_CliSocketArr[i]);
					closesocket(g_CliSocketArr[i]);
					if (i<g_iTotalConn-1)
					{
						//�Ͽ����ӣ������һ��socket�Ƶ���ǰλ��
						g_CliSocketArr[i--] = g_CliSocketArr[--g_iTotalConn];
					}
				}
				else
				{
					//echo ����
					szMessage[ret] = '\0';
					send(g_CliSocketArr[i],szMessage,strlen(szMessage),0);
				}
			}
		}
	}
	//�����Ͽ�����ʱ��������Ҫ����returned������CPU�����ʼ��ߡ�
	return 0;
}

