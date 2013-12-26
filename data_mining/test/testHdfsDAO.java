package atrmat;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IOUtils;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.security.proto.SecurityProtos.*;
import org.apache.hadoop.ipc.*;

/**
* the DAO of Hdfs
*/
public class HdfsDAO {

//    private static final String hdfs_ip = "hdfs://192.168.1.28:9000/";
	private static String hdfs_ip;
	private static String core_site_conf;
	private static String hdfs_site_conf;
	private static String mapred_site_conf;
	private static String hadoop_user;
	private static String hadoop_home;
	private static String hdfsPath;
//	private static String propertiesFile;
	private Configuration conf;
	public HdfsDAO() {
    }
    public HdfsDAO(String hdfs, Configuration conf) {
        this.hdfsPath = hdfs;
        this.conf = conf;
    }
    public static String getHdfsPath() {
    	return hdfsPath;
    }
    public static void setHdfsPath(String hdfsPath) {
    	HdfsDAO.hdfsPath = hdfsPath;
    }
    public Configuration getConf() {
    	return conf;
    }
    public void setConf(Configuration conf) {
    	this.conf = conf;
    }
    /**
     *  function: read the hadoop properties from config file
     * */
	public static void readHadoopProperties(String properties) throws IOException {
//		System.out.println(propertiesFile);
		InputStream inputStream = new BufferedInputStream(new FileInputStream(properties));
		//./ represents local project path 
		Properties p = new Properties();   
		try {   
		   p.load(inputStream);   
		} catch (IOException e1) {   
		   e1.printStackTrace();   
		}
		hdfs_ip = p.getProperty("hdfs_ip");
		System.out.println("ip: "+hdfs_ip);
		hadoop_home = p.getProperty("hadoop_home");
		System.out.println("hadoop_home: "+hadoop_home);
//		core_site_conf = hadoop_home+"/"+p.getProperty("core_site");
		core_site_conf = p.getProperty("core_site_conf");
		System.out.println("core_site: "+core_site_conf);
//		hdfs_site_conf = hadoop_home+"/"+p.getProperty("hdfs_site");
		hdfs_site_conf = p.getProperty("hdfs_site_conf");
		System.out.println("hdfs_site: "+hdfs_site_conf);
//		mapred_site_conf = hadoop_home+"/"+p.getProperty("mapred_site");
		mapred_site_conf = p.getProperty("mapred_site_conf");
		System.out.println("mapred_site: "+mapred_site_conf);
		hadoop_user = p.getProperty("hadoop_user");
		System.out.println("hadoop_user: "+hadoop_user);
		hdfsPath = "hdfs://"+hdfs_ip+"/";
	}

	/**
	 *  function: config a hadoop job, using the hadoop config files
	 * */
    public static JobConf config(){
        JobConf conf = new JobConf(HdfsDAO.class);
        conf.setJobName("HdfsDAO");
//        conf.addResource("$HADOOP_HOME/etc/hadoop/core-site.xml");
//        conf.addResource("$HADOOP_HOME/etc/hadoop/hdfs-site.xml");
//        conf.addResource("$HADOOP_HOME/etc/hadoop/mapred-site.xml");
        conf.addResource(hadoop_home+"/"+core_site_conf);
        conf.addResource(hadoop_home+"/"+hdfs_site_conf);
        conf.addResource(hadoop_home+"/"+mapred_site_conf);
        return conf;
    }
	/**
	 *  function: mkdirs in HDFS
	 * */    
    public void mkdirs(String folder) throws IOException {
        Path path = new Path(folder);
        System.out.println(hdfsPath);
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        if (!fs.exists(path)) {
            fs.mkdirs(path);
            System.out.println("Create: " + folder);
        }
        fs.close();
    }
	/**
	 *  function: rmdir in HDFS
	 * */
    public void rmr(String folder) throws IOException, InterruptedException {
        Path path = new Path(folder);
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        fs.deleteOnExit(path);
        System.out.println("Delete: " + folder);
        fs.close();
    }
	/**
	 *  function: list the folder in HDFS
	 * */
    public void ls(String folder) throws IOException {
        Path path = new Path(folder);
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        FileStatus[] list = fs.listStatus(path);
        System.out.println("ls: " + folder);
        System.out.println("==========================================================");
        for (FileStatus f : list) {
            System.out.printf("name: %s, folder: %s, size: %d \n", f.getPath(), f.isDir(), f.getLen());
        }
        System.out.println("==========================================================");
        fs.close();
    }
	/**
	 *  function: create file in HDFS
	 * */
    public void createFile(String file, String content) throws IOException {
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        byte[] buff = content.getBytes();
        FSDataOutputStream os = null;
        try {
            os = fs.create(new Path(file));
            os.write(buff, 0, buff.length);
            System.out.println("Create: " + file);
        } finally {
            if (os != null)
                os.close();
        }
        fs.close();
    }
	/**
	 *  function: copy local file to remote
	 * */
    public void copyFile(String local, String remote) throws IOException, InterruptedException {
    	String user = hadoop_user;
    	System.out.println(hdfsPath);
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        fs.copyFromLocalFile(new Path(local), new Path(remote));
        System.out.println("copy from: " + local + " to " + remote);
        fs.close();
    }
	/**
	 *  function: download remote file to local
	 * */
    public void download(String remote, String local) throws IOException {
        Path path = new Path(remote);
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        fs.copyToLocalFile(path, new Path(local));
        System.out.println("download: from" + remote + " to " + local);
        fs.close();
    }
	/**
	 *  function: cat remote file in HDFS
	 * */   
    public void cat(String remoteFile) throws IOException {
        Path path = new Path(remoteFile);
        FileSystem fs = FileSystem.get(URI.create(hdfsPath), conf);
        FSDataInputStream fsdis = null;
        System.out.println("cat: " + remoteFile);
        try {  
            fsdis =fs.open(path);
            IOUtils.copyBytes(fsdis, System.out, 4096, false);  
          } finally {  
            IOUtils.closeStream(fsdis);
            fs.close();
          }
    }
    public static void main(String[] args) throws IOException, InterruptedException{
//    	System.out.println(args.length);
//    	System.out.println(args);
//    	if(args.length > 1)
//    		propertiesFile = args[1];
    	HdfsDAO hdfs = new HdfsDAO();
    	// config a hadoop job
    	JobConf conf = hdfs.config();
    	// read the config file: hadoop.properties
    	hdfs.readHadoopProperties("./hadoop.properties");
    	hdfs.setConf(conf);
    	// list "/" in HDFS
    	hdfs.ls("/");
    	// mk a test dir
    	hdfs.mkdirs("/mkdir_test");
    	hdfs.ls("/");
    	// rm the test dir
    	hdfs.rmr("/mkdir_test");
    	hdfs.ls("/");
    	// cp local file to HDFS
    	hdfs.copyFile("./hadoop.properties", "/hadoop.properties");
    	hdfs.ls("/");
    	// cat the file in HDFS
    	hdfs.cat("/hadoop.properties");
    	// download the file from HDFS
    	hdfs.download("/hadoop.properties", "/root/hadoop.properties1");
    	// rm file in HDFS
    	hdfs.rmr("/hadoop.properties");
    	hdfs.ls("/");
    }
}
