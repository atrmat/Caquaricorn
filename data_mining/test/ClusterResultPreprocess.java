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
	//files to process
	private static String localDirectory = "J:\\2014_czh_thesis\\data\\Kmeans_80w\\Kmeans_80w_一天流量和位置\\canopy+kmeans\\3-19-5";
	private static String clusterCenterFile = "MyClusterResultReport";
	private static String clusterStatsFile = "clusterResultFile_stats";
	//output processed files
	private static String clusterSummaryFile = "cluster_summary.csv";
	private static String clusterDistanceFile = "cluster_distance.csv";
	private static String clusterCenters_Clustering = "clusterCenters_clustering.csv";
	private static String parseDirectory = "parse1";
	private static String[] clusterCenters = new String[100];
	private static long[] centerNum = new long[100];
	private static double[][] centers = new double[100][100];
	private static int centersCounter = 0;

	public static void main(String[] args) throws IOException {
		
		File tmp = new File(localDirectory + "/" + parseDirectory);
		if (!tmp.exists()){
			//if not exist dir "parse", then create one
			tmp.mkdir();
		}
		
		BufferedReader fileReader, fileReader2;
		File resultFile = new File(localDirectory + "/" + parseDirectory + "/"
				+ clusterSummaryFile);// output clustering summary File
		FileOutputStream out = new FileOutputStream(resultFile);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);

		// cluster centers report file
		fileReader = new BufferedReader(new FileReader(localDirectory + "/"
				+ clusterCenterFile));
		fileReader2 = new BufferedReader(new FileReader(localDirectory + "/"
				+clusterStatsFile));
		String line = null, line2 = null;
		bw.write("clusterID,Num,Type,NumOfObserved,User_id,无流量无位置变动的次数,被动流量无位置变动的次数,主动流量无位置变动的次数,无流量有位置变动的次数,被动流量有位置变动的次数,主动流量有位置变动的次数\n");
		int i = 0;
		int cn = 0;
		String tmp2[][] = new String[100][];
		while ((line2 = fileReader2.readLine()) != null){
			tmp2[i++]=line2.split(","); 
//			System.out.println(tmp2[i-1][0]);
		}
		cn = i;
		i = 0;
		while ((line = fileReader.readLine()) != null ) {
			String cNum = null;
			int clusterNum_begin;
			int j = 0;
			if (line.contains("n=")){
				clusterNum_begin = line.indexOf("-");
				cNum = line.substring(clusterNum_begin+1, line.indexOf("{n=", clusterNum_begin));
				while (j < cn){
					if(tmp2[j][0].equals(cNum)){
						bw.write(tmp2[j][0]+",");
						bw.write(tmp2[j][1]+",");
						
						if (line.contains("CL-")){
							bw.write("CL"+",");
						}else if (line.contains("SC-")){
							bw.write("SC"+",");
						}else if (line.contains("SV-")){
							bw.write("SV"+",");
						}else if (line.contains("VL-")){
							bw.write("VL"+",");
						}else
							continue;
						centerNum[i]= Long.parseLong(cNum);
						// Has the centroid converged with the center in Kluster? VL:CL
						// S is the soft-cluster
						clusterNum_begin = line.indexOf("n=");//n is Num of Observations
						String clusterNum;
						if (clusterNum_begin > 0) {
							int clusterNum_end = line.indexOf("c=[", clusterNum_begin);//c is center
							clusterNum = line.substring(clusterNum_begin + 2,
									clusterNum_end).trim();
							bw.write(clusterNum + ",");
							String tmpLine = line.substring(clusterNum_end + 3,
									line.indexOf("]", clusterNum_end));
							// r is radius
							if (tmpLine.contains(":")){// format the centers that contains ":"
								String tmp3[] = tmpLine.split(",");
								tmpLine = "";
								String tmp4[][] = new String[8][];
								for (int i1 = 0; i1 < tmp3.length; i1++){
									tmp4[i1] = tmp3[i1].split(":");
								}
								for (int i1 = 0, j1 = 0; i1 < 7; i1++){
									if (j1 < tmp3.length){
										if (Integer.parseInt(tmp4[j1][0].trim())== i1){
											tmpLine += tmp4[j1][1];
											j1++;
										}else
											tmpLine += "0.0";
									}else
										tmpLine += "0.0";
									if (i1+1 < 7){
										tmpLine += ",";
									}
								}
								System.out.println(tmpLine);
							}
							bw.write(tmpLine + "\n");
							clusterCenters[i++] = tmpLine;
						}
						
						break;
					}
					j++;
				}
			}
			if (j > cn)// no found the cNum in tmp2[j][0]
				continue;
		}
		bw.close();
		osw.close();
		out.close();
		fileReader.close();
		fileReader2.close();
		
		i--; // i is the num of clustering centers
		System.out.println(i);
		for (int j = 0; j <= i; j++){
			if (!new File(localDirectory + "/"+"clusterResultFile_"+centerNum[j]+".csv").exists()){
				System.out.println(localDirectory + "/"+"clusterResultFile_"+centerNum[j]+".csv"+" doesnot exists");
				continue;//no exist clusterResultFile_i
			}
			fileReader = new BufferedReader(new FileReader(localDirectory + "/"+"clusterResultFile_"+centerNum[j]+".csv"));
			System.out.println("processing cluster #" + centerNum[j]);
			if (calculateDistance(centerNum[j], clusterCenters[j], fileReader) < 0) {
				System.out.println("cluster center #" + centerNum[j]
						+ " contains invalid vector");
			}
			fileReader.close();
		}
		fileReader.close();
		
		//calculate the minimum distance among the centers
		double tmpDis;
		double minDis = -1;
		File resultFile1 = new File(localDirectory + "/" + parseDirectory + "/"
				+ clusterDistanceFile);// output clustering summary File
		FileOutputStream out1 = new FileOutputStream(resultFile1);
		OutputStreamWriter osw1 = new OutputStreamWriter(out1, "UTF8");
		BufferedWriter bw1 = new BufferedWriter(osw1);
		bw1.write("centerNum,minDistance\n");
		for (int j = 0; j <= i; j++){
			minDis = -1;
			for (int k = 0; k <= i; k++){
				if (k == j)
					continue;
				tmpDis = 0.0;
				for (int t = 1; t < centers[j].length; t++){
					tmpDis += Math.abs(centers[j][t]*centers[j][t] - centers[k][t]*centers[k][t]);
				}
				tmpDis = Math.sqrt(tmpDis);
				if (tmpDis < minDis || minDis == -1)
					minDis = tmpDis;
			}
			bw1.write(centerNum[j]+","+minDis+"\n");
		}
//		System.out.println(minDis);
		bw1.close();
		osw1.close();
		out1.close();
	}

	private static int calculateDistance(long centerNum, String clusterCenter,
			BufferedReader fileReader) throws IOException {
		// TODO Auto-generated method stub
		if (clusterCenter.contains(":")) {
			System.out.println(clusterCenter);
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
		centers[centersCounter++]=center;
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
				// start from second vector
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
