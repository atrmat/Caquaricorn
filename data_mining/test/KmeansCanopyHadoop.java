package atrmat;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.util.Iterator;

import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.JobConf;
import org.apache.mahout.clustering.Cluster;
import org.apache.mahout.clustering.canopy.CanopyDriver;
import org.apache.mahout.clustering.classify.WeightedVectorWritable;
import org.apache.mahout.clustering.conversion.InputDriver;
import org.apache.mahout.clustering.kmeans.KMeansDriver;
import org.apache.mahout.clustering.kmeans.RandomSeedGenerator;
import org.apache.mahout.common.distance.CosineDistanceMeasure;
import org.apache.mahout.common.distance.DistanceMeasure;
import org.apache.mahout.common.distance.EuclideanDistanceMeasure;
import org.apache.mahout.math.Vector;
import org.apache.mahout.utils.clustering.ClusterDumper;

/**
* kmeansHadoop with canopy to initialize the cluster center for K-means 
*/
public class KmeansHadoop {
//    private static final String HDFS = "hdfs://192.168.1.210:9000";
	private String clusterResultFile = "/clusterResultFile.csv";
	private HdfsDAO hdfs;
	private JobConf conf;
	
	public HdfsDAO getHdfs() {
		return hdfs;
	}

	public void setHdfs(HdfsDAO hdfs) {
		this.hdfs = hdfs;
	}

	public JobConf getConf() {
		return conf;
	}

	public void setConf(JobConf conf) {
		this.conf = conf;
	}

	public KmeansHadoop() throws Exception{
		hdfs = new HdfsDAO();
        conf = hdfs.config();
        hdfs.setConf(conf);
        hdfs.readHadoopProperties("./hadoop.properties");
//        String localFile = "./randomData.csv";
        String localFile = "./datafile";
        String inPath = "/mix_data";
        String seqFile = inPath + "/seqfile";
        String seeds = inPath + "/seeds";
        String outPath = inPath + "/result/";
        String clusteredPoints = outPath + "/clusteredPoints";
        System.out.println(inPath);
        hdfs.rmr(inPath);
        hdfs.rmr(clusterResultFile);
        hdfs.mkdirs(inPath);
		File root = new File(localFile);
		File[] files = root.listFiles();
		for (File file : files) {
			if (file.isDirectory()) {
				continue;
			} else {
				if (file.getName().endsWith(".csv"))
					hdfs.copyFile(file.getAbsolutePath(), inPath);
			}
		}
        
//        hdfs.copyFile(localFile, inPath);
        hdfs.ls(inPath);
        
        /*
        InputDriver.runJob(new Path(inPath), new Path(seqFile), "org.apache.mahout.math.RandomAccessSparseVector");

        int k = 3;
        Path seqFilePath = new Path(seqFile);
        Path clustersSeeds = new Path(seeds);
//        DistanceMeasure measure = new EuclideanDistanceMeasure();
        DistanceMeasure measure = new CosineDistanceMeasure();
        clustersSeeds = RandomSeedGenerator.buildRandom(conf, seqFilePath, clustersSeeds, k, measure);
        KMeansDriver.run(conf, seqFilePath, clustersSeeds, new Path(outPath), measure, 0.01, 10, true, 0.01, false);
        */
        DistanceMeasure measure = new EuclideanDistanceMeasure();
        Path inputPath = new Path(inPath);
        Path outputPath = new Path(outPath);
        Path seqFilePath = new Path(seqFile);
        Path canopyOutput = new Path(outPath, "canopies");
		InputDriver.runJob(inputPath, seqFilePath,"org.apache.mahout.math.RandomAccessSparseVector");
		CanopyDriver.run(conf, seqFilePath, canopyOutput, measure, hdfs.getCanopy_t1(),hdfs.getCanopy_t2(), false, 0.0, false);
		KMeansDriver.run(conf, seqFilePath, new Path(canopyOutput, Cluster.INITIAL_CLUSTERS_DIR + "-final"), 
				outputPath, measure, 0.01, 10, true, 0.0, false);
        //
        Path outGlobPath = new Path(outPath, "clusters-*-final");
        Path clusteredPointsPath = new Path(clusteredPoints);
        
        System.out.printf("Dumping out clusters from clusters: %s and clusteredPoints: %s\n", outGlobPath, clusteredPointsPath);
        
        ClusterDumper clusterDumper = new ClusterDumper(outGlobPath, clusteredPointsPath);
        clusterDumper.printClusters(null);
//        System.out.println("using the display cluster function:");
        displayCluster(clusterDumper);
	}
	
    public static void main(String[] args) throws Exception {
        KmeansHadoop kmeansHadoop = new KmeansHadoop();
    }

    public void displayCluster(ClusterDumper clusterDumper) throws Exception, IOException {
        Iterator<Integer> keys = clusterDumper.getClusterIdToPoints().keySet().iterator();
        FileSystem fs = FileSystem.get(URI.create(this.getHdfs().getHdfsPath()), this.getConf());
        FSDataOutputStream os = null;
        os = fs.create(new Path(clusterResultFile));
        os.writeBytes("Center:\n");
        while (keys.hasNext()) {
            Integer center = keys.next();
            System.out.println("Center:" + center);
            try {
            	os.writeBytes(center+"\n");
            } catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
            for (WeightedVectorWritable point : clusterDumper.getClusterIdToPoints().get(center)) {
                Vector v = point.getVector();
//                System.out.println(v.get(0) + " " + v.get(1));
                os.writeBytes("("+v.get(0) + "," + v.get(1)+") \n");
            }
            os.writeBytes("\n");
        }
        fs.close();
    }
}
