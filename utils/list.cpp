#include "list.h"
#include <stdlib.h>

//新建链表
list *listCreate(void)
{
	struct list *list;
	if ((list = (struct list*)malloc(sizeof(*list))) == NULL)
		return NULL;
	list->head = list->tail = NULL;
	list->len = 0;

	return list;
}

//释放链表、节点
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
	// 为节点分配内存
	if ((node = (listNode*)malloc(sizeof(*node))) == NULL)
		return NULL;
	// 保存值指针
	node->value = value;
	// 添加节点到空链表
	if (list->len == 0) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
		// 添加节点到非空链表
	}
	else {
		node->prev = NULL;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}
	// 更新链表节点数
	list->len++;
	return list;
}

list *listAddNodeTail(list *list, void *value)
{
	listNode *node;
	// 为新节点分配内存
	if ((node = (listNode*)malloc(sizeof(*node))) == NULL)
		return NULL;
	// 保存值指针
	node->value = value;
	// 目标链表为空
	if (list->len == 0) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
		// 目标链表非空
	}
	else {
		node->prev = list->tail;
		node->next = NULL;
		list->tail->next = node;
		list->tail = node;
	}
	// 更新链表节点数
	list->len++;
	return list;
}

void listDelNode(list *list, listNode *node)
{
	// 调整前置节点的指针
	if (node->prev)
		node->prev->next = node->next;
	else
		list->head = node->next;
	// 调整后置节点的指针
	if (node->next)
		node->next->prev = node->prev;
	else
		list->tail = node->prev;
	// 释放值
	free(node->value);
	// 释放节点
	free(node);
	// 链表数减一
	list->len--;
}