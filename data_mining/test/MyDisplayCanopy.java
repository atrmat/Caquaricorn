package atrmat;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.mahout.clustering.Cluster;
import org.apache.mahout.clustering.canopy.CanopyDriver;
import org.apache.mahout.clustering.display.DisplayClustering;
import org.apache.mahout.common.HadoopUtil;
import org.apache.mahout.common.RandomUtils;
import org.apache.mahout.common.distance.ManhattanDistanceMeasure;
import org.apache.mahout.math.DenseVector;

/**
 * Java desktop graphics class that runs canopy clustering and displays the
 * results. This class generates random data and clusters it.
 */
public class MyDisplayCanopy extends DisplayClustering {

	static final Color[] COLORS = { Color.red, Color.orange, Color.yellow,
			Color.green, Color.blue, Color.magenta, Color.lightGray };
	static double significance = 0.05;
	
	MyDisplayCanopy() {
		initialize();
		this.setTitle("Canopy Clusters (>" + (int) (significance * 100)
				+ "% of population)");
	}

	@Override
	public void paint(Graphics g) {
		plotSampleData((Graphics2D) g);
		plotClusters((Graphics2D) g);
	}

	protected static void plotClusters(Graphics2D g2) {
		int cx = CLUSTERS.size() - 1;
		for (List<Cluster> clusters : CLUSTERS) {
			for (Cluster cluster : clusters) {
				if (isSignificant(cluster)) {
					g2.setStroke(new BasicStroke(1));
					g2.setColor(Color.BLUE);
					double[] t1 = { T1, T1 };
					plotEllipse(g2, cluster.getCenter(), new DenseVector(t1));
					double[] t2 = { T2, T2 };
					plotEllipse(g2, cluster.getCenter(), new DenseVector(t2));
					g2.setColor(COLORS[Math.min(
							COLORS.length - 1, cx)]);
					g2.setStroke(new BasicStroke(cx == 0 ? 3 : 1));
					plotEllipse(g2, cluster.getCenter(), cluster.getRadius()
							.times(3));
				}
			}
			cx--;
		}
	}

	public static void main(String[] args) throws Exception {
		Path samples = new Path("samples");
		Path output = new Path("Canopy cluster");
		Configuration conf = new Configuration();
//		HadoopUtil.delete(conf, samples);
//		HadoopUtil.delete(conf, output);
//		RandomUtils.useTestSeed();
//		generateSamples();
//		writeSampleData(samples);
//		CanopyDriver.buildClusters(conf, samples, output,
//				new ManhattanDistanceMeasure(), T1, T2, 0, true);
		loadClustersWritable(output);
		
		new MyDisplayCanopy();
	}

}