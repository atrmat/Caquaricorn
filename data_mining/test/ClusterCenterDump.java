package atrmat;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.Writer;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.mahout.clustering.Cluster;
import org.apache.mahout.common.iterator.sequencefile.PathType;
import org.apache.mahout.common.iterator.sequencefile.SequenceFileDirValueIterable;

import com.google.common.base.Charsets;
import com.google.common.io.Files;

/**
 * just output the center vector to a given file
 * 
 *
 */
public class ClusterCenterDump {
	private Log log=LogFactory.getLog(ClusterCenterDump.class);
	private Configuration conf;
	private Path centerPathDir;
	private String outputPath;
	
	/*public ClusterCenterDump(){}
	public ClusterCenterDump(Configuration conf){
		this.conf=conf;
	}*/
	
	public Configuration getConf() {
		return conf;
	}
	public void setConf(Configuration conf) {
		this.conf = conf;
	}
	public Path getCenterPathDir() {
		return centerPathDir;
	}
	public void setCenterPathDir(Path centerPathDir) {
		this.centerPathDir = centerPathDir;
	}
	/**
	 * @return the outputPath
	 */
	public String getOutputPath() {
		return outputPath;
	}
	/**
	 * @param outputPath the outputPath to set
	 */
	public void setOutputPath(String outputPath) {
		this.outputPath = outputPath;
	}

	public ClusterCenterDump(Configuration conf,String centerPathDir,String outputPath){
		this.conf=conf;
		this.centerPathDir=new Path(centerPathDir);
		this.setOutputPath(outputPath);
	}
	
	/**
	 * write the given cluster center to the given file
	 * @return
	 * @throws FileNotFoundException 
	 */
	public boolean writeCenterToLocal() throws FileNotFoundException{
		if(this.conf==null||this.outputPath==null||this.centerPathDir==null){
			log.info("error:\nshould initial the configuration ,outputPath and centerPath");
			return false;
		}
		Writer writer=null;
		try {
			File outputFile=new File(outputPath);
			writer = Files.newWriter(outputFile, Charsets.UTF_8);
			this.writeTxtCenter(writer, 
				
					new SequenceFileDirValueIterable<Cluster>(new Path(centerPathDir, "part-*"), PathType.GLOB, conf));
	//				new SequenceFileDirValueIterable<Writable>(new Path(centerPathDir, "part-r-00000"), PathType.LIST,
					//		PathFilters.partFilter(),conf));
					writer.flush();
		} catch (IOException e) {
			log.info("write error:\n"+e.getMessage());
			return false;
		}finally{
			try {
				if(writer!=null){
					writer.close();
				}
			} catch (IOException e) {
				log.info("close writer error:\n"+e.getMessage());
			}
		}
		return true;
	}
	
	/**
	 * write the cluster to writer
	 * @param writer
	 * @param cluster
	 * @return
	 * @throws IOException 
	 */
	private boolean writeTxtCenter(Writer writer,Iterable<Cluster> clusters) throws IOException{
		
		for(Cluster cluster:clusters){
			String fmtStr = cluster.asFormatString(null);
			System.out.println("fmtStr:"+fmtStr);
			writer.write(fmtStr);
			writer.write("\n");
		}
		return true;
	}
	

	
}
