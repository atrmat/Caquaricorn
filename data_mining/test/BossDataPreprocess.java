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
public class BossDataPreprocess {
	public static String csvTitle[];

	public static void main(String[] args) {
		try {
			BufferedReader reader = new BufferedReader(new FileReader(
					"BossData.csv"));// the source file path
			// "J://HTTP_CorrelatedXDR.csv"));// the source file path
			int linenum = 0;
			int itemnum = 0;
			String itemLine = "";
			String line = "";
			line = reader.readLine();// first line is the title of csv file
			linenum++;
			String splitSymbol = " ";// the split symbol writes into the csv
										// file
			File file = new File("./BossData_preprocessed.csv");// output
			FileOutputStream out = new FileOutputStream(file);
			OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
			BufferedWriter bw = new BufferedWriter(osw);

			csvTitle = line.split(",");
			System.out.println("csv title length: "+csvTitle.length);

			for (int i = 0; i <= csvTitle.length - 1; i++) {
				csvTitle[i] = csvTitle[i].replace("\"", "").trim();
//				System.out
//						.print("case \""
//								+ csvTitle[i]
//								+ "\":"
//								+ "\n"
//								+ "if(!item[i].equals(\"\")){\n itemnum++;\n line+=item[i]+splitSymbol;\n} \n"
//								+ "break;\n");
				System.out.println(csvTitle[i]);
			}

			// write the processed csv title
			while ((line = reader.readLine()) != null) {
				linenum++;
				itemnum = 0;
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
					case "USER_ID":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
//							itemLine += linenum + splitSymbol;
						}
						break;
					case "第1时段无流量无位置变动的次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第2时段无流量无位置变动的次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第3时段无流量无位置变动的次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第4时段无流量无位置变动的次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第5时段无流量无位置变动的次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第6时段无流量无位置变动的次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第1时段被动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第2时段被动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第3时段被动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第4时段被动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第5时段被动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第6时段被动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第1时段主动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第2时段主动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第3时段主动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第4时段主动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第5时段主动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第6时段主动流量无位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第1时段无流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第2时段无流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第3时段无流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第4时段无流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第5时段无流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第6时段无流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第1时段被动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第2时段被动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第3时段被动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第4时段被动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第5时段被动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第6时段被动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第1时段主动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第2时段主动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第3时段主动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第4时段主动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第5时段主动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					case "第6时段主动流量有位置变动次数":
						if (!item[i].equals("")) {
							itemnum++;
							itemLine += item[i] + splitSymbol;
						}
						break;
					default:
						break;
					}// switch-case
						//
					 if (item[i].equals(""))
						 System.out.println(linenum + ": " + csvTitle[i]
							+ " contains null or invalid value!");
				}// for (int i = 0; i < item.length - 1; i++)
				if (itemnum != 37) {
					System.out.println(linenum + ": "
							+ "lacks values, item is " + csvTitle[itemnum]);
				} else {
					System.out.println("write line: "+linenum);
					bw.write(itemLine + "\n");
				}
//				if (linenum == 1000)
//					break;
				itemnum = 0;
				itemLine = "";

			}// while-loop
			bw.close();
			osw.close();
			out.close();
			reader.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
