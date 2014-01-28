package atrmat;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
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
		System.out.println("Running: " + sql);
		res = stmt.executeQuery(sql);
//		File file = new File("querySQL"+".csv");// output the preprocessed file
//		FileOutputStream out = new FileOutputStream(file);
//		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
//		BufferedWriter bw = new BufferedWriter(osw);
//		bw.write(sql+"\n");
		while (res.next()) {
			System.out.println(res.getString(1));
//			bw.write(res.getString(1)+","+res.getString(2)+","+res.getString(3)+"\n");
		}
//		bw.close();
	}
	
	public HiveJdbcClient() throws SQLException {
		connectionHive();
	}

}