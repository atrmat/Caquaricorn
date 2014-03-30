package atrmat;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.StringTokenizer;

/**
 * @category util tools func: preprocess the second clustering result csv file
 * */
public class ClusterResultPreprocess2 {
	//files to process
	private static String localDirectory = "Kmeans_result_80w";
	private static String clusterCenterFile = "MyClusterResultReport2";
	private static String clusterStatsFile = "clusterResultFile2_stats";
	//output processed files
	private static String parseDirectory = "parse2";
	private static String[] clusterCenters = new String[100];
	private static int centerID[]= new int[100];
	private static int centerNum = 0;
	
	public static Comparator<clusterCenter> myComparator = new Comparator<clusterCenter>(){

		@Override
		public int compare(clusterCenter arg0, clusterCenter arg1) {
			// TODO Auto-generated method stub
			return arg0.ID-arg1.ID;
		}
	};

	public static void main(String[] args) throws IOException {
		
		ArrayList<clusterCenter> clusterCentersArrayList = new ArrayList<clusterCenter>(100);
		File tmp = new File(localDirectory + "/" + parseDirectory);
		if (!tmp.exists()){
			//if not exist dir "parse", then create one
			tmp.mkdir();
		}
		
		BufferedReader fileReader, fileReader2, fileReader3;		

		// cluster centers report file
		fileReader = new BufferedReader(new FileReader(localDirectory + "/"
				+ clusterCenterFile));
		fileReader2 = new BufferedReader(new FileReader(localDirectory + "/"
				+clusterStatsFile));
		String line = null, line2 = null;
		int i = 0;
		while ((line = fileReader.readLine()) != null ) {
			if (!line.contains("CL-") && !line.contains("SC-") && !line.contains("SV-") && !line.contains("VL-"))
				continue;
			
			centerNum++;
			// Has the centroid converged with the center in Kluster? VL:CL
			// S is the soft-cluster
			String clusterNum;
			int clusterNum_begin = line.indexOf("-");
			int clusterNum_end = line.indexOf("{n=",clusterNum_begin);
			int cNum = Integer.parseInt(line.substring(clusterNum_begin+1, clusterNum_end));
			centerID[centerNum-1] = cNum;
			clusterNum_begin = line.indexOf("n=");//n is Num of Observations
			if (clusterNum_begin > 0) {
				clusterNum_end = line.indexOf("c=[", clusterNum_begin);//c is center
				clusterNum = line.substring(clusterNum_begin + 2,
						clusterNum_end).trim();
				String tmpLine = line.substring(clusterNum_end + 3,
						line.indexOf("]", clusterNum_end));
				clusterCenters[centerNum-1] = tmpLine;
				// r is radius
				if (line.indexOf(":", clusterNum_begin)== -1){
					continue;
//					bw2.write(tmpLine+"\n");
				}
			}
		}
		for(int i1=0;i1<centerNum;i1++){
			clusterCentersArrayList.add(new clusterCenter(centerID[i1], clusterCenters[i1]));
//			System.out.println(centerID[i1]+" "+clusterCenters[i1]);
		}
		fileReader.close();
		fileReader2.close();
		System.out.println("size: "+clusterCentersArrayList.size());
		// sorting the center points
		Collections.sort(clusterCentersArrayList,myComparator);
		System.out.println(centerNum);
		for(int i1=0;i1<centerNum;i1++){
			System.out.println(clusterCentersArrayList.get(i1).toString());
		}
		
		//calculate the distance in the cluster
		fileReader2 = new BufferedReader(new FileReader(localDirectory + "/"+clusterStatsFile));
		String line3 = null;
		for (i = 0; i < centerNum; i++){
			double clusterNum[];
			double clusterID[];
			if ((line2 = fileReader2.readLine()) != null) {
				clusterNum = StringtoDouble(line2);
				int cNum = (int) clusterNum[0];
				if (!new File(localDirectory + "/" + "clusterResultFile2_" + cNum
						+ ".csv").exists()){
					System.out.println("no exist clusterResultFile2_" + cNum);
					continue;// no exist clusterResultFile_clusterNum[0]
				}
				fileReader3 = new BufferedReader(new FileReader(localDirectory + "/" + "clusterResultFile2_" + cNum + ".csv"));
				for (int j = 0; j < clusterNum[1]; j++) {
					line3 = fileReader3.readLine();
					clusterID = StringtoDouble(line3);
					int cID = (int) clusterID[0];
					if (!new File(localDirectory + "/" + "clusterResultFile_" + cID
							+ ".csv").exists()) {
						System.out.println("no exist clusterResultFile_");
						continue;// no exist clusterResultFile_i
					}
					fileReader = new BufferedReader(new FileReader(localDirectory
							+ "/" + "clusterResultFile_" + cID + ".csv"));
					System.out.println("processing cluster #" + clusterID[0]);
					if (cNum != clusterCentersArrayList.get(i).getID())
						System.out.println("cNum != clusterCentersArrayList.get(i).getID()");
					if (calculateDistance(clusterCentersArrayList.get(i).getID(), clusterCentersArrayList.get(i).getCenterVector(), fileReader) < 0) {
						System.out.println("cluster center #" + clusterID[0]
								+ " contains invalid vector");
					}
					fileReader.close();
				}
				fileReader3.close();
			}
		}
		
		fileReader.close();
	}

	public static int calculateDistance(int centerNum, String clusterCenter,
			BufferedReader fileReader) throws IOException {
		// TODO Auto-generated method stub
		if (clusterCenter.contains(":")) {
			System.out.println(clusterCenter);
			return -1;
		}
		File resultFile = new File(localDirectory + "/" + parseDirectory + "/"
				+ "cluster_distribute2_" + centerNum + ".csv");// output File
		FileOutputStream out = new FileOutputStream(resultFile,true);//append
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
	
	public static int[] insertSort(int[] intArray, String[] strArray){// insert sorting centers
        for(int i=0;i<intArray.length;i++){
                for(int j=i;j>0;j--){
                        if (intArray[j]<intArray[j-1]){// swap j and j-1
                                int temp=intArray[j-1];
                                intArray[j-1]=intArray[j];
                                intArray[j]=temp;
                                String tempStr = strArray[j-1];
                                strArray[j-1] = strArray[j];
                                strArray[j] = tempStr;
                        }else 
                        	break;
                }
        }
        return intArray;
	}
}

class clusterCenter{
	int ID;
	String centerVector;
	public int getID() {
		return ID;
	}
	public void setID(int iD) {
		ID = iD;
	}
	public String getCenterVector() {
		return centerVector;
	}
	public void setCenterVector(String centerVector) {
		this.centerVector = centerVector;
	}
	public clusterCenter(int ID, String centerVector) {
		// TODO Auto-generated constructor stub
		this.ID = ID;
		this.centerVector = centerVector;
	}
	public clusterCenter() {
		// TODO Auto-generated constructor stub
	}
	public String toString(){
		return (ID+" : "+centerVector);
	}
}