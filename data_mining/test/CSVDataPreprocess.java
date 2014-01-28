package atrmat;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.OutputStreamWriter;

/**
 * @category util tools
 * func: preprocess the src csv file
 * */
public class CSVDataPreprocess {
	public static String csvTitle[];
	
	/**
     * ip address to long
     * @param ip
     * @return
     */  
   public static long ip2long(String ip) {  
       if (ip.equals("null"))
    	   return -1;
	   String[] ips = ip.split("[.]");  
       long num =   16777216L*Long.parseLong(ips[0]) + 65536L*Long.parseLong(ips[1]) + 256*Long.parseLong(ips[2]) + Long.parseLong(ips[3]);  
       return num;  
    }  
     
   /**
     * long to ip address
     * @param ipLong
     * @return
     */  
   public static String long2ip(long ipLong) {  
       //long ipLong = 1037591503;  
       long mask[] = {0x000000FF,0x0000FF00,0x00FF0000,0xFF000000};  
       long num = 0;  
        StringBuffer ipInfo = new StringBuffer();  
       for(int i=0;i<4;i++){  
            num = (ipLong & mask[i])>>(i*8);  
           if(i>0) ipInfo.insert(0,".");  
            ipInfo.insert(0,Long.toString(num,10));  
        }  
       return ipInfo.toString();  
    }  
   
	public static void main(String[] args) {
		try {
			BufferedReader reader = new BufferedReader(new FileReader(
					"J://GENERIC_CorrelatedXDR.csv"));// the source file path
			int linenum = 0;
			int itemnum = 0;
			String itemLine = null;
			String line = null;
			line = reader.readLine();// first line is the title of csv file
			linenum++;
			long start = -1;
			long end = -1;
			long sessionTime = 0;
			long ConnectionTime = 0;
			File file = new File("./GENERIC_CorrelatedXDR_preprocessed.csv");// output the preprocessed file
			FileOutputStream out = new FileOutputStream(file);
			OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
			BufferedWriter bw = new BufferedWriter(osw);

			csvTitle = line.split(",");
			System.out.println(csvTitle.length);

			for (int i = 0; i <= csvTitle.length - 1; i++) {
				System.out.print(csvTitle[i] + ",");
				// write the csv title to the output file
				// if (i < csvTitle.length -1)
				// bw.write(csvTitle[i]+",");
				// else
				// bw.write(csvTitle[i]+"\n");
			}
			// write the processed csv title
//			bw.write("XDR ID,session Time,ClientAddress,ServerAddress,IpID,ClientPort,Connection Time,"
//					+ "ClientTunnelAddress,ServerTunnelAddress,ClientToServerPackets,ServerToClientPackets,"
//					+ "ClientToServerOctets,ServerToClientOctets,AverageResponseTime,TotalResp,"
//					+ "SessionLastUpdateTime,appId,End User Address,"
//					+ "MSISDN,IMSI,IMEI,APN,CELL/SAC,LAC/TAC/SID,RAC/NID,NumOfUpdates\n");
			while ((line = reader.readLine()) != null) {
				linenum++;
				itemnum++;
				String item[] = line.split(",");// split by dot -- ","
//				String last = item[item.length - 1];
				// int value = Integer.parseInt(last);//
				// System.out.println(last);

				for (int i = 0; i <= item.length - 1; i++) {
					if (csvTitle[i].equals("EndPoinType|IpAddress|TeId")
							|| csvTitle[i].equals("ECGI|RATType|TAC")
							|| csvTitle[i].equals("BSID")
							|| csvTitle[i].equals("IMSI|NSAPI"))
						continue;
					switch (csvTitle[i]) {
					case "XDR ID":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine = item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "sessionStartTime":
						start = new Long(item[i].trim());
						break;
					case "sessionEndTime":
						end = new Long(item[i].trim());
						if (start != -1) {
							sessionTime = end - start;
							end = -1;
							start = -1;
						}
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += sessionTime + ",";
							// bw.write(sessionTime+",");
						}
						break;
					case "ClientAddress":
						if (!item[i].equals("")) {
							itemnum++;
//							itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + ",";
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "ServerAddress":
						if (!item[i].equals("")) {
							itemnum++;
//							itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + ",";
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "IpID":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "ClientPort":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "ConnectionEndTime":
						end = new Long(item[i].trim());
						break;
					case "ConnectionStartTime":
						start = new Long(item[i].trim());
						if (end != -1) {
							ConnectionTime = end - start;
							end = -1;
							start = -1;
						}
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += ConnectionTime + ",";
							// bw.write(ConnectionTime+",");
						}
						break;
					case "ClientTunnelAddress":
						if (!item[i].equals("")) {
							itemnum++;
//							itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + ",";
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "ServerTunnelAddress":
						if (!item[i].equals("")) {
							itemnum++;
//							itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + ",";
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "ClientToServerPackets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "ServerToClientPackets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "ClientToServerOctets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "ServerToClientOctets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "AverageResponseTime":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "TotalResp":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "SessionLastUpdateTime":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "ClientTeid":
						// contains empty value
						break;
					case "ServerTeid":
						// contains empty value
						break;
					case "appId":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "End User Address":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += ip2long(item[i]) + ",";
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "MSISDN":
						if (!item[i].equals("")) {
							if (!item[i].equals(0)){
								itemnum++;
								itemLine += item[i] + ",";
								// bw.write(item[i]+",");
							}
						}
						break;
					case "IMSI":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + ",";
							// bw.write(item[i]+",");
						}
						break;
					case "IMEI":
						if (!item[i].equals("")) {
							if (!item[i].equals(0)){
								itemnum++;
								itemLine += item[i] + ",";
								// bw.write(item[i]+",");
							}
						}
						break;
					case "APN":
						if (!item[i].equals("")) {
							itemnum++;
						}
						if (item[i].equalsIgnoreCase("cmnet")) {
							itemLine += "1" + ",";
							// bw.write("1"+",");
						} else if (item[i].equalsIgnoreCase("cmwap")) {
							itemLine += "2" + ",";
							// bw.write("2"+",");
						} else {
							itemLine += "0" + ",";
							// bw.write("0"+",");
						}
						break;
					case "CELL/SAC":
						if (!item[i].equals("")){
							itemnum++;
							itemLine += item[i] + ",";
						}
						break;
					case "LAC/TAC/SID":
						if (!item[i].equals("")){
							itemnum++;
							itemLine += item[i] + ",";
						}
						break;
					case "RAC/NID":
						if (!item[i].equals("")){
							itemnum++;
							itemLine += item[i] + ",";
						}
						break;
					case "NumOfUpdates":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + "\n";
							// bw.write(item[i]+"\n");
						}
						break;
					default:
						break;
					}
					if (item[i].equals(""))
						System.out.println(linenum + ": " + csvTitle[i]
								+ " contains null or invalid value!");
				}
				if (itemnum < 26) {
					System.out.println(linenum + ": "
							+ "lacks values, item num is " + itemnum);
				} else {
					bw.write(itemLine);
				}
				itemnum = 0;
				itemLine = null;
			}
            bw.close();
            osw.close();
            out.close(); 
            reader.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
