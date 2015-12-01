#include<list>
#include<thread>
#include<functional>
#include<memory>
#include<atomic>
#include"SyncQueue.hpp"

const int MaxTaskCount = 100;

class ThreadPool
{
public:
	//Task模板类，用于定义可调用函数对象，c++11
	using Task = std::function<void()>;
	ThreadPool(int numThreads = std::thread::hardware_concurrency()) :m_queue(MaxTaskCount)
	{
	}
	~ThreadPool(void)
	{
		Stop();
	}
	void Stop()
	{
		//std::call_once 保证StopThreaddGroup函数只被调用一次
		std::call_once(m_flag, [this]{StopThreaddGroup(); });
	}
	void AddTask(Task&& task)
	{
		m_queue.Put(std::forward<Task>(task));
	}
	void AddTask(const Task&task)
	{
		m_queue.Put(task);
	}
	void pool_test()
	{
		ThreadPool pool;
		pool.Start(2);
		std::thread thread1([&pool]{
			for (int i = 0; i < 10; i++)
			{
				auto thdId = this_thread::get_id();
				pool.AddTask([thdId]{
					cout << "同步层线程1的线程 ID：" << thdId << endl;
				});
			}
		});
		std::thread thread2([&pool]{
			for (int i = 0; i < 10; i++)
			{
				auto thdId = this_thread::get_id();
				pool.AddTask([thdId]{
					cout << "同步层线程2的线程 ID：" << thdId << endl;
				});
			}
		});
		this_thread::sleep_for(std::chrono::seconds(2));
		getchar();
		pool.Stop();
		thread1.join();
		thread2.join();
	}
private:
	void Start(int numThreads)
	{
		m_running = true;
		for (int i = 0; i < numThreads; i++)
		{
			//make_shared会自动创建thread对象，并进行初始化
			m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
		}
	}
	void RunInThread()
	{
		while (m_running)
		{
			//取任务分别执行
			std::list<Task> list;
			m_queue.Take(list);
			for (auto & task : list)
			{
				if (!m_running)
					return;
				task();
			}
		}
	}
	void StopThreaddGroup()
	{
		m_queue.Stop();
		m_running = false;

		for (auto thread : m_threadgroup)
		{
			if (thread)
				thread->join();
			//join的作用是等待该子线程完成操作，然后在执行group.clear
		}
		m_threadgroup.clear();
	}
private:
	std::list<std::shared_ptr<std::thread>> m_threadgroup;  /*处理任务的线程组*/
	SyncQueue<Task> m_queue; /*同步队列*/
	atomic_bool m_running;  /*停止标记*/
	std::once_flag m_flag;
};