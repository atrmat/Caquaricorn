package atrmat;

import java.sql.SQLException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.DriverManager;
 
public class HiveJdbcClient {
  private static String driverName = "org.apache.hadoop.hive.jdbc.HiveDriver";
 
  /**
   * using the hive-jdbc client to insert GENERIC_CorrelatedXDR data into the hive
   * @param args
   * @throws SQLException
   */
  public static void main(String[] args) throws SQLException {
    try {
      Class.forName(driverName);
    } catch (ClassNotFoundException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
      System.exit(1);
    }
    ResultSet res;
    //replace "hive" here with the name of the user the queries should run as
    Connection con = DriverManager.getConnection("jdbc:hive://192.168.1.28:10000/default", "root", "1");
    Statement stmt = con.createStatement();
    res = stmt.executeQuery("show tables 'pokes'");
    if (res.next()){
    	System.out.println(res.getString(1));
    }
    String tableName = "GENERIC_CorrelatedXDR";
    stmt.execute("drop table if exists " + tableName);
    // create the table with the special terminate symbol: ',' between the column
    // the column in csv file is dot by ',' , e.g., example csv data file is as follows
    // 1,foo
    // 2,bar
    // 3,txt
    stmt.execute("create table " + tableName + 
    		"(XDRID string, sessionStartTime double,sessionEndTime double, "
    		+ "status int, errorCode int, ClientAddress string, ServerAddress string, IpID string,ClientPort int, "
    		+ "ConnectionEndTime double, ConnectionStartTime double, ConnectionStatus int, ConnectionErrorcode int, "
    		+ "ClientTunnelAddress string, ServerTunnelAddress string, ClientToServerPackets int, ServerToClientPackets int, "
    		+ "ClientToServerOctets int, ServerToClientOctets int, AverageResponseTime int, TotalResp int, SessionLastUpdateTime double, "
    		+ "TotalEventCount int, NumSuccTrans int, ClientTeid double, ServerTeid double, EndPoinType_IpAddress_TeId string, "
    		+ "ECGI_RATType_TAC string, BSID string, IMSI_NSAPI string, appId string, EndUserAddress string, MSISDN double, "
    		+ "IMSI double, IMEI double, APN string, NSAPI_BearerId int, CELL_SAC int, LAC_TAC_SID int, RAC_NID int, "
    		+ "NumOfUpdates int, NAI string)"
    		+"ROW FORMAT DELIMITED FIELDS TERMINATED BY ','");
    // show tables
    String sql = "show tables '" + tableName + "'";
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
    stmt.executeQuery("show tables");
 
    // load data into table
    // NOTE: file path has to be local to the hive server: e.g.,
    // /usr/local/src/hive-0.11.0-bin/a.txt
    // NOTE: a.txt is a ctrl-A separated file with two fields per line
    // echo -e '1\x01foo' > a.txt
    // echo -e '2\x01bar' >> a.txt
    String filepath = "GENERIC_CorrelatedXDR.csv";
    sql = "load data local inpath '" + filepath + "' into table " + tableName;
    System.out.println("Running: " + sql);
    stmt.execute(sql);
 
    // select * query
    sql = "select * from " + tableName;
    System.out.println("Running: " + sql);
    res = stmt.executeQuery(sql);
    while (res.next()) {
//      System.out.println(String.valueOf(res.getInt(1)) + "\t" + res.getString(2));
    	System.out.println(res.getString(1));
    }
 
    // regular hive query
    sql = "select count(1) from " + tableName;
    System.out.println("Running: " + sql);
    res = stmt.executeQuery(sql);
    while (res.next()) {
      System.out.println(res.getString(1));
    }
  }
}