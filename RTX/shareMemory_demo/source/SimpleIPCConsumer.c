/*
*Copyright (c) 2015 IntervalZero, Inc.  All rights reserved.
*
*	Module Name:  
*		SimpleIPCConsumer.c
*
*	Abstract:
*		A simple program that opens a memory address that is shared between the RTX64 subsystem and windows.
*		The program reads the shared memory address and then sets a globally named event to report it has 
*		completed reading the data.  The program will end when a TERMINATE_EVENT is signaled.  
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
    
#include "SimpleIPCConsumer.h"
#include "ProducerConsumer.h"
#include <tchar.h>

HANDLE hEvent[TERMINATE_EVENT + 1];
HANDLE hEventConsumedDone;
HANDLE hSharedMem;


void 
_cdecl
wmain(
       int     argc,
       wchar_t **argv
     )
{

	DWORD dwWaitStatus = 0;
	SHARED_DATA_AREA * pData;

	RtPrintf("SimpleIPCConsumer: Started on Processor %d\n", GetCurrentProcessorNumber());

	//  open shared memory，打开内存共享区，第三个参数是内存共享区对象的名字，不需要加锁
	hSharedMem = RtOpenSharedMemory(SHM_MAP_WRITE, FALSE,  _T("ProducerConsumerSample"), (void**)&pData);
	if(!hSharedMem)
	{
		RtPrintf("SimpleIPCConsumer: RtOpenSharedMemory failed\n Last error: %d", GetLastError());
		ExitProcess(0);
	}

	// 构建事件
	hEventConsumedDone = RtCreateEvent(NULL, FALSE, FALSE, _T("ConsumedDone"));

	hEvent[EXECUTE_EVENT] = RtCreateEvent(NULL, FALSE, FALSE, _T("ProducerDone"));
	hEvent[TERMINATE_EVENT] = RtCreateEvent(NULL, FALSE, FALSE, _T("ProducerConsumerExit"));

	// check the size  first ，检查大小
	if( pData->size != sizeof(SHARED_DATA_AREA))
	{
		RtPrintf("SimpleIPCConsumer: Shared memory size is invalid\n");
		RtSetEvent(hEventConsumedDone);
		ExitProcess(0);
	}
	
	// check a text identifier，检查内存区的ID
	if( wcscmp( pData->id, _T("ProducerConsumerSample")) != 0 )
	{
		RtPrintf("SimpleIPCConsumer: Shared ID is invalid\n");
		RtSetEvent(hEventConsumedDone);
		ExitProcess(0);
	}


	// start loop
	while(1)
	{
		// wait for Producer to signal
		dwWaitStatus = RtWaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
		//等待生产者程序的结束消息，来结束本程序
		if (dwWaitStatus == WAIT_OBJECT_0 + TERMINATE_EVENT) 
		{
			RtPrintf("SimpleIPCConsumer: WaitForMultipleObjects TERMINATE_EVENT\n");
			ExitProcess(0);
		}

		if (dwWaitStatus == WAIT_OBJECT_0 + EXECUTE_EVENT)
		{

			// copy data，复制数据到consumerData中，模拟读取数据
			//pData->consumerData[0] = pData->producerData[0];
			memcpy(pData->consumerData, pData->producerData, sizeof(pData->producerData));

			// signal SimpleIPCProducer，发送信号
			RtSetEvent(hEventConsumedDone);
		}
	}  // end while

	RtSetEvent(hEvent[TERMINATE_EVENT]);

	RtPrintf("SimpleIPCConsumer: exit\n");
    ExitProcess(0);
}
