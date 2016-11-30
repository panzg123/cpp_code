/*
��Redis��ֲ������˫������
*/

#ifndef LIST_H
#define LIST_H

//����ڵ㣬test
typedef struct NodeTest{
	char name[10];
	char email[20];
}NodeTest;

//����ڵ�
typedef struct listNode {
	struct listNode *prev;
	struct listNode *next;
	void *value;

}listNode;

//����
typedef struct list {
	listNode *head;
	listNode *tail;
	int len;
} list;

//��������
list *listCreate(void);
//�ͷ�����
void listRelease(list *list);
//����ͷ��������ڵ�
list *listAddNodeHead(list *list, void *value);
//����β��������ڵ�
list *listAddNodeTail(list *list, void *value);
//����ɾ���ڵ�
void listDelNode(list *list, listNode *node);

#endif