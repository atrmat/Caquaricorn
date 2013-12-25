package atrmat;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

/**
* parse the property file, get the config of hadoop.properties
* generate jar in Eclipse, 
* then execute it by: java -jar {Path to readHadoopProperties.jar}
*/
public class readHadoopProperties {

	public readHadoopProperties() throws IOException {
		// TODO Auto-generated constructor stub
//		InputStream inputStream = this.getClass().getClassLoader().getResourceAsStream("hadoop.properties");
		InputStream inputStream = new BufferedInputStream(new FileInputStream("./hadoop.properties"));//./代表当前目录即工程的根目录 
		  Properties p = new Properties();   
		  try {   
		   p.load(inputStream);   
		  } catch (IOException e1) {   
		   e1.printStackTrace();   
		  }   
		System.out.println("ip:"+p.getProperty("hdfs-ip"));  
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			new readHadoopProperties();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
