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

	public String tableName = "rawdata80w";
	public String filePath = "3-18.csv";
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
//		String sql = "create table "
//				+ tableName
//				+ "(XDRID string, sessionTime double,"
//				+ "ClientAddress string, ServerAddress string, IpID string,ClientPort int, "
//				+ "ConnectionTime double,"
//				+ "ClientTunnelAddress string, ServerTunnelAddress string, ClientToServerPackets int, ServerToClientPackets int, "
//				+ "ClientToServerOctets int, ServerToClientOctets int, AverageResponseTime int, TotalResp int, SessionLastUpdateTime double, "
//				+ "appId string, EndUserAddress string, MSISDN double, "
//				+ "IMSI double, IMEI double, APN string, CELL_SAC int, LAC_TAC_SID int, RAC_NID int, "
//				+ "NumOfUpdates int)"
//				+ "ROW FORMAT DELIMITED FIELDS TERMINATED BY ','";
//		// 2-24 2-25 2-26
//		String sql = "create table "
//				+ tableName
//				+ "(A string, B string,"
//				+ "C int, D string,"
//				+ "E int, F string,"
//				+ "G int,H string,"
//				+ "I int, J int,"
//				+ "K int, L int, "
//				+ "M int, N int, "
//				+ "O int, P int, "
//				+ "Q int, R int, "
//				+ "S int, T int, "
//				+ "U int, V int, "
//				+ "W int, X int, "
//				+ "Y int, Z int, "
//				+ "AA int, AB int, "
//				+ "AC int, AD int, "
//				+ "AE int, AF int, "
//				+ "AG string, AH int, "
//				+ "AI int, AJ int, "
//				+ "AK int, AL int, "
//				+ "AM int, AN int, "
//				+ "AO int, AP int, "
//				+ "AQ int, AR int, "
//				+ "AS int, AT int, "
//				+ "AU int, AV int, "
//				+ "AW int, AX int, "
//				+ "AY int, AZ int, "
//				+ "BA int, BB int, "
//				+ "BC int, BD int, "
//				+ "BE int, BF int, "
//				+ "BG int, BH int, "
//				+ "BI int, BJ int, "
//				+ "BK int, BL int, "
//				+ "BM int, BN int, "
//				+ "BO int, BP int, "
//				+ "BQ int, BR int, "
//				+ "BS int, BT int, "
//				+ "BU int, BV int, "
//				+ "BW int, BX int, "
//				+ "BY int, BZ int, "
//				+ "CA int,  CB int, "
//				+ "CC int,  CD int, "
//				+ "CE int,  CF string,"
//				+ "CG string, CH string,"
//				+ "CI string"
//				+ ")"
//				+ "ROW FORMAT DELIMITED FIELDS TERMINATED BY ','";
		// data 20w
				String sql = "create table "
						+ tableName
						+ "(A string, B string,"
						+ "C int, D string,"
						+ "E int, F string,"
						+ "G int,H string,"
						+ "I int, J int,"
						+ "K int, L int, "
						+ "M int, N int, "
						+ "O int, P int, "
						+ "Q int, R int, "
						+ "S int, T int, "
						+ "U int, V int, "
						+ "W int, X int, "
						+ "Y int, Z int, "
						+ "AA int, AB int, "
						+ "AC int, AD int, "
						+ "AE String, AF int, "
						+ "AG int, AH int, "
						+ "AI int, AJ int, "
						+ "AK int, AL int, "
						+ "AM int, AN int, "
						+ "AO int, AP int, "
						+ "AQ int, AR int, "
						+ "AS int, AT int, "
						+ "AU int, AV int, "
						+ "AW int, AX int, "
						+ "AY int, AZ int, "
						+ "BA int, BB int, "
						+ "BC int, BD int, "
						+ "BE int, BF int, "
						+ "BG int, BH int, "
						+ "BI int, BJ int, "
						+ "BK int, BL int, "
						+ "BM int, BN int, "
						+ "BO int, BP int, "
						+ "BQ int, BR int, "
						+ "BS int, BT int, "
						+ "BU int, BV int, "
						+ "BW int, BX int, "
						+ "BY int, BZ int, "
						+ "CA int,  CB int, "
						+ "CC int,  CD string, "
						+ "CE string,  CF string,"
						+ "CG string"
						+ ")"
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
		// XDR-sql
//		sql = "select MSISDN,count(*) from GENERIC_CorrelatedXDR where MSISDN is not null group by MSISDN";
//		sql = "select count(distinct MSISDN) from GENERIC_CorrelatedXDR where MSISDN is not null and MSISDN <> 0";
		// 2-24, 2-25, 2-26 data sql
//		sql = "select r2.a, r2.an, r2.ao, r2.ap, r2.aq, r2.ar, r2.as from rawdata0225 r2 where r2.a='592302000007946'";
//		sql = "select r1.a, (r1.an)/24, (r1.ao)/24, (r1.ap)/24, (r1.aq)/24, (r1.ar)/24, (r1.as)/24 from rawdata20w r1";
//		sql = "select r1.a, (r1.an+r2.an+r3.an)/72, (r1.ao+r2.ao+r3.ao)/72, (r1.ap+r2.ap+r3.ap)/72, (r1.aq+r2.aq+r3.aq)/72, (r1.ar+r2.ar+r3.ar)/72, (r1.as+r2.as+r3.as)/72 from rawdata0224 r1 join rawdata0225 r2 on r1.a=r2.a join rawdata0226 r3 on r1.a=r3.a";
//		sql = "select r1.a, r1.an+r2.an+r3.an, r1.ao+r2.ao+r3.ao, r1.ap+r2.ap+r3.ap, r1.aq+r2.aq+r3.aq, r1.ar+r2.ar+r3.ar, r1.as+r2.as+r3.as from rawdata0224 r1 join rawdata0225 r2 on r1.a=r2.a join rawdata0226 r3 on r1.a=r3.a";
		// 20w data sql
//		sql = "select a, al/24, an/24, am/24, ao/24, ap/24, aq/24 from rawdata20w limit 50";
		
		//无流量无位置变动的次数	被动流量无位置变动的次数	主动流量无位置变动的次数	无流量有位置变动的次数	被动流量有位置变动的次数	主动流量有位置变动的次数
//		sql = "select a, al/24, an/24, am/24, ao/24, ap/24, aq/24 from rawdata80w";
//		//第1时段无流量无位置变动的次数,第1时段被动流量无位置变动次数,第1时段主动流量无位置变动次数,第1时段无流量有位置变动次数,第1时段被动流量有位置变动次数,第1时段主动流量有位置变动次数
//		sql = "select a, ar/4, ax/4, bd/4, bj/4, bp/4, bv/4 from rawdata80w";
//		//第2时段无流量无位置变动的次数,第2时段被动流量无位置变动次数,第2时段主动流量无位置变动次数,第2时段无流量有位置变动次数,第2时段被动流量有位置变动次数,第2时段主动流量有位置变动次数
//		sql = "select a, as/4, ay/4, be/4, bk/4, bq/4, bw/4 from rawdata80w";
//		//第3时段无流量无位置变动的次数,第3时段被动流量无位置变动次数,第3时段主动流量无位置变动次数,第3时段无流量有位置变动次数,第3时段被动流量有位置变动次数,第3时段主动流量有位置变动次数
//		sql = "select a, at/4, az/4, bf/4, bl/4, br/4, bx/4 from rawdata80w";
//		//第4时段无流量无位置变动的次数,第4时段被动流量无位置变动次数,第4时段主动流量无位置变动次数,第4时段无流量有位置变动次数,第4时段被动流量有位置变动次数,第4时段主动流量有位置变动次数
//		sql = "select a, au/4, ba/4, bg/4, bm/4, bs/4, by/4 from rawdata80w";
//		//第5时段无流量无位置变动的次数,第5时段被动流量无位置变动次数,第5时段主动流量无位置变动次数,第5时段无流量有位置变动次数,第5时段被动流量有位置变动次数,第5时段主动流量有位置变动次数
//		sql = "select a, av/4, bb/4, bh/4, bn/4, bt/4, bz/4 from rawdata80w";
		//第6时段无流量无位置变动的次数,第6时段被动流量无位置变动次数,第6时段主动流量无位置变动次数,第6时段无流量有位置变动次数,第6时段被动流量有位置变动次数,第6时段主动流量有位置变动次数
//		sql = "select a, aw/4, bc/4, bi/4, bo/4, bu/4, ca/4 from rawdata80w";

//		sql = "select count(DISTINCT a) from rawdata80w where aw=3 and bc=1";
//		sql = "select count(DISTINCT a) from rawdata80w where al=0 & an=0 & am=0 & ao=1 & ap=3 & aq=0";
		sql = "select a, ar/24, as/24, at/24, au/24, av/24, aw/24, ax/24, ay/24, az/24, "
				+ "ba/24, bb/24, bc/24, bd/24, be/24, bf/24, bg/24, bh/24, bi/24, bj/24,"
				+ " bk/24, bl/24, bm/24, bn/24, bo/24, bp/24, bp/24, br/24, bs/24, bt/24,"
				+ " bu/24, bv/24, bw/24, bx/24, by/24, bz/24, ca/24 from rawdata80w";
		hiveJdbcClient.querySQL(sql);
	}
	
	@Test
	public void testCalcStates() throws SQLException, IOException{
		HiveJdbcClient hiveJdbcClient = new HiveJdbcClient();
//		hiveJdbcClient.calcStates();
		System.out.println("========================1========================");
		String[] a1 = {"ar", "ax", "bd", "bj", "bp", "bv"};
		hiveJdbcClient.calcStates2(1, a1);
		System.out.println("========================2========================");
		String[] a2 = {"as", "ay", "be", "bk", "bq", "bw"};
		hiveJdbcClient.calcStates2(2, a2);
//		System.out.println("========================3========================");
//		String[] a3 = {"at", "az", "bf", "bl", "br", "bx"};
//		hiveJdbcClient.calcStates2(3, a3);
		System.out.println("========================4========================");
		String[] a4 = {"au", "ba", "bg", "bm", "bs", "by"};
		hiveJdbcClient.calcStates2(4, a4);
		System.out.println("========================5========================");
		String[] a5 = {"av", "bb", "bh", "bn", "bt", "bz"};
		hiveJdbcClient.calcStates2(5, a5);
		System.out.println("========================6========================");
		String[] a6 = {"aw", "bc", "bi", "bo", "bu", "ca"};
		hiveJdbcClient.calcStates2(6, a6);
		
	}
}
