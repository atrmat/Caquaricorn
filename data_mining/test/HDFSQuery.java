package atrmat;

import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

public class HDFSQuery {
	private static FileSystem hdfs;

	public static void main(String[] args) throws Exception {
		//1. 创建配置器
		Configuration conf = new Configuration();
		//2. 手动加载配置
		conf.addResource(new Path("H://hadoop-2.2.0/core-site.xml"));
		//3. 创建文件系统
		hdfs = FileSystem.get(URI.create("hdfs://localhost:9000/"),conf);
		//4. 本地文件
		Path src = new Path("H:/intro.csv");
		//5. 目标路径 
		Path dst = new Path("/");
		//6. 上传文件
		if(!hdfs.exists(new Path("/intro.csv"))){
			hdfs.copyFromLocalFile(src, dst);
			System.err.println("文件上传成功至: " + conf.get("fs.default.name"));
		}else{
			System.err.println(conf.get("fs.default.name") + " 中已经存在 intro.csv");
		}
		//7. 遍历HDFS文件
		System.out.println("\nHDFS文件系统中存在的目录和文件：");
		FileStatus[] fs = hdfs.listStatus(dst);
		for (FileStatus f : fs) {
			showDir(f);
		}
	}
	private static void showDir(FileStatus f) throws Exception{
		if(f.isDir()){
			System.err.println("目录:" + f.getPath());
			FileStatus[] listStatus = hdfs.listStatus(f.getPath());
			for(FileStatus fn : listStatus){
				showDir(fn);
			}
		}else{
			System.err.println("文件:" + f.getPath());
		}
	}
}