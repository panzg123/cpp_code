/*
 * 多线程模拟并发，测达梦速度
 */
package dm_jdbc_demo01;

import java.sql.SQLException;

public class My_Thread extends Thread {
	@Override
	public void run() {
		try {
			// 定义类对象
			BasicApp basicApp = new BasicApp();
			// 加载驱动程序
			basicApp.loadJdbcDriver();
			// 连接 DM 数据库
			basicApp.connect();

			// basicApp.insertTable();
			basicApp.insertTableTest();

			basicApp.disConnect();
		}
		// 关闭连接
		catch (SQLException e) {
			System.out.println(e.getMessage());
		}
	}

	public static void main(String args[]) {
		// 计时开始
		long startMili = System.currentTimeMillis();// 当前时间对应的毫秒数
		System.out.println("主线程开始 " + startMili);
		int thread_number =1;
		My_Thread threads[] = new My_Thread[thread_number];
		for (int i = 0; i < thread_number; i++) {
			threads[i] = new My_Thread();
		}
		for (int i = 0; i < thread_number; i++) {
			threads[i].start();
		}
		for (int i = 0; i < thread_number; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		// 计时结束
		long endMili = System.currentTimeMillis();
		System.out.println("主线程结束 s" + endMili);
		System.out.println("主线程总耗时为：" + (endMili - startMili) + "毫秒");
	}
}
