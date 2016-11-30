/*
从Redis移植过来的双端链表
*/

#ifndef LIST_H
#define LIST_H

//链表节点，test
typedef struct NodeTest{
	char name[10];
	char email[20];
}NodeTest;

//链表节点
typedef struct listNode {
	struct listNode *prev;
	struct listNode *next;
	void *value;

}listNode;

//链表
typedef struct list {
	listNode *head;
	listNode *tail;
	int len;
} list;

//创建链表
list *listCreate(void);
//释放链表
void listRelease(list *list);
//链表头部，插入节点
list *listAddNodeHead(list *list, void *value);
//链表尾部，插入节点
list *listAddNodeTail(list *list, void *value);
//链表，删除节点
void listDelNode(list *list, listNode *node);

#endif