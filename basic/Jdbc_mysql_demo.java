/*
 * jdbc操作mysql，100W条数据，测速度
 */
package mysql_jdbc;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;

public class Jdbc_mysql_demo {
	public static void main(String[] args) {
		Connection conn = null;
		String sql;
		// MySQL的JDBC URL编写方式：jdbc:mysql://主机名称：连接端口/数据库的名称?参数=值
		// 避免中文乱码要指定useUnicode和characterEncoding
		// 执行数据库操作之前要在数据库管理系统上创建一个数据库，名字自己定，
		// 下面语句之前就要先创建javademo数据库
		String url = "jdbc:mysql://localhost:3307/test?"
				+ "user=root&password=shadow&useUnicode=true&characterEncoding=UTF8";

		try {
			// 之所以要使用下面这条语句，是因为要使用MySQL的驱动，所以我们要把它驱动起来，
			// 可以通过Class.forName把它加载进去，也可以通过初始化来驱动起来，下面三种形式都可以
			Class.forName("com.mysql.jdbc.Driver");// 动态加载mysql驱动
			// or:
			// com.mysql.jdbc.Driver driver = new com.mysql.jdbc.Driver();
			// or：
			// new com.mysql.jdbc.Driver();

			System.out.println("成功加载MySQL驱动程序");
			// 一个Connection代表一个数据库连接
			conn = DriverManager.getConnection(url);
			// Statement里面带有很多方法，比如executeUpdate可以实现插入，更新和删除等
			Statement stmt = conn.createStatement();

			// 计时开始
			long startMili = System.currentTimeMillis();// 当前时间对应的毫秒数
			System.out.println("开始 " + startMili);
			//开始插入
			for (int i = 0; i < 1000000; i++) {
				sql = "insert into person(sex,name,email,phone) values('F','scit','test@test.com','test123')";
				stmt.executeUpdate(sql);
			}
			// 计时结束
			long endMili = System.currentTimeMillis();
			System.out.println("结束 s" + endMili);
			System.out.println("总耗时为：" + (endMili - startMili) + "毫秒");

			conn.close();
			
		} catch (SQLException e) {
			System.out.println("MySQL操作错误");
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
		}

	}
}
