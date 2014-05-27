package atrmat;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.OutputStreamWriter;

/**
 * @category util tools func: preprocess the HTTP csv file
 * */
public class HTTPDataPreprocess {
	public static String csvTitle[];

	/**
	 * ip address to long
	 * 
	 * @param ip
	 * @return
	 */
	public static long ip2long(String ip) {
		if (ip.equals("null"))
			return -1;
		String[] ips = ip.split("[.]");
		long num = 16777216L * Long.parseLong(ips[0]) + 65536L
				* Long.parseLong(ips[1]) + 256 * Long.parseLong(ips[2])
				+ Long.parseLong(ips[3]);
		return num;
	}

	/**
	 * long to ip address
	 * 
	 * @param ipLong
	 * @return
	 */
	public static String long2ip(long ipLong) {
		// long ipLong = 1037591503;
		long mask[] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
		long num = 0;
		StringBuffer ipInfo = new StringBuffer();
		for (int i = 0; i < 4; i++) {
			num = (ipLong & mask[i]) >> (i * 8);
			if (i > 0)
				ipInfo.insert(0, ".");
			ipInfo.insert(0, Long.toString(num, 10));
		}
		return ipInfo.toString();
	}

	public static void main(String[] args) {
		try {
			BufferedReader reader = new BufferedReader(new FileReader(
					"J://HTTP_CorrelatedXDR.csv"));// the source file path
			int linenum = 0;
			int itemnum = 0;
			String itemLine = "";
			String line = "";
			line = reader.readLine();// first line is the title of csv file
			linenum++;
			Double start = -1.0;
			Double end = -1.0;
			Double sessionTime = 0.0;
			Double ConnectionTime = 0.0;
			String splitSymbol = ",";
			File file = new File("./HTTP_CorrelatedXDR_preprocessed.csv");// output
			FileOutputStream out = new FileOutputStream(file);
			OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
			BufferedWriter bw = new BufferedWriter(osw);

			csvTitle = line.split(",");
			System.out.println(csvTitle.length);

			for (int i = 0; i <= csvTitle.length - 1; i++) {
				System.out.print(csvTitle[i] + ",");
			}
			bw.write("sessionTime,ClientAddress,ServerAddress,IpID,ClientPort,ConnectionTime,"
					+ "ClientTunnelAddress,ServerTunnelAddress,ClientToServerPackets,ServerToClientPackets,"
					+ "ClientToServerOctets,ServerToClientOctets,AverageResponseTime,TotalResp,SessionLastUpdateTime\n");
			// write the processed csv title
			while ((line = reader.readLine()) != null) {
				linenum++;
				itemnum++;
				String item[] = line.split(",");// split by dot -- ","
				// String last = item[item.length - 1];
				// int value = Integer.parseInt(last);//
				// System.out.println(last);
				if (item.length != csvTitle.length)
					continue;
				for (int i = 0; i < item.length - 1; i++) {
					item[i] = item[i].replace("\"", "").trim();
					// switch and write
					switch (csvTitle[i].replace("\"", "").trim()) {
					case "sessionStartTime":
						start = new Double(item[i].replace("\"", "").trim());
						break;
					case "sessionEndTime":
						end = new Double(item[i].replace("\"", "").trim());
						if (start != -1) {
							sessionTime = end - start;
							end = -1.0;
							start = -1.0;
						}
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += sessionTime + splitSymbol;
							// bw.write(sessionTime+",");
						}
						break;
					case "ClientAddress":
						if (!item[i].equals("")) {
							itemnum++;
							// itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + splitSymbol;
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "ServerAddress":
						if (!item[i].equals("")) {
							itemnum++;
							// itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + splitSymbol;
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "IpID":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "ClientPort":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "ConnectionEndTime":
						end = new Double(item[i].trim());
						break;
					case "ConnectionStartTime":
						start = new Double(item[i].trim());
						if (end != -1) {
							ConnectionTime = end - start;
							end = -1.0;
							start = -1.0;
						}
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += ConnectionTime + splitSymbol;
							// bw.write(ConnectionTime+",");
						}
						break;
					case "ClientTunnelAddress":
						if (!item[i].equals("")) {
							itemnum++;
							// itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + splitSymbol;
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "ServerTunnelAddress":
						if (!item[i].equals("")) {
							itemnum++;
							// itemLine += item[i] + ",";
							itemLine += ip2long(item[i]) + splitSymbol;
							// bw.write(ip2long(item[i])+",");
						}
						break;
					case "ClientToServerPackets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "ServerToClientPackets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "ClientToServerOctets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "ServerToClientOctets":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "AverageResponseTime":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "TotalResp":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
							// bw.write(item[i]+",");
						}
						break;
					case "SessionLastUpdateTime":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i];
							// bw.write(item[i]+",");
						}
						break;
					default:
						break;
					}

					if (item[i].equals(""))
						System.out.println(linenum + ": " + csvTitle[i]
								+ " contains null or invalid value!");
				}
				if (itemnum != 16) {
					System.out.println(linenum + ": "
							+ "lacks values, item is " + csvTitle[itemnum]);
				} else {
					bw.write(itemLine+"\n");
				}
//				if(linenum == 2)
//					continue;
//				if(linenum == 1000){
//					break;
//				}
				itemnum = 0;
				itemLine = "";
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
