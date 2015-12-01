package panzg;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class Count {
	/**
	 * 返回指定endwith文件后缀名的文件行数
	 * @param file 文件名或目录
	 * @param endsWith 后缀名
	 * @return
	 * @throws IOException
	 */
	public static int getProjectFileNumber(File file, String endsWith) throws IOException {
        int number = 0;
        if (file.exists()) {
            if (file.isDirectory()) {
                for (File subFile : file.listFiles()) {
                    number += getProjectFileNumber(subFile, endsWith);
                }
            } else if (file.isFile() && file.getName().endsWith(endsWith)) {
                BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
                while (br.readLine() != null) {
                    number += 1;
                }
            } else {
                //System.out.println("===" + file.getAbsolutePath());
            }
        }
        return number;
    }
 
    public static void main(String[] args) throws IOException {
        //统计nginx.c代码行数
        int num_c = getProjectFileNumber(new File("E:\\code\\opensource_code\\nginx-1.0.14_comment-master\\nginx-1.0.14_comment-master\\src"), ".h");
        System.out.println("count h:"+num_c);
        //统计nginx.h代码行数
        int num_h = getProjectFileNumber(new File("E:\\code\\opensource_code\\nginx-1.0.14_comment-master\\nginx-1.0.14_comment-master\\src"), ".c");
        System.out.println("count c:"+num_h);
        //nginx 1.0.3源码总数
        System.out.println("count all:"+(num_h+num_c));
    }
}
