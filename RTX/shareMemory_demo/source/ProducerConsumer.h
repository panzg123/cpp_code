//////////////////////////////////////////////////////////////////
//
// ProducerConsumer.h 
//
//////////////////////////////////////////////////////////////////

//�궨��
#define EXECUTE_EVENT 0
#define TERMINATE_EVENT 1
//���ݽ���ȥ��С
#define DATA_SIZE 2048*10*20
typedef struct
{
	int		size;
	wchar_t	id[256];//���ַ�
	int		producerData[DATA_SIZE];
	int		consumerData[DATA_SIZE];
}SHARED_DATA_AREA;

