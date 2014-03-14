package atrmat;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.StringTokenizer;

/**
 * @category util tools func: preprocess the cluster result csv file
 * */
public class ClusterResultPreprocess {
	private static String localDirectory = "cluster_result";
	private static String clusterCenterFile = "MyClusterResultReport";
	private static String parseDirectory = "parse";
	private static String[] clusterCenters = new String[100];
	private static int centerNum = 0;

	public static void main(String[] args) throws IOException {

		BufferedReader fileReader;
		File resultFile = new File(localDirectory + "/" + parseDirectory + "/"
				+ "cluster_stat.csv");// output File
		FileOutputStream out = new FileOutputStream(resultFile);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);

		// cluster centers report file
		fileReader = new BufferedReader(new FileReader(localDirectory + "/"
				+ clusterCenterFile));
		String line = null;
		bw.write("Num,User_id,an,ao,ap,aq,ar,as\n");
		int i = 0;
		while ((line = fileReader.readLine()) != null) {
			if (line.contains("CL-")) {
				centerNum++;
				int clusterNum_begin = line.indexOf("n=");
				String clusterNum;
				if (clusterNum_begin > 0) {
					int clusterNum_end = line.indexOf("c=[", clusterNum_begin);
					clusterNum = line.substring(clusterNum_begin + 2,
							clusterNum_end).trim();
					bw.write(clusterNum + ",");
					String tmpLine = line.substring(clusterNum_end + 3,
							line.indexOf("]", clusterNum_end));
					bw.write(tmpLine + "\n");
					clusterCenters[i++] = tmpLine;
				}
			}
		}
		bw.close();
		osw.close();
		out.close();
		fileReader.close();
		i = 1;

		/*
		File rootDirectory = new File(localDirectory);
		File[] files = rootDirectory.listFiles();
		for (File file : files) {
			if (file.isDirectory()) {
				continue;
			} else {
				if (file.getName().endsWith(i + ".csv")) {
					// cluster #i report file
					System.out.println("processing cluster #" + i);
					fileReader = new BufferedReader(new FileReader(
							file.getAbsolutePath()));
					if (calculateDistance(i, clusterCenters[i], fileReader) < 0) {
						System.out.println("cluster center #" + i
								+ " contains invalid vector");
					}
					// file.getAbsolutePath();
				}
			}
			i++;
		}
		*/
		for (i = 1; i < centerNum; i++){ 
			fileReader = new BufferedReader(new FileReader(localDirectory + "/"+"clusterResultFile_"+i+".csv"));
			System.out.println("processing cluster #" + i);
			if (calculateDistance(i, clusterCenters[i], fileReader) < 0) {
				System.out.println("cluster center #" + i
						+ " contains invalid vector");
			}
			fileReader.close(); 
			i++;
		}
		
		fileReader.close();
	}

	private static int calculateDistance(int centerNum, String clusterCenter,
			BufferedReader fileReader) throws IOException {
		// TODO Auto-generated method stub
		if (clusterCenter.contains(":")) {
			return -1;
		}
		File resultFile = new File(localDirectory + "/" + parseDirectory + "/"
				+ "cluster_distribute_" + centerNum + ".csv");// output File
		FileOutputStream out = new FileOutputStream(resultFile);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);
		String line = null;
		double sum = 0.0;
		double[] center = StringtoDouble(clusterCenter);
		while ((line = fileReader.readLine()) != null && !line.equals("\n") ) {
			double[] point = StringtoDouble(line);
			if (point.length != center.length) {
				System.out.println("invalid line: " + line);
				bw.close();
				osw.close();
				out.close();
				return -1;
			}
			sum = 0.0;
			int i;
			for (i = 1; i < center.length; i++) {
				// start from second
				sum += Math.abs(center[i] * center[i] - point[i] * point[i]);
			}
			bw.write(Math.sqrt(sum) + "\n");
		}

		bw.close();
		osw.close();
		out.close();
		return 0;
	}

	public static double[] StringtoDouble(String str) {
		double ret[] = new double[str.split(",").length];
		StringTokenizer toKenizer = new StringTokenizer(str, ",");
		int i = 0;
		while (toKenizer.hasMoreElements()) {
			ret[i++] = Double.valueOf(toKenizer.nextToken());
		}
		return ret;

	}

}
