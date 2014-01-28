package atrmat;

import java.io.File;
import java.util.ArrayList;

public class FileList {
	private static String datafile = "H://WLSVM";
	private static ArrayList<String> filelist = new ArrayList<String>();

	public static void main(String[] args) throws Exception {
		getFiles(datafile);
	}
	/*
	 * recursive to list all files under this directory
	 */
	static void getFiles(String filePath) {
		File root = new File(filePath);
		File[] files = root.listFiles();
		for (File file : files) {
			if (file.isDirectory()) {
				getFiles(file.getAbsolutePath());
				filelist.add(file.getAbsolutePath());
				System.out.println("��ʾ" + filePath + "��������Ŀ¼�����ļ�"
						+ file.getAbsolutePath());
			} else {
				System.out.println("��ʾ" + filePath + "��������Ŀ¼"
						+ file.getAbsolutePath());
			}
		}
	}
}
