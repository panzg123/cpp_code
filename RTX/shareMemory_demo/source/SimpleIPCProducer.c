/*
*Copyright (c) 2015 IntervalZero, Inc.  All rights reserved.
*
*	Module Name:  
*		SimpleIPCProducer.c
*
*	Abstract:
*		A simple program that creates and writes to a globally named shared memory address.
*		The program writes the shared memory address and then sets a globally named event to report it has 
*		completed writing the data.  The program will run for a set amount of iterations and then set a TERMINATE_EVENT.
*		outside of the loop it will report on the performance of of each exchange.
*		To use: Run SimpleIPCProducer as an .exe or an .rtss first, then run this Application as either an
*		.exe or an .rtss
*
*	Author: 
*		IntervalZero
*
*	Environment:
*		Windows 64-bit User mode (supported configurations: Debug(x64) and Release(x64))
*		Real-time 64-bit Subsystem (supported configurations: RTSSDebug(x64) and RTSSRelease(x64))
*                         
*   Revision History:
*       IntervalZero (25-Feb-2015) Created
*/
    
#include "SimpleIPCProducer.h"
#include "ProducerConsumer.h"
#include <tchar.h>


HANDLE hEvent[TERMINATE_EVENT + 1];
HANDLE hEventProducerDone;
HANDLE hSharedMem;

#define  MAXREADWRITENANOSECSIZE 100

//_cdecl��ʾc���Ե�Ĭ�Ϻ������÷���
void  _cdecl
wmain(int argc, wchar_t **argv)
{
	DWORD dwWaitStatus = 0;
	unsigned int i;
	SHARED_DATA_AREA * pData;
	LARGE_INTEGER start, end, freq;
	//LARGE_INTEG������ʾһ��64λ�з�������ֵ
	LARGE_INTEGER roundTripNanoSecs[MAXREADWRITENANOSECSIZE];

	//��ȡ�ó������еĴ���������
	RtPrintf("SimpleIPCProducer: Started on Processor %d\n", GetCurrentProcessorNumber());

	//���������ڴ�����������ͬ����ȡ�ڴ�������˲���Ҫ����
	//RtCreateSharedMemory���÷����鿴RTX Helper
	hSharedMem = RtCreateSharedMemory(PAGE_READWRITE, 0, sizeof(SHARED_DATA_AREA), _T("ProducerConsumerSample"), (void **)&pData);
	
	//�����ڴ�������ʧ��
	if(!pData)
	{
		RtPrintf("SimpleIPCProducer:  RtCreateSharedMemory failed\n");
		ExitProcess(0);
	}

	// pData�Ĵ�С
	pData->size = sizeof(SHARED_DATA_AREA);
	
	// wcscpyΪ���ַ�����������
	wcscpy( pData->id, _T("ProducerConsumerSample"));
	

	// create events to share between producer and consumer
	//�����¼�
	hEventProducerDone = RtCreateEvent(NULL, FALSE, FALSE, _T("ProducerDone"));

	hEvent[EXECUTE_EVENT] = RtCreateEvent(NULL, FALSE, FALSE, _T("ConsumedDone"));
	hEvent[TERMINATE_EVENT] = RtCreateEvent(NULL, FALSE, FALSE, _T("ProducerConsumerExit"));

	// start loop
	for(i=0; i< MAXREADWRITENANOSECSIZE; i++)
	{
		//QueryPerformanceCounter����׼����ִ��ʱ��
		QueryPerformanceCounter(&start);

		// �����ڴ�����producerData���������
		memset(pData->producerData, i, sizeof(pData->producerData));
		
		// ����ProducerDone���ź�
		RtSetEvent(hEventProducerDone);

		//// �ȴ������ߵ��źţ�dwWaitStatus���淵��״̬
		dwWaitStatus = RtWaitForMultipleObjects(2, hEvent, FALSE, INFINITE);

		if (dwWaitStatus == WAIT_OBJECT_0 + TERMINATE_EVENT) 
		{
			RtPrintf("SimpleIPCProducer:  WaitForMultipleObjects failed\n");
			RtSetEvent(hEvent[TERMINATE_EVENT]);
			ExitProcess(0);
		}
		//ִ�н���
		if (dwWaitStatus == WAIT_OBJECT_0 + EXECUTE_EVENT)
		{
			// check results������������Ƿ��ȡ����
			//if(pData->consumerData[0] != i)
			if(memcmp(pData->producerData, pData->consumerData, sizeof(pData->producerData)) != 0)
			{
				RtPrintf("SimpleIPCProducer:  Consumer failed to update data\n");
				RtSetEvent(hEvent[TERMINATE_EVENT]);
				ExitProcess(0);
			}
			
			QueryPerformanceCounter(&end);
			//QueryPerformanceFrequency����Ӳ��֧�ֵĸ߾��ȼ�������Ƶ��
			QueryPerformanceFrequency(&freq);
			//���������֧��64λ��ֱ��ʹ��QuadPart����������ʱ�䣬��λ������
			roundTripNanoSecs[i].QuadPart = (LONGLONG)((end.QuadPart - start.QuadPart) / ((double)(freq.QuadPart)/ (1000000000.0L))); 

		}
	}

	RtPrintf("SimpleIPCProducer:  exit\n");
	RtSetEvent(hEvent[TERMINATE_EVENT]);//���ͽ����¼�
	
	// give consumer a chance to exit��˯��100����
	Sleep(100);

	for(i = 1; i < MAXREADWRITENANOSECSIZE; i ++)
	{
		RtPrintf("roundTrip time of sample %d = %d(nanoseconds)\n", i, roundTripNanoSecs[i].QuadPart);
		if(i > MAXREADWRITENANOSECSIZE - 1)
			break;
	}

    ExitProcess(0);
}
