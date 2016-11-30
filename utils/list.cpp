#include "list.h"
#include <stdlib.h>

//�½�����
list *listCreate(void)
{
	struct list *list;
	if ((list = (struct list*)malloc(sizeof(*list))) == NULL)
		return NULL;
	list->head = list->tail = NULL;
	list->len = 0;

	return list;
}

//�ͷ������ڵ�
void listRelease(list *list)
{
	unsigned long len;
	listNode *current, *next;

	current = list->head;
	len = list->len;
	while (len--) {
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
	free(list);
}

list *listAddNodeHead(list *list, void *value)
{
	listNode *node;
	// Ϊ�ڵ�����ڴ�
	if ((node = (listNode*)malloc(sizeof(*node))) == NULL)
		return NULL;
	// ����ֵָ��
	node->value = value;
	// ��ӽڵ㵽������
	if (list->len == 0) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
		// ��ӽڵ㵽�ǿ�����
	}
	else {
		node->prev = NULL;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}
	// ��������ڵ���
	list->len++;
	return list;
}

list *listAddNodeTail(list *list, void *value)
{
	listNode *node;
	// Ϊ�½ڵ�����ڴ�
	if ((node = (listNode*)malloc(sizeof(*node))) == NULL)
		return NULL;
	// ����ֵָ��
	node->value = value;
	// Ŀ������Ϊ��
	if (list->len == 0) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
		// Ŀ������ǿ�
	}
	else {
		node->prev = list->tail;
		node->next = NULL;
		list->tail->next = node;
		list->tail = node;
	}
	// ��������ڵ���
	list->len++;
	return list;
}

void listDelNode(list *list, listNode *node)
{
	// ����ǰ�ýڵ��ָ��
	if (node->prev)
		node->prev->next = node->next;
	else
		list->head = node->next;
	// �������ýڵ��ָ��
	if (node->next)
		node->next->prev = node->prev;
	else
		list->tail = node->prev;
	// �ͷ�ֵ
	free(node->value);
	// �ͷŽڵ�
	free(node);
	// ��������һ
	list->len--;
}