/**
* 实现功能
* 1.初始化指定个数的线程
* 2.使用链表来管理任务队列
* 3.支持拓展动态线程
* 4.如果闲置线程过多,动态销毁部分线程
*/

/**------------------线程池API函数----------------
* 创建线程池
* thread_pool_t* thread_pool_create(int num);
* 线程的入口函数
* void *thread_excute_route(void *arg);
* 添加任务
* int   thread_pool_add_worker(thread_pool_t *pool,void*(*process)(void *arg),void *arg);
* 如果闲置时间过久则销毁部分线程*
* void *thread_pool_is_need_recovery(void *arg);
* 检测是否需要拓展线程
* void *thread_pool_is_need_extend(void *arg);
* 等待所有的线程结束
void  thread_pool_wait(thread_pool_t *pool);
* 销毁线程池
void  thread_pool_destory(thread_pool_t *pool);
*  显示线程的状态信息
*   void *display_thread(void *arg);
*-----------------------------------------------*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>


/*线程的任务队列由,函数和参数组成,任务由链表来进行管理*/
typedef struct  thread_worker_s{
	void *(*process)(void *arg);  //处理函数
	void *arg;                    //参数
	struct thread_worker_s *next;
}thread_worker_t;

#define bool int
#define true  1
#define false 0

/*线程池中各线程状态描述*/
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

static char* thread_state_map[] = { "创建", "等待任务", "处理中", "处理完成", "已退出" };
/*线程压缩的时候只有 0,1,2,4 状态的线程可以销毁*/


/*线程池管理器*/
#define THREAD_BUSY_PERCENT  0.5   /*线程:任务 = 1:2 值越小，说明任务多,增加线程*/
#define THREAD_IDLE_PERCENT  2     /*线程:任务 = 2:1 值大于1,线程多于任务,销毁部分线程*/

typedef struct thread_pool_s{
	pthread_mutex_t queue_lock;  //队列互斥锁,即涉及到队列修改时需要加锁
	pthread_cond_t  queue_ready; //队列条件锁,队列满足某个条件，触发等待这个条件的线程继续执行,比如说队列满了,队列空了

	thread_worker_t *head;       //任务队列头指针
	bool        is_destroy;      //线程池是否已经销毁
	int num;                     //线程的个数
	int rnum;;                   //正在跑的线程
	int knum;;                   //已杀死的线程
	int queue_size;              //工作队列的大小  
	thread_info_t *threads;      //线程组id,通过pthread_join(thread_ids[0],NULL) 来执行线程
	pthread_t     display;       //打印线程
	pthread_t     destroy;       //定期销毁线程的线程id
	pthread_t     extend;
	float  percent;              //线程个数于任务的比例 rnum/queue_size
	int    init_num;
	pthread_cond_t    extend_ready;       //如果要增加线程
}thread_pool_t;

/*-------------------------函数声明----------------------*/
/**
*  1.初始化互斥变量
*  2.初始化等待变量
*  3.创建指定个数的线程线程
*/
thread_pool_t* thread_pool_create(int num);
void *thread_excute_route(void *arg);


/*调试函数*/
void debug(char *message, int flag){
	if (flag)
		printf("%s\n", message);
}

void *display_thread(void *arg);
/**
*  添加任务包括以下几个操作
*  1.将任务添加到队列末尾
*  2.通知等待进程来处理这个任务 pthread_cond_singal();
*/
int thread_pool_add_worker(thread_pool_t *pool, void*(*process)(void *arg), void *arg);  //网线程池的队列中增加一个需要执行的函数，也就是任务

/**
*  销毁线程池,包括以下几个部分
*  1.通知所有等待的进程 pthread_cond_broadcase
*  2.等待所有的线程执行完
*  3.销毁任务列表
*  4.释放锁，释放条件
*  4.销毁线程池对象
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
	/*初始化互斥变量与条件变量*/
	pthread_mutex_init(&(p->queue_lock), NULL);
	pthread_cond_init(&(p->queue_ready), NULL);

	/*设置线程个数*/
	p->num = num;
	p->rnum = num;
	p->knum = 0;

	p->head = NULL;
	p->queue_size = 0;
	p->is_destroy = false;


	int i = 0;
	thread_info_t *tmp = NULL;
	for (i = 0; i<num; i++){
		/*创建线程*/
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

	/*显示*/
	pthread_create(&(p->display), NULL, display_thread, p);
	/*检测是否需要动态线程*/
	//pthread_create(&(p->extend),NULL,thread_pool_is_need_extend,p);
	/*动态销毁*/
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
	/*销毁任务列表*/
	while (p->head){
		member = p->head;
		p->head = member->next;
		free(member);
	}
	/*销毁线程列表*/
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
/*通过线程id,找到对应的线程*/
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


/*每个线程入口函数*/
void *thread_excute_route(void *arg){
	thread_worker_t *worker = NULL;
	thread_info_t   *thread = NULL;
	thread_pool_t*   p = (thread_pool_t*)arg;
	//printf("thread %lld create success\n",pthread_self());
	while (1){
		pthread_mutex_lock(&(p->queue_lock));

		/*获取当前线程的id*/
		pthread_t  pthread_id = pthread_self();
		/*设置当前状态*/
		thread = get_thread_by_id(p, pthread_id);

		/*线程池被销毁,并且没有任务了*/
		if (p->is_destroy == true && p->queue_size == 0){
			pthread_mutex_unlock(&(p->queue_lock));
			thread->state = THREAD_STATE_EXIT;
			p->knum++;
			p->rnum--;
			pthread_exit(NULL);
		}
		if (thread){
			thread->state = THREAD_STATE_TASK_WAITING;  /*线程正在等待任务*/
		}
		/*线程池没有被销毁,没有任务到来就一直等待*/
		while (p->queue_size == 0 && !p->is_destroy){
			pthread_cond_wait(&(p->queue_ready), &(p->queue_lock));
		}
		p->queue_size--;
		worker = p->head;
		p->head = worker->next;
		pthread_mutex_unlock(&(p->queue_lock));


		if (thread)
			thread->state = THREAD_STATE_TASK_PROCESSING;  /*线程正在执行任务*/
		(*(worker->process))(worker->arg);
		if (thread)
			thread->state = THREAD_STATE_TASK_FINISHED;    /*任务执行完成*/
		free(worker);
		worker = NULL;
	}
}



/*拓展线程*/
void *thread_pool_is_need_extend(void *arg){
	thread_pool_t *p = (thread_pool_t *)arg;
	thread_pool_t *pool = p;
	/*判断是否需要增加线程,最终目的 线程:任务=1:2*/
	if (p->queue_size>100){
		int incr = 0;
		if (((float)p->rnum / p->queue_size) < THREAD_BUSY_PERCENT){
			incr = (p->queue_size*THREAD_BUSY_PERCENT) - p->rnum;  /*计算需要增加线程个数*/
			int i = 0;
			thread_info_t *tmp = NULL;
			thread_pool_t *p = pool;
			pthread_mutex_lock(&pool->queue_lock);
			if (p->queue_size<100){
				pthread_mutex_unlock(&pool->queue_lock);
				return;
			}
			for (i = 0; i<incr; i++){
				/*创建线程*/
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
/*恢复初始线程个数*/
void *thread_pool_is_need_recovery(void *arg){
	thread_pool_t *pool = (thread_pool_t *)arg;
	int i = 0;
	thread_info_t *tmp = NULL, *prev = NULL, *p1 = NULL;
	/*如果没有任务了,当前线程大于初始化的线程个数*/
	while (1){
		i = 0;
		if (pool->queue_size == 0 && pool->rnum > pool->init_num){
			sleep(5);
			/*5s秒内还是这个状态的话就,销毁部分线程*/
			if (pool->queue_size == 0 && pool->rnum > pool->init_num){
				pthread_mutex_lock(&pool->queue_lock);
				tmp = pool->threads;
				while ((pool->rnum != pool->init_num) && tmp){
					/*找到空闲的线程*/
					if (tmp->state != THREAD_STATE_TASK_PROCESSING){
						i++;
						if (prev)
							prev->next = tmp->next;
						else
							pool->threads = tmp->next;
						pool->rnum--;  /*正在运行的线程减一*/
						pool->knum++;  /*销毁的线程加一*/
						kill(tmp->id, SIGKILL); /*销毁线程*/
						p1 = tmp;
						tmp = tmp->next;
						free(p1);
						continue;
					}
					prev = tmp;
					tmp = tmp->next;
				}
				pthread_mutex_unlock(&pool->queue_lock);
				printf("5s内没有新任务销毁部分线程,销毁了 %d 个线程\n", i);
			}
		}
		sleep(5);
	}
}



/*打印一些信息的*/
void *display_thread(void *arg){
	thread_pool_t *p = (thread_pool_t *)arg;
	thread_info_t *thread = NULL;
	int i = 0;
	while (1){
		printf("threads %d,running %d,killed %d\n", p->num, p->rnum, p->knum);    /*线程总数,正在跑的,已销毁的*/
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
	/*添加任务*/
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