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

//_cdecl表示c语言的默认函数调用方法
void  _cdecl
wmain(int argc, wchar_t **argv)
{
	DWORD dwWaitStatus = 0;
	unsigned int i;
	SHARED_DATA_AREA * pData;
	LARGE_INTEGER start, end, freq;
	//LARGE_INTEG用来表示一项64位有符号整数值
	LARGE_INTEGER roundTripNanoSecs[MAXREADWRITENANOSECSIZE];

	//获取该程序运行的处理器索引
	RtPrintf("SimpleIPCProducer: Started on Processor %d\n", GetCurrentProcessorNumber());

	//创建共享内存区域，由于是同步读取内存区域，因此不需要加锁
	//RtCreateSharedMemory的用法，查看RTX Helper
	hSharedMem = RtCreateSharedMemory(PAGE_READWRITE, 0, sizeof(SHARED_DATA_AREA), _T("ProducerConsumerSample"), (void **)&pData);
	
	//共享内存区分配失败
	if(!pData)
	{
		RtPrintf("SimpleIPCProducer:  RtCreateSharedMemory failed\n");
		ExitProcess(0);
	}

	// pData的大小
	pData->size = sizeof(SHARED_DATA_AREA);
	
	// wcscpy为宽字符函数，复制
	wcscpy( pData->id, _T("ProducerConsumerSample"));
	

	// create events to share between producer and consumer
	//创建事件
	hEventProducerDone = RtCreateEvent(NULL, FALSE, FALSE, _T("ProducerDone"));

	hEvent[EXECUTE_EVENT] = RtCreateEvent(NULL, FALSE, FALSE, _T("ConsumedDone"));
	hEvent[TERMINATE_EVENT] = RtCreateEvent(NULL, FALSE, FALSE, _T("ProducerConsumerExit"));

	// start loop
	for(i=0; i< MAXREADWRITENANOSECSIZE; i++)
	{
		//QueryPerformanceCounter来精准计算执行时间
		QueryPerformanceCounter(&start);

		// 向共享内存区的producerData中填充数据
		memset(pData->producerData, i, sizeof(pData->producerData));
		
		// 发送ProducerDone的信号
		RtSetEvent(hEventProducerDone);

		//// 等待消费者的信号，dwWaitStatus保存返回状态
		dwWaitStatus = RtWaitForMultipleObjects(2, hEvent, FALSE, INFINITE);

		if (dwWaitStatus == WAIT_OBJECT_0 + TERMINATE_EVENT) 
		{
			RtPrintf("SimpleIPCProducer:  WaitForMultipleObjects failed\n");
			RtSetEvent(hEvent[TERMINATE_EVENT]);
			ExitProcess(0);
		}
		//执行结束
		if (dwWaitStatus == WAIT_OBJECT_0 + EXECUTE_EVENT)
		{
			// check results，检查消费者是否读取数据
			//if(pData->consumerData[0] != i)
			if(memcmp(pData->producerData, pData->consumerData, sizeof(pData->producerData)) != 0)
			{
				RtPrintf("SimpleIPCProducer:  Consumer failed to update data\n");
				RtSetEvent(hEvent[TERMINATE_EVENT]);
				ExitProcess(0);
			}
			
			QueryPerformanceCounter(&end);
			//QueryPerformanceFrequency返回硬件支持的高精度计数器的频率
			QueryPerformanceFrequency(&freq);
			//如果编译器支持64位，直接使用QuadPart来保存运行时间，单位是纳秒
			roundTripNanoSecs[i].QuadPart = (LONGLONG)((end.QuadPart - start.QuadPart) / ((double)(freq.QuadPart)/ (1000000000.0L))); 

		}
	}

	RtPrintf("SimpleIPCProducer:  exit\n");
	RtSetEvent(hEvent[TERMINATE_EVENT]);//发送结束事件
	
	// give consumer a chance to exit，睡眠100毫秒
	Sleep(100);

	for(i = 1; i < MAXREADWRITENANOSECSIZE; i ++)
	{
		RtPrintf("roundTrip time of sample %d = %d(nanoseconds)\n", i, roundTripNanoSecs[i].QuadPart);
		if(i > MAXREADWRITENANOSECSIZE - 1)
			break;
	}

    ExitProcess(0);
}
