/**
* ʵ�ֹ���
* 1.��ʼ��ָ���������߳�
* 2.ʹ�������������������
* 3.֧����չ��̬�߳�
* 4.��������̹߳���,��̬���ٲ����߳�
*/

/**------------------�̳߳�API����----------------
* �����̳߳�
* thread_pool_t* thread_pool_create(int num);
* �̵߳���ں���
* void *thread_excute_route(void *arg);
* �������
* int   thread_pool_add_worker(thread_pool_t *pool,void*(*process)(void *arg),void *arg);
* �������ʱ����������ٲ����߳�*
* void *thread_pool_is_need_recovery(void *arg);
* ����Ƿ���Ҫ��չ�߳�
* void *thread_pool_is_need_extend(void *arg);
* �ȴ����е��߳̽���
void  thread_pool_wait(thread_pool_t *pool);
* �����̳߳�
void  thread_pool_destory(thread_pool_t *pool);
*  ��ʾ�̵߳�״̬��Ϣ
*   void *display_thread(void *arg);
*-----------------------------------------------*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>


/*�̵߳����������,�����Ͳ������,���������������й���*/
typedef struct  thread_worker_s{
	void *(*process)(void *arg);  //������
	void *arg;                    //����
	struct thread_worker_s *next;
}thread_worker_t;

#define bool int
#define true  1
#define false 0

/*�̳߳��и��߳�״̬����*/
#define THREAD_STATE_RUN               0
#define THREAD_STATE_TASK_WAITING      1
#define THREAD_STATE_TASK_PROCESSING   2
#define THREAD_STATE_TASK_FINISHED     3
#define THREAD_STATE_EXIT              4     


typedef struct thread_info_s{
	pthread_t id;
	int       state;
	struct  thread_info_s *next;
}thread_info_t;

static char* thread_state_map[] = { "����", "�ȴ�����", "������", "�������", "���˳�" };
/*�߳�ѹ����ʱ��ֻ�� 0,1,2,4 ״̬���߳̿�������*/


/*�̳߳ع�����*/
#define THREAD_BUSY_PERCENT  0.5   /*�߳�:���� = 1:2 ֵԽС��˵�������,�����߳�*/
#define THREAD_IDLE_PERCENT  2     /*�߳�:���� = 2:1 ֵ����1,�̶߳�������,���ٲ����߳�*/

typedef struct thread_pool_s{
	pthread_mutex_t queue_lock;  //���л�����,���漰�������޸�ʱ��Ҫ����
	pthread_cond_t  queue_ready; //����������,��������ĳ�������������ȴ�����������̼߳���ִ��,����˵��������,���п���

	thread_worker_t *head;       //�������ͷָ��
	bool        is_destroy;      //�̳߳��Ƿ��Ѿ�����
	int num;                     //�̵߳ĸ���
	int rnum;;                   //�����ܵ��߳�
	int knum;;                   //��ɱ�����߳�
	int queue_size;              //�������еĴ�С  
	thread_info_t *threads;      //�߳���id,ͨ��pthread_join(thread_ids[0],NULL) ��ִ���߳�
	pthread_t     display;       //��ӡ�߳�
	pthread_t     destroy;       //���������̵߳��߳�id
	pthread_t     extend;
	float  percent;              //�̸߳���������ı��� rnum/queue_size
	int    init_num;
	pthread_cond_t    extend_ready;       //���Ҫ�����߳�
}thread_pool_t;

/*-------------------------��������----------------------*/
/**
*  1.��ʼ���������
*  2.��ʼ���ȴ�����
*  3.����ָ���������߳��߳�
*/
thread_pool_t* thread_pool_create(int num);
void *thread_excute_route(void *arg);


/*���Ժ���*/
void debug(char *message, int flag){
	if (flag)
		printf("%s\n", message);
}

void *display_thread(void *arg);
/**
*  �������������¼�������
*  1.��������ӵ�����ĩβ
*  2.֪ͨ�ȴ������������������ pthread_cond_singal();
*/
int thread_pool_add_worker(thread_pool_t *pool, void*(*process)(void *arg), void *arg);  //���̳߳صĶ���������һ����Ҫִ�еĺ�����Ҳ��������

/**
*  �����̳߳�,�������¼�������
*  1.֪ͨ���еȴ��Ľ��� pthread_cond_broadcase
*  2.�ȴ����е��߳�ִ����
*  3.���������б�
*  4.�ͷ������ͷ�����
*  4.�����̳߳ض���
*/



void *thread_pool_is_need_recovery(void *arg);
void *thread_pool_is_need_extend(void *arg);
void thread_pool_destory(thread_pool_t *pool);


thread_pool_t *thread_pool_create(int num){
	if (num<1){
		return NULL;
	}
	thread_pool_t *p;
	p = (thread_pool_t*)malloc(sizeof(struct thread_pool_s));
	if (p == NULL)
		return NULL;
	p->init_num = num;
	/*��ʼ�������������������*/
	pthread_mutex_init(&(p->queue_lock), NULL);
	pthread_cond_init(&(p->queue_ready), NULL);

	/*�����̸߳���*/
	p->num = num;
	p->rnum = num;
	p->knum = 0;

	p->head = NULL;
	p->queue_size = 0;
	p->is_destroy = false;


	int i = 0;
	thread_info_t *tmp = NULL;
	for (i = 0; i<num; i++){
		/*�����߳�*/
		tmp = (struct thread_info_s*)malloc(sizeof(struct thread_info_s));
		if (tmp == NULL){
			free(p);
			return NULL;
		}
		else{
			tmp->next = p->threads;
			p->threads = tmp;
		}
		pthread_create(&(tmp->id), NULL, thread_excute_route, p);
		tmp->state = THREAD_STATE_RUN;
	}

	/*��ʾ*/
	pthread_create(&(p->display), NULL, display_thread, p);
	/*����Ƿ���Ҫ��̬�߳�*/
	//pthread_create(&(p->extend),NULL,thread_pool_is_need_extend,p);
	/*��̬����*/
	pthread_create(&(p->destroy), NULL, thread_pool_is_need_recovery, p);
	return p;
}

int thread_pool_add_worker(thread_pool_t *pool, void*(*process)(void*arg), void*arg){
	thread_pool_t *p = pool;
	thread_worker_t *worker = NULL, *member = NULL;
	worker = (thread_worker_t*)malloc(sizeof(struct thread_worker_s));
	int incr = 0;
	if (worker == NULL){
		return -1;
	}
	worker->process = process;
	worker->arg = arg;
	worker->next = NULL;
	thread_pool_is_need_extend(pool);
	pthread_mutex_lock(&(p->queue_lock));
	member = p->head;
	if (member != NULL){
		while (member->next != NULL)
			member = member->next;
		member->next = worker;
	}
	else{
		p->head = worker;
	}
	p->queue_size++;
	pthread_mutex_unlock(&(p->queue_lock));
	pthread_cond_signal(&(p->queue_ready));
	return 1;
}


void thread_pool_wait(thread_pool_t *pool){
	thread_info_t *thread;
	int i = 0;
	for (i = 0; i<pool->num; i++){
		thread = (thread_info_t*)(pool->threads + i);
		thread->state = THREAD_STATE_EXIT;
		pthread_join(thread->id, NULL);
	}
}
void thread_pool_destory(thread_pool_t *pool){
	thread_pool_t   *p = pool;
	thread_worker_t *member = NULL;

	if (p->is_destroy)
		return;
	p->is_destroy = true;
	pthread_cond_broadcast(&(p->queue_ready));
	thread_pool_wait(pool);
	free(p->threads);
	p->threads = NULL;
	/*���������б�*/
	while (p->head){
		member = p->head;
		p->head = member->next;
		free(member);
	}
	/*�����߳��б�*/
	thread_info_t *tmp = NULL;
	while (p->threads){
		tmp = p->threads;
		p->threads = tmp->next;
		free(tmp);
	}

	pthread_mutex_destroy(&(p->queue_lock));
	pthread_cond_destroy(&(p->queue_ready));
	return;
}
/*ͨ���߳�id,�ҵ���Ӧ���߳�*/
thread_info_t *get_thread_by_id(thread_pool_t *pool, pthread_t id){
	thread_info_t *thread = NULL;
	thread_info_t *p = pool->threads;
	while (p != NULL){
		if (p->id == id)
			return p;
		p = p->next;
	}
	return NULL;
}


/*ÿ���߳���ں���*/
void *thread_excute_route(void *arg){
	thread_worker_t *worker = NULL;
	thread_info_t   *thread = NULL;
	thread_pool_t*   p = (thread_pool_t*)arg;
	//printf("thread %lld create success\n",pthread_self());
	while (1){
		pthread_mutex_lock(&(p->queue_lock));

		/*��ȡ��ǰ�̵߳�id*/
		pthread_t  pthread_id = pthread_self();
		/*���õ�ǰ״̬*/
		thread = get_thread_by_id(p, pthread_id);

		/*�̳߳ر�����,����û��������*/
		if (p->is_destroy == true && p->queue_size == 0){
			pthread_mutex_unlock(&(p->queue_lock));
			thread->state = THREAD_STATE_EXIT;
			p->knum++;
			p->rnum--;
			pthread_exit(NULL);
		}
		if (thread){
			thread->state = THREAD_STATE_TASK_WAITING;  /*�߳����ڵȴ�����*/
		}
		/*�̳߳�û�б�����,û����������һֱ�ȴ�*/
		while (p->queue_size == 0 && !p->is_destroy){
			pthread_cond_wait(&(p->queue_ready), &(p->queue_lock));
		}
		p->queue_size--;
		worker = p->head;
		p->head = worker->next;
		pthread_mutex_unlock(&(p->queue_lock));


		if (thread)
			thread->state = THREAD_STATE_TASK_PROCESSING;  /*�߳�����ִ������*/
		(*(worker->process))(worker->arg);
		if (thread)
			thread->state = THREAD_STATE_TASK_FINISHED;    /*����ִ�����*/
		free(worker);
		worker = NULL;
	}
}



/*��չ�߳�*/
void *thread_pool_is_need_extend(void *arg){
	thread_pool_t *p = (thread_pool_t *)arg;
	thread_pool_t *pool = p;
	/*�ж��Ƿ���Ҫ�����߳�,����Ŀ�� �߳�:����=1:2*/
	if (p->queue_size>100){
		int incr = 0;
		if (((float)p->rnum / p->queue_size) < THREAD_BUSY_PERCENT){
			incr = (p->queue_size*THREAD_BUSY_PERCENT) - p->rnum;  /*������Ҫ�����̸߳���*/
			int i = 0;
			thread_info_t *tmp = NULL;
			thread_pool_t *p = pool;
			pthread_mutex_lock(&pool->queue_lock);
			if (p->queue_size<100){
				pthread_mutex_unlock(&pool->queue_lock);
				return;
			}
			for (i = 0; i<incr; i++){
				/*�����߳�*/
				tmp = (struct thread_info_s*)malloc(sizeof(struct thread_info_s));
				if (tmp == NULL){
					continue;
				}
				else{
					tmp->next = p->threads;
					p->threads = tmp;
				}
				p->num++;
				p->rnum++;
				pthread_create(&(tmp->id), NULL, thread_excute_route, p);
				tmp->state = THREAD_STATE_RUN;
			}
			pthread_mutex_unlock(&pool->queue_lock);
		}
	}
	//pthread_cond_signal(&pool->extend_ready);
}
pthread_cond_t  sum_ready;
/*�ָ���ʼ�̸߳���*/
void *thread_pool_is_need_recovery(void *arg){
	thread_pool_t *pool = (thread_pool_t *)arg;
	int i = 0;
	thread_info_t *tmp = NULL, *prev = NULL, *p1 = NULL;
	/*���û��������,��ǰ�̴߳��ڳ�ʼ�����̸߳���*/
	while (1){
		i = 0;
		if (pool->queue_size == 0 && pool->rnum > pool->init_num){
			sleep(5);
			/*5s���ڻ������״̬�Ļ���,���ٲ����߳�*/
			if (pool->queue_size == 0 && pool->rnum > pool->init_num){
				pthread_mutex_lock(&pool->queue_lock);
				tmp = pool->threads;
				while ((pool->rnum != pool->init_num) && tmp){
					/*�ҵ����е��߳�*/
					if (tmp->state != THREAD_STATE_TASK_PROCESSING){
						i++;
						if (prev)
							prev->next = tmp->next;
						else
							pool->threads = tmp->next;
						pool->rnum--;  /*�������е��̼߳�һ*/
						pool->knum++;  /*���ٵ��̼߳�һ*/
						kill(tmp->id, SIGKILL); /*�����߳�*/
						p1 = tmp;
						tmp = tmp->next;
						free(p1);
						continue;
					}
					prev = tmp;
					tmp = tmp->next;
				}
				pthread_mutex_unlock(&pool->queue_lock);
				printf("5s��û�����������ٲ����߳�,������ %d ���߳�\n", i);
			}
		}
		sleep(5);
	}
}



/*��ӡһЩ��Ϣ��*/
void *display_thread(void *arg){
	thread_pool_t *p = (thread_pool_t *)arg;
	thread_info_t *thread = NULL;
	int i = 0;
	while (1){
		printf("threads %d,running %d,killed %d\n", p->num, p->rnum, p->knum);    /*�߳�����,�����ܵ�,�����ٵ�*/
		thread = p->threads;
		while (thread){
			printf("id=%ld,state=%s\n", thread->id, thread_state_map[thread->state]);
			thread = thread->next;
		}
		sleep(5);
	}
}

/*-----------------demo------------------------*/
struct param{
	int id;
};
void *demo(void *arg){
	struct param *p = (struct param*)arg;
	printf("thread[%ld] excute task %d\n", pthread_self(), p->id);
}

int main(int argc, char **argv){
	thread_pool_t  *thread_pool = NULL;
	thread_pool = thread_pool_create(10);
	/*�������*/
	int i = 0;
	struct param *p = NULL;
	for (i = 0; i < 10000; i++){
		p = (struct param*) malloc(sizeof(struct param));
		p->id = i;
		thread_pool_add_worker(thread_pool, demo, (void*)p);
		//free(p);
	}
	thread_pool_wait(thread_pool);
	//thread_pool_destory(thread_pool);
	return 0;
}