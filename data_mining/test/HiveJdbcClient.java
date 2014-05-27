package atrmat;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class HiveJdbcClient {
	private static String driverName = "org.apache.hadoop.hive.jdbc.HiveDriver";
	private static Connection con;
	private static Statement stmt;

	public static Statement getStmt() {
		return stmt;
	}

	public static void setStmt(Statement stmt) {
		HiveJdbcClient.stmt = stmt;
	}

	public Connection getCon() {
		return con;
	}

	public void setCon(Connection con) {
		this.con = con;
	}

	public void connectionHive() throws SQLException {
		try {
			Class.forName(driverName);
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		}

		// replace "hive" here with the name of the user the queries should run
		// as
		this.setCon(DriverManager.getConnection(
				"jdbc:hive://192.168.1.28:10000/default", "root", "1"));
		this.setStmt(this.getCon().createStatement());
	}

	public void createHiveTable(String tableName, String sql) throws SQLException {
		// create table
		ResultSet res;
		Statement stmt = this.getStmt();
		stmt.executeQuery("show tables");
		stmt.execute("drop table if exists " + tableName);
		// create the table with the special terminate symbol: ',' between the
		// column
		// the column in csv file is dot by ',' , e.g., example csv data file is
		// as follows
		// 1,foo
		// 2,bar
		// 3,txt
		stmt.execute(sql);
		// show tables
		sql = "show tables '" + tableName + "'";
		System.out.println("Running: " + sql);
		res = stmt.executeQuery(sql);
		if (res.next()) {
			System.out.println(res.getString(1));
		}
		// describe table
		sql = "describe " + tableName;
		System.out.println("Running: " + sql);
		res = stmt.executeQuery(sql);
		while (res.next()) {
			System.out.println(res.getString(1) + "\t" + res.getString(2));
		}
	}

	/**
	 * using the hive-jdbc client to insert GENERIC_CorrelatedXDR data into the
	 * hive
	 * 
	 * @param args
	 * @return
	 * @throws SQLException
	 */
	public void insertTable(String tableName, String filePath)
			throws SQLException {
		// load data into table
		// NOTE: file path has to be local to the hive server where it start: e.g.,
		// start the hiveserver in this /usr/local/src/hive-0.11.0-bin/ console path
		// the file path should be: /usr/local/src/hive-0.11.0-bin/a.txt
		// NOTE: a.txt is a ctrl-A separated file with two fields per line
		// echo -e '1\x01foo' > a.txt
		// echo -e '2\x01bar' >> a.txt
		String sql = "load data local inpath '" + filePath + "' into table "
				+ tableName;
		System.out.println("Running: " + sql);
		Statement stmt = this.getStmt();
		stmt.execute(sql);
	}
	
	public void queryAttributeValue(String tableName, String attributeName) throws SQLException, IOException{
		String sql;
		ResultSet res;
		File file = new File("query"+attributeName+".csv");// output the preprocessed file
		FileOutputStream out = new FileOutputStream(file);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);
//		select count(distinct MSISDN) from GENERIC_CorrelatedXDR where MSISDN is not null and MSISDN <> 0
		sql = "select count(distinct "+attributeName+")"+" from "+tableName+" where "+attributeName+" is not null and "+attributeName+" <> 0";
		res = stmt.executeQuery(sql);
		if (res.next()){
			bw.write("totol num: "+ res.getString(1)+"\n");
		}
		sql = "select "+attributeName+", count(*) from " +tableName+" where "+attributeName+" is not null group by "+attributeName;
		System.out.println("Running: " + sql);
		res = stmt.executeQuery(sql);
		bw.write(attributeName+","+"num"+"\n");
		while (res.next()) {
			// System.out.println(String.valueOf(res.getInt(1)) + "\t" +
			// res.getString(2));
			System.out.println(res.getString(1)+","+res.getString(2));
			bw.write(res.getString(1)+","+res.getString(2)+"\n");
		}
		bw.close();
	}
	
	public void queryTable(String tableName) throws SQLException {
		// select * query
		String sql;
		ResultSet res;
//		sql = "select * from " + tableName;
//		System.out.println("Running: " + sql);
//		res = stmt.executeQuery(sql);
//		while (res.next()) {
//			// System.out.println(String.valueOf(res.getInt(1)) + "\t" +
//			// res.getString(2));
//			System.out.println(res.getString(1));
//		}

		// regular hive query
		sql = "select count(1) from " + tableName;
		System.out.println("Running: " + sql);
		res = stmt.executeQuery(sql);
		while (res.next()) {
			System.out.println(res.getString(1));
		}
	}

	public void querySQL(String sql) throws SQLException, IOException{
		ResultSet res;
		int countNum = 0;
		String splitSymbol = ",";// write the split symbol into the files
		System.out.println("Running: " + sql);
		res = stmt.executeQuery(sql);
		File file = new File("querySQL"+".csv");// output the preprocessed file
		FileOutputStream out = new FileOutputStream(file);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);
//		bw.write(sql+"\n");
		while (res.next()) {
			countNum++;
			bw.write(res.getString(1)+splitSymbol+res.getDouble(2)+splitSymbol+res.getDouble(3)+splitSymbol+res.getDouble(4)+splitSymbol+res.getDouble(5)+splitSymbol+res.getDouble(6)+splitSymbol+res.getDouble(7)
					+splitSymbol+res.getDouble(8)+splitSymbol+res.getDouble(9)+splitSymbol+res.getDouble(10)+splitSymbol+res.getDouble(11)+splitSymbol+res.getDouble(12)+splitSymbol+res.getDouble(13)+splitSymbol+res.getDouble(14)
					+splitSymbol+res.getDouble(15)+splitSymbol+res.getDouble(16)+splitSymbol+res.getDouble(17)+splitSymbol+res.getDouble(18)+splitSymbol+res.getDouble(19)+splitSymbol+res.getDouble(20)+splitSymbol+res.getDouble(21)
					+splitSymbol+res.getDouble(22)+splitSymbol+res.getDouble(23)+splitSymbol+res.getDouble(24)+splitSymbol+res.getDouble(25)+splitSymbol+res.getDouble(26)+splitSymbol+res.getDouble(27)+splitSymbol+res.getDouble(28)
					+splitSymbol+res.getDouble(29)+splitSymbol+res.getDouble(30)+splitSymbol+res.getDouble(31)+splitSymbol+res.getDouble(32)+splitSymbol+res.getDouble(33)+splitSymbol+res.getDouble(34)+splitSymbol+res.getDouble(35)
					+splitSymbol+res.getDouble(36)+splitSymbol+res.getDouble(37)+"\n");
//			System.out.println(res.getLong(1));
//			System.out.println(res.getString(1)+","+res.getDouble(2)+","+res.getDouble(3)+","+res.getDouble(4)+","+res.getDouble(5)+","+res.getDouble(6)+","+res.getDouble(7));
//			bw.write(res.getString(1)+splitSymbol+res.getDouble(2)+splitSymbol+res.getDouble(3)+splitSymbol+res.getDouble(4)+splitSymbol+res.getDouble(5)+splitSymbol+res.getDouble(6)+splitSymbol+res.getDouble(7)+"\n");
//			bw.write(res.getString(1)+","+res.getString(2)+","+res.getString(3)+"\n");
		}
//		bw.write("count Num: "+countNum);
		bw.close();
	}
	
	public void calcStates()throws SQLException, IOException{
		ResultSet res;
		String sql;
		String line = "";
		BufferedReader bufferedReader = new BufferedReader(new FileReader("permute.csv"));
		bufferedReader.readLine();
		String []attributes = {"ar", "ax", "bd", "bj", "bp", "bv"};
		File file = new File("permute_1.csv");
		FileOutputStream out = new FileOutputStream(file);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);
		bw.write("A,B,C,D,E,F,sum\n");
//		int n = 0;
		while ((line = bufferedReader.readLine())!= null){
//			n++;
//			if (n > 2)
//				break;
			String[] tmp = line.split(",");
			int len = tmp.length-1;
			sql = "select count(DISTINCT a) from rawdata80w where ";
			for (int i = 0; i < len; i++){
				sql += attributes[i]+"="+tmp[i]+" & ";
				bw.write(tmp[i]+",");
			}
			sql += attributes[len]+"="+tmp[len];
			bw.write(tmp[len]+",");
			// hive sql processing
			System.out.println(sql);
			res = stmt.executeQuery(sql);
			while(res.next()){
				bw.write(res.getLong(1)+"\n");
				System.out.println(res.getLong(1));
			}
		}
		bw.close();
		osw.close();
		out.close();		
	}
	
	public void calcStates2(int num, String[] attributes)throws SQLException, IOException{
		ResultSet res;
		String sql;
		String line = "";
		BufferedReader bufferedReader = new BufferedReader(new FileReader("permute_3.csv"));
		// order by this bufferedReader to query others
		bufferedReader.readLine();
		File file = new File("permute_"+num+".csv");
		FileOutputStream out = new FileOutputStream(file);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);
		bw.write("A,B,C,D,E,F,sum\n");
		while ((line = bufferedReader.readLine())!= null){
			String[] tmp = line.split(",");
			int len = tmp.length-2;
			sql = "select count(DISTINCT a) from rawdata80w where ";
			for (int i = 0; i < len; i++){
				sql += attributes[i]+"="+tmp[i]+" & ";
				bw.write(tmp[i]+",");
			}
			sql += attributes[len]+"="+tmp[len];
			bw.write(tmp[len]+",");
			// hive sql processing
			System.out.println(sql);
			res = stmt.executeQuery(sql);
			while(res.next()){
				bw.write(res.getLong(1)+"\n");
				System.out.println(res.getLong(1));
			}
		}
		bw.close();
		osw.close();
		out.close();		
	}
	
	public HiveJdbcClient() throws SQLException {
		connectionHive();
	}

}