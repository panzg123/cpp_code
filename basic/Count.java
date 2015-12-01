package panzg;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class Count {
	/**
	 * ����ָ��endwith�ļ���׺�����ļ�����
	 * @param file �ļ�����Ŀ¼
	 * @param endsWith ��׺��
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
        //ͳ��nginx.c��������
        int num_c = getProjectFileNumber(new File("E:\\code\\opensource_code\\nginx-1.0.14_comment-master\\nginx-1.0.14_comment-master\\src"), ".h");
        System.out.println("count h:"+num_c);
        //ͳ��nginx.h��������
        int num_h = getProjectFileNumber(new File("E:\\code\\opensource_code\\nginx-1.0.14_comment-master\\nginx-1.0.14_comment-master\\src"), ".c");
        System.out.println("count c:"+num_h);
        //nginx 1.0.3Դ������
        System.out.println("count all:"+(num_h+num_c));
    }
}
