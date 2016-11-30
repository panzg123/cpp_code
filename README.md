C++的学习记录
==============

### utils 工具库

#### **1.list双端链表**

是从`Redis`代码中移植过来的双端链表。

Api Guideline:

+ `list *listCreate(void);`//创建链表
+ `void listRelease(list *list);`//释放链表
+ `list *listAddNodeHead(list *list, void *value);`//链表头部，插入节点
+ `list *listAddNodeTail(list *list, void *value);`//链表尾部，插入节点
+ `void listDelNode(list *list, listNode *node);`//链表，删除节点


#### **2.logger简易日志库**

从[OneValue](https://github.com/onexsoft/OneValue)移植过来，可以根据实际需求来自定义`MsgType`

如果需要高性能的日志库，可以使用 **[spdlog](https://github.com/gabime/spdlog)**

demo:

```
#include "logger.h"
int main()
{
	FileLogger fileLogger;
	if (fileLogger.setFileName("log_test.txt"))
	{
		//default use stdout
		Logger::log(Logger::Message, "Using the log file(%s) output", fileLogger.fileName());
		//use log_test.txt
		Logger::setDefaultLogger(&fileLogger);
		Logger::log(Logger::Error, "Using!!!");
	}
	return 0;
}
```

### boost_demo asio库的实例

### basic  工具代码

### thread_pool 半同步半异步线程池

### RTX 学习代码

### mywebsever 小作业，设计web 服务器

### UdpFileTrans 小作业，UDP文件传输

### SimpleStack 简单web协议栈

### Simple_Fire_Stack 简单防火墙

### Winsock windows网络编程例子

### tsp 用DP来求解TSP问题，适用于小规模节点

用来解了解华为的软件挑战赛的题，然...

### StrVec的简单实现

### 智能指针的简单实现，引用计数法

`smart_ptr.cpp`

### 线程同步方法，互斥锁、信号量、条件变量的封装

`locker.h`

### 模拟压力测试的简单代码

`stress_client.cpp`

### 多路多屏RTSP直播

`16channel_rtsp.c`

这个例子利用多线程做一个16路的分屏多路RTSP流播放，是在雷博士原本的单路代码上修改的。

效果图：

![](http://7xsvsk.com1.z0.glb.clouddn.com/16%E8%B7%AF%E7%9B%B4%E6%92%AD%E6%95%88%E6%9E%9C%E5%9B%BE.jpg?imageMogr2/thumbnail/!25p)
