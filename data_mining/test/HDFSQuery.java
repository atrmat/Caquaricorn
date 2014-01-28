package atrmat;

import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

public class HDFSQuery {
	private static FileSystem hdfs;

	public static void main(String[] args) throws Exception {
		//1. ����������
		Configuration conf = new Configuration();
		//2. �ֶ���������
		conf.addResource(new Path("H://hadoop-2.2.0/core-site.xml"));
		//3. �����ļ�ϵͳ
		hdfs = FileSystem.get(URI.create("hdfs://localhost:9000/"),conf);
		//4. �����ļ�
		Path src = new Path("H:/intro.csv");
		//5. Ŀ��·�� 
		Path dst = new Path("/");
		//6. �ϴ��ļ�
		if(!hdfs.exists(new Path("/intro.csv"))){
			hdfs.copyFromLocalFile(src, dst);
			System.err.println("�ļ��ϴ��ɹ���: " + conf.get("fs.default.name"));
		}else{
			System.err.println(conf.get("fs.default.name") + " ���Ѿ����� intro.csv");
		}
		//7. ����HDFS�ļ�
		System.out.println("\nHDFS�ļ�ϵͳ�д��ڵ�Ŀ¼���ļ���");
		FileStatus[] fs = hdfs.listStatus(dst);
		for (FileStatus f : fs) {
			showDir(f);
		}
	}
	private static void showDir(FileStatus f) throws Exception{
		if(f.isDir()){
			System.err.println("Ŀ¼:" + f.getPath());
			FileStatus[] listStatus = hdfs.listStatus(f.getPath());
			for(FileStatus fn : listStatus){
				showDir(fn);
			}
		}else{
			System.err.println("�ļ�:" + f.getPath());
		}
	}
}