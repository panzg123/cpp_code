/*
 * jdbc操作达梦数据库，测速度
 */
package dm_jdbc_demo01;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Statement;

public class BasicApp {
	// 定义 DM JDBC 驱动串
	String jdbcString = "dm.jdbc.driver.DmDriver";
	// 定义 DM URL 连接串
	String urlString = "jdbc:dm://localhost:5236";
	// 定义连接用户名
	String userName = "SYSDBA";
	// 定义连接用户口令
	String password = "SYSDBA";
	// 定义连接对象
	Connection conn = null;

	/*
	 * 加载 JDBC 驱动程序
	 * 
	 * @throws SQLException 异常
	 */
	public void loadJdbcDriver() throws SQLException {
		try {
			System.out.println("Loading JDBC Driver...");
			// 加载 JDBC 驱动程序
			Class.forName(jdbcString);
		} catch (ClassNotFoundException e) {
			throw new SQLException("Load JDBC Driver Error : " + e.getMessage());
		} catch (Exception ex) {
			throw new SQLException("Load JDBC Driver Error : " + ex.getMessage());
		}
	}

	/*
	 * 连接 DM 数据库
	 * 
	 * @throws SQLException 异常
	 */
	public void connect() throws SQLException {
		try {
			System.out.println("Connecting to DM Server...");
			// 连接 DM 数据库
			conn = DriverManager.getConnection(urlString, userName, password);
		} catch (SQLException e) {
			throw new SQLException("Connect to DM Server Error : " + e.getMessage());
		}
	}

	/*
	 * 关闭连接
	 * 
	 * @throws SQLException 异常
	 */
	public void disConnect() throws SQLException {
		try {
			// 关闭连接
			System.out.println("关闭连接");
			conn.close();
		} catch (SQLException e) {
			throw new SQLException("close connection error : " + e.getMessage());
		}
	}

	/*
	 * 往产品信息表插入数据
	 * 
	 * @throws SQLException 异常
	 */
	public void insertTable() throws SQLException {
		// 创建语句对象
		Statement stmt = conn.createStatement();
		// 插入数据语句
		for (int i = 0; i < 1000000; i++) {
			String sql = "INSERT INTO person.person(sex,name,email,phone) VALUES('F','scit','test@test.com','test123');";
			stmt.executeQuery(sql);
		}
		// 关闭语句
		stmt.close();
	}

	public void insertTableTest() throws SQLException {
		String txtContent = txt2String("G:\\non_git_code\\dm_jdbc_demo01\\src\\test.txt");

		// 计时开始
		long startMili = System.currentTimeMillis();// 当前时间对应的毫秒数
		System.out.println("开始 " + startMili);
		String sql = "INSERT INTO TEST.TEST(col1,col2,col3,col4,col5,col6,col7,col8,col9,col10) values(?,?,?,?,?,?,?,?,?,?);";
		for (int i = 0; i < 100; i++) {
			PreparedStatement pstmt = conn.prepareStatement(sql);
			pstmt.setString(1, txtContent);
			pstmt.setString(2, txtContent);
			pstmt.setString(3, txtContent);
			pstmt.setString(4, txtContent);
			pstmt.setString(5, txtContent);
			pstmt.setString(6, txtContent);
			pstmt.setString(7, txtContent);
			pstmt.setString(8, txtContent);
			pstmt.setString(9, txtContent);
			pstmt.setString(10, txtContent);
			pstmt.executeUpdate();
			pstmt.close();
		}
		// 计时结束
		long endMili = System.currentTimeMillis();
		System.out.println("结束 s" + endMili);
		System.out.println("总耗时为：" + (endMili - startMili) + "毫秒");
	}

	/*
	 * 查询产品信息表
	 * 
	 * @throws SQLException 异常
	 */
	public void queryTable() throws SQLException {
		// 创建语句对象
		Statement stmt = conn.createStatement();

		// 查询语句
		String sql = "SELECT count(*) from PERSON.ADDRESS";
		// 执行查询
		stmt.executeQuery(sql);
		// 关闭语句
		stmt.close();
	}

	/**
	 * 读取txt文件的内容
	 * 
	 * @param filePath 文件路径
	 * @return 返回文件内容
	 */
	public static String txt2String(String filePath) {
		String result = "";
		try {
			String encoding = "utf-8"; //编码格式
			File file = new File(filePath);
			if (file.isFile() && file.exists()) { // 判断文件是否存在
				InputStreamReader read = new InputStreamReader(new FileInputStream(file), encoding);// 考虑到编码格式
				BufferedReader bufferedReader = new BufferedReader(read);
				String lineTxt = null;
				while ((lineTxt = bufferedReader.readLine()) != null) {
					result += lineTxt; //追加文件内容到字符串
				}
				read.close();
			} else {
				System.out.println("找不到指定的文件");
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return result; //返回字符串
	}

	/*
	 * 类主方法 @param args 参数
	 */
	public static void main(String args[]) {
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
}
