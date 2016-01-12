//////////////////////////////////////////////////////////////////
//
// ProducerConsumer.h 
//
//////////////////////////////////////////////////////////////////

//宏定义
#define EXECUTE_EVENT 0
#define TERMINATE_EVENT 1
//数据交换去大小
#define DATA_SIZE 2048*10*20
typedef struct
{
	int		size;
	wchar_t	id[256];//宽字符
	int		producerData[DATA_SIZE];
	int		consumerData[DATA_SIZE];
}SHARED_DATA_AREA;

