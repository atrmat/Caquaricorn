/**
 * 
 */
package atrmat;

import static org.junit.Assert.*;

import java.io.IOException;
import java.sql.SQLException;

import org.junit.Test;

/**
 * @author atrmat
 * 
 */
public class HiveJdbcClientTest {

	public String tableName = "GENERIC_CorrelatedXDR_processed_1_28";
	public String filePath = "GENERIC_CorrelatedXDR.csv";
	/**
	 * Test method for {@link atrmat.HiveJdbcClient#connectionHive()}.
	 * 
	 * @throws SQLException
	 */
	@Test
	public void testConnectionHive() throws SQLException {
		 fail("Not yet implemented");
		// connection to hive
//		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
//		hiveJdbcClient.connectionHive();
	}

	/**
	 * Test method for
	 * {@link atrmat.HiveJdbcClient#createHiveTable(java.lang.String)}.
	 * 
	 * @throws SQLException
	 */
	@Test
	public void testCreateHiveTable() throws SQLException {
		// fail("Not yet implemented");
		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
		hiveJdbcClient.connectionHive();
		// create table
//		String sql = "create table "
//				+ tableName
//				+ "(XDRID string, sessionStartTime double,sessionEndTime double, "
//				+ "status int, errorCode int, ClientAddress string, ServerAddress string, IpID string,ClientPort int, "
//				+ "ConnectionEndTime double, ConnectionStartTime double, ConnectionStatus int, ConnectionErrorcode int, "
//				+ "ClientTunnelAddress string, ServerTunnelAddress string, ClientToServerPackets int, ServerToClientPackets int, "
//				+ "ClientToServerOctets int, ServerToClientOctets int, AverageResponseTime int, TotalResp int, SessionLastUpdateTime double, "
//				+ "TotalEventCount int, NumSuccTrans int, ClientTeid double, ServerTeid double, EndPoinType_IpAddress_TeId string, "
//				+ "ECGI_RATType_TAC string, BSID string, IMSI_NSAPI string, appId string, EndUserAddress string, MSISDN double, "
//				+ "IMSI double, IMEI double, APN string, NSAPI_BearerId int, CELL_SAC int, LAC_TAC_SID int, RAC_NID int, "
//				+ "NumOfUpdates int, NAI string)"
//				+ "ROW FORMAT DELIMITED FIELDS TERMINATED BY ','";
		String sql = "create table "
				+ tableName
				+ "(XDRID string, sessionTime double,"
				+ "ClientAddress string, ServerAddress string, IpID string,ClientPort int, "
				+ "ConnectionTime double,"
				+ "ClientTunnelAddress string, ServerTunnelAddress string, ClientToServerPackets int, ServerToClientPackets int, "
				+ "ClientToServerOctets int, ServerToClientOctets int, AverageResponseTime int, TotalResp int, SessionLastUpdateTime double, "
				+ "appId string, EndUserAddress string, MSISDN double, "
				+ "IMSI double, IMEI double, APN string, CELL_SAC int, LAC_TAC_SID int, RAC_NID int, "
				+ "NumOfUpdates int)"
				+ "ROW FORMAT DELIMITED FIELDS TERMINATED BY ','";
		hiveJdbcClient.createHiveTable(tableName,sql);
	}

	/**
	 * Test method for
	 * {@link atrmat.HiveJdbcClient#insertTable(java.lang.String, java.lang.String)}
	 * .
	 * 
	 * @throws SQLException
	 */
	@Test
	public void testInsertTable() throws SQLException {
		// fail("Not yet implemented");
		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
		hiveJdbcClient.connectionHive();
		// insert
		hiveJdbcClient.insertTable(tableName, filePath);

	}

	/**
	 * Test method for
	 * {@link atrmat.HiveJdbcClient#queryTable(java.lang.String)}.
	 * 
	 * @throws SQLException
	 */
	@Test
	public void testQueryTable() throws SQLException {
		// fail("Not yet implemented");
		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
//		hiveJdbcClient.connectionHive();
		// query
		hiveJdbcClient.queryTable(tableName);
	}
	/**
	 * Test method for
	 * {@link atrmat.HiveJdbcClient#queryAttributeValue(java.lang.String)}.
	 * 
	 * @throws SQLException
	 */
	@Test
	public void testQueryAttributeValue() throws SQLException, IOException {
		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
		hiveJdbcClient.connectionHive();
		// query
		String attributeName = "MSISDN";
		hiveJdbcClient.queryAttributeValue(tableName, attributeName);
		attributeName = "IMEI";
		hiveJdbcClient.queryAttributeValue(tableName, attributeName);
		attributeName = "LAC/TAC/SID";
		hiveJdbcClient.queryAttributeValue(tableName, attributeName);
		attributeName = "CELL/SAC";
		hiveJdbcClient.queryAttributeValue(tableName, attributeName);
		attributeName = "appId";
		hiveJdbcClient.queryAttributeValue(tableName, attributeName);
	}
	
	/**
	 * Test method for
	 * {@link atrmat.HiveJdbcClient#querySQL(java.lang.String)}.
	 * 
	 * @throws SQLException
	 */
	@Test
	public void testQuerySQL() throws SQLException, IOException{
		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
		String sql;
//		sql = "select MSISDN,count(*) from GENERIC_CorrelatedXDR where MSISDN is not null group by MSISDN";
		sql = "select count(distinct MSISDN) from GENERIC_CorrelatedXDR where MSISDN is not null and MSISDN <> 0";
		hiveJdbcClient.querySQL(sql);
	}
}
