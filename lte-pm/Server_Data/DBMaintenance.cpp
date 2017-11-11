#include "stdafx.h"
#include "DBMaintenance.h"


DBMaintenance::DBMaintenance(void)
{
}


DBMaintenance::~DBMaintenance(void)
{
}
//------------------------------------------------------------------------------------------------------			
//函数功能：定时创建每月六个的表
//参    数:sTableTime:表时间【形式对应于数据库时间形式例如:201602】
//------------------------------------------------------------------------------------------------------
void DBMaintenance::CreateHourGranularityTable(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp;							//临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//对应于表CON_System中的变量名称
	CString              sPmName[6];                        //6个表名称
	list<CString>        listTemp[6];                       //各个表的Pm名称
	list<CString>        listCalculateTemp;                          //各个表的Pm名称
	COleDateTime         tableTime;                         //表时间
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{   //基本性能表创建
		tableTime+=COleDateTimeSpan(10,0,0,0);                           //20号以后加10天就获得下一个月
	    for (int control_count = 1;control_count <= 6; control_count ++)
	    {
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(1000);
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //表Pm原始数据排序
			v_sSql += "SELECT * FROM CON_Pm_LTE ";
			v_sSql+=" order by ztype,ztype_1,zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
				//保存数据
				if (v_sTable_Key=="MAC")
				{
					listTemp[0].push_back(v_sPm);
				}
				else if(v_sTable_Key=="RRC")
				{
					listTemp[1].push_back(v_sPm);
				}
				else if(v_sTable_Key=="PDCP")
				{
					listTemp[2].push_back(v_sPm);
				}
				else if(v_sTable_Key=="PHY")
				{
					listTemp[3].push_back(v_sPm);
				}
				else if(v_sTable_Key=="THO")
				{
					listTemp[4].push_back(v_sPm);
				}
				else if(v_sTable_Key=="TXT")
				{
					listTemp[5].push_back(v_sPm);
				}
				else
				{
					v_sTemp= "获取PM名称错误"+v_sPm;
					theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	  //log
				}	
				v_pRecordset->MoveNext();									      //
			}
			sPmName[0]="MAC";
			sPmName[1]="RRC";
			sPmName[2]="PDCP";
			sPmName[3]="PHY";
			sPmName[4]="THO";
			sPmName[5]="TXT";
			for (int i=0;i<6;i++)
			{
				v_sTemp=tableTime.Format("%Y%m");
				temp.Format("%d",control_count);
				v_sTemp += "_" + temp;
				v_sSql="CREATE TABLE Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//时间、小区
				while(listTemp[i].size()>0)
				{
					v_sSql+=",["+*(listTemp[i].begin())+"] [float] NULL";
					listTemp[i].pop_front();
				}
				v_sSql+=")\r\n";										         //结束
				v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_"+sPmName[i]+"_"+v_sTemp+" ON Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"(zCell)";	//建索引
				v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
				listTemp[i].clear();
			}
	    }
		//公式性能表创建
		for (int control_count = 1;control_count <= 6; control_count ++)
		{
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //表Pm原始数据排序
			v_sSql += "SELECT * FROM CON_Pm_LTE_Cal_Next_Month ";
			v_sSql+=" order by zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				listCalculateTemp.push_back(v_sPm);
				v_pRecordset->MoveNext();									      //
			}
			v_sTemp=tableTime.Format("%Y%m");
			temp.Format("%d",control_count);
			v_sTemp += "_" + temp;
			v_sSql="CREATE TABLE Pm_LTE_Cal_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//时间、小区
			while(listCalculateTemp.size()>0)
			{
				v_sSql+=",["+*(listCalculateTemp.begin())+"] [float] NULL";
				listCalculateTemp.pop_front();
			}
			v_sSql+=")\r\n";										         //结束
			v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_Cal_"+v_sTemp+" ON Pm_LTE_Cal_"+v_sTemp+"(zCell)";	//建索引
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			listCalculateTemp.clear();
		}
		v_cAdoConn.ExitConnect();										//断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：定时创建每月六个的表
//参    数:sTableTime:表时间【形式对应于数据库时间形式例如:201602】
//------------------------------------------------------------------------------------------------------
void DBMaintenance::CreateThisMonthHourGranularityTable(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp;							//临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//对应于表CON_System中的变量名称
	CString              sPmName[6];                        //6个表名称
	list<CString>        listTemp[6];                       //各个表的Pm名称
	list<CString>        listCalculateTemp;                          //各个表的Pm名称
	COleDateTime         tableTime;                         //表时间
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{   //基本性能表创建
		//tableTime+=COleDateTimeSpan(10,0,0,0);                           //20号以后加10天就获得下一个月
	    for (int control_count = 1;control_count <= 6; control_count ++)
	    {
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(1000);
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //表Pm原始数据排序
			v_sSql += "SELECT * FROM CON_Pm_LTE ";
			v_sSql+=" order by ztype,ztype_1,zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
				//保存数据
				if (v_sTable_Key=="MAC")
				{
					listTemp[0].push_back(v_sPm);
				}
				else if(v_sTable_Key=="RRC")
				{
					listTemp[1].push_back(v_sPm);
				}
				else if(v_sTable_Key=="PDCP")
				{
					listTemp[2].push_back(v_sPm);
				}
				else if(v_sTable_Key=="PHY")
				{
					listTemp[3].push_back(v_sPm);
				}
				else if(v_sTable_Key=="THO")
				{
					listTemp[4].push_back(v_sPm);
				}
				else if(v_sTable_Key=="TXT")
				{
					listTemp[5].push_back(v_sPm);
				}
				else
				{
					v_sTemp= "获取PM名称错误"+v_sPm;
					theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	  //log
				}	
				v_pRecordset->MoveNext();									      //
			}
			sPmName[0]="MAC";
			sPmName[1]="RRC";
			sPmName[2]="PDCP";
			sPmName[3]="PHY";
			sPmName[4]="THO";
			sPmName[5]="TXT";
			for (int i=0;i<6;i++)
			{
				v_sTemp=tableTime.Format("%Y%m");
				temp.Format("%d",control_count);
				v_sTemp += "_" + temp;
				v_sSql="CREATE TABLE Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//时间、小区
				while(listTemp[i].size()>0)
				{
					v_sSql+=",["+*(listTemp[i].begin())+"] [float] NULL";
					listTemp[i].pop_front();
				}
				v_sSql+=")\r\n";										         //结束
				v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_"+sPmName[i]+"_"+v_sTemp+" ON Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"(zCell)";	//建索引
				v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
				listTemp[i].clear();
			}
	    }
		//公式性能表创建
		for (int control_count = 1;control_count <= 6; control_count ++)
		{
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //表Pm原始数据排序
			v_sSql += "SELECT * FROM CON_Pm_LTE_Cal_Next_Month ";
			v_sSql+=" order by zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				listCalculateTemp.push_back(v_sPm);
				v_pRecordset->MoveNext();									      //
			}
			v_sTemp=tableTime.Format("%Y%m");
			temp.Format("%d",control_count);
			v_sTemp += "_" + temp;
			v_sSql="CREATE TABLE Pm_LTE_Cal_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//时间、小区
			while(listCalculateTemp.size()>0)
			{
				v_sSql+=",["+*(listCalculateTemp.begin())+"] [float] NULL";
				listCalculateTemp.pop_front();
			}
			v_sSql+=")\r\n";										         //结束
			v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_Cal_"+v_sTemp+" ON Pm_LTE_Cal_"+v_sTemp+"(zCell)";	//建索引
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			listCalculateTemp.clear();
		}
		v_cAdoConn.ExitConnect();										//断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：定时创建每月六个的表
//参    数:sTableTime:表时间【形式对应于数据库时间形式例如:20160201】
//------------------------------------------------------------------------------------------------------
void DBMaintenance::CreateQuarterGranularityTable(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp;							//临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//对应于表CON_System中的变量名称
	CString              sPmName[6];                        //6个表名称
	list<CString>        listTemp[6];                       //各个表的Pm名称
	list<CString>        listCalculateTemp;                          //各个表的Pm名称
	COleDateTime         tableTime;                         //表时间
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{   //基本性能表创建
		//tableTime+=COleDateTimeSpan(1,0,0,0);                           //加一天
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //表Pm原始数据排序
		v_sSql += "SELECT * FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
			//保存数据
			if (v_sTable_Key=="MAC")
			{
				listTemp[0].push_back(v_sPm);
			}
			else if(v_sTable_Key=="RRC")
			{
				listTemp[1].push_back(v_sPm);
			}
			else if(v_sTable_Key=="PDCP")
			{
				listTemp[2].push_back(v_sPm);
			}
			else if(v_sTable_Key=="PHY")
			{
				listTemp[3].push_back(v_sPm);
			}
			else if(v_sTable_Key=="THO")
			{
				listTemp[4].push_back(v_sPm);
			}
			else if(v_sTable_Key=="TXT")
			{
				listTemp[5].push_back(v_sPm);
			}
			else
			{
				v_sTemp= "获取PM名称错误"+v_sPm;
				theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	  //log
			}	
			v_pRecordset->MoveNext();									      //
		}
		sPmName[0]="MAC";
		sPmName[1]="RRC";
		sPmName[2]="PDCP";
		sPmName[3]="PHY";
		sPmName[4]="THO";
		sPmName[5]="TXT";
		for (int i=0;i<6;i++)
		{
			v_sTemp=tableTime.Format("%Y%m%d");
			v_sSql="CREATE TABLE Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//时间、小区
			while(listTemp[i].size()>0)
			{
				v_sSql+=",["+*(listTemp[i].begin())+"] [float] NULL";
				listTemp[i].pop_front();
			}
			v_sSql+=")\r\n";										         //结束
			v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_"+sPmName[i]+"_"+v_sTemp+" ON Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"(zCell)";	//建索引
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			listTemp[i].clear();
		}
		//公式性能表创建
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //表Pm原始数据排序
		v_sSql += "SELECT * FROM CON_Pm_LTE_Cal_This_Month ";
		v_sSql+=" order by zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			listCalculateTemp.push_back(v_sPm);
			v_pRecordset->MoveNext();									      //
		}
		v_sTemp=tableTime.Format("%Y%m%d");
		v_sSql="CREATE TABLE Pm_LTE_Cal_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//时间、小区
		while(listCalculateTemp.size()>0)
		{
			v_sSql+=",["+*(listCalculateTemp.begin())+"] [float] NULL";
			listCalculateTemp.pop_front();
		}
		v_sSql+=")\r\n";										         //结束
		v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_Cal_"+v_sTemp+" ON Pm_LTE_Cal_"+v_sTemp+"(zCell)";	//建索引
		v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
		listCalculateTemp.clear();
		v_cAdoConn.ExitConnect();										//断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：判断是否已经创建下月表
//参    数:sTableTime:表时间【形式对应于数据库时间形式例如:201602】
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeHourGranularityTableExist(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name";       //临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;   //对应于表CON_System中的变量名称
	COleDateTime    tableTime;										   //表时间
	list<CString>   listPmTable;									   //保存查询pm表名称
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{
		tableTime+=COleDateTimeSpan(10,0,0,0);                           //20号以后加10天就获得下一个月
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_%' order by [name]";     //表Pm原始数据排序
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		while(listPmTable.size()>0)
		{
			sTime = tableTime.Format("%Y%m");
			sTemp = *(listPmTable.begin());
			sTemp = sTemp.Right(8);
			sTemp.Delete(6,2);
			if(sTemp == sTime)
			{
				listPmTable.clear();
				return false;
			}				
			listPmTable.pop_front();
		}
		listPmTable.clear();
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：判断是否已经创建下月表
//参    数:sTableTime:表时间【形式对应于数据库时间形式例如:201602】
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeThisMonthHourGranularityTableExist(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name";       //临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;   //对应于表CON_System中的变量名称
	COleDateTime    tableTime;										   //表时间
	list<CString>   listPmTable;									   //保存查询pm表名称
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{
		//tableTime+=COleDateTimeSpan(10,0,0,0);                           //20号以后加10天就获得下一个月
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_%' order by [name]";     //表Pm原始数据排序
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		while(listPmTable.size()>0)
		{
			sTime = tableTime.Format("%Y%m");
			sTime = sTime + "_";
			sTemp = *(listPmTable.begin());
			sTemp = sTemp.Right(8);
			sTemp.Delete(7,1);
			if(sTemp == sTime)
			{
				listPmTable.clear();
				return false;
			}				
			listPmTable.pop_front();
		}
		listPmTable.clear();
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：判断是否已经创建下月表
//参    数:sTableTime:表时间【形式对应于数据库时间形式例如:20160201】
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeQuarterGranularityTableExist(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name";        //临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;    //对应于表CON_System中的变量名称
	COleDateTime    tableTime;										    //表时间
	list<CString>   listPmTable;									    //保存查询pm表名称
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{
		//tableTime += COleDateTimeSpan(1,0,0,0);                           //加一天
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_%' order by [name]";     //表Pm原始数据排序
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		while(listPmTable.size()>0)
		{
			sTime = tableTime.Format("%Y%m%d");
			sTemp = *(listPmTable.begin());
			sTemp = sTemp.Right(8);
			if(sTemp == sTime)
			{
				return false;
			}				
			listPmTable.pop_front();
		}
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：判断是否更新CON_Pm_LTE_Cal_Next_Month表？
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeCalculateTableUpdate()
{
	CString v_sSql,formula,formula_name,v_sTemp;
	_RecordsetPtr v_pRecordset;											//记录集
	map_Counter next_month_map_formula,month_map_formula;
	CMyADOConn v_cAdoConn;												//数据库实例
	map_Counter::iterator	v_piterator_map_formula;						//当月性能迭代
	map_Counter::iterator   v_piterator_map_formula_nextmonth;              //下月性能迭代
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
			month_map_formula.insert(pair<CString,CString> (formula_name,formula));
			v_pRecordset->MoveNext();
		}
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_Next_Month";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
			next_month_map_formula.insert(pair<CString,CString> (formula_name,formula));
			v_pRecordset->MoveNext();
		}
		v_cAdoConn.ExitConnect();

		v_piterator_map_formula = month_map_formula.begin();
		v_piterator_map_formula_nextmonth = next_month_map_formula.begin();
		if (month_map_formula.size() != next_month_map_formula.size())
		{
			next_month_map_formula.clear();
			month_map_formula.clear();
			return true;
		}
		while(v_piterator_map_formula != month_map_formula.end() || v_piterator_map_formula_nextmonth != next_month_map_formula.end())
		{
			if (v_piterator_map_formula->first == v_piterator_map_formula->first)
			{
				v_piterator_map_formula++;
				v_piterator_map_formula_nextmonth++;
				continue;
			}
			next_month_map_formula.clear();
			month_map_formula.clear();
			return true;
		}
		next_month_map_formula.clear();
		month_map_formula.clear();
		return false;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：判断是否更新CON_Pm_LTE_Cal_This_Month表？
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeThisCalculateTableUpdate()
{
	CString v_sSql,formula,formula_name,v_sTemp;
	_RecordsetPtr v_pRecordset;											//记录集
	map_Counter next_month_map_formula,month_map_formula;
	CMyADOConn v_cAdoConn;												//数据库实例
	map_Counter::iterator	v_piterator_map_formula;						//当月性能迭代
	map_Counter::iterator   v_piterator_map_formula_nextmonth;              //下月性能迭代
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_This_Month";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
			month_map_formula.insert(pair<CString,CString> (formula_name,formula));
			v_pRecordset->MoveNext();
		}
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_Next_Month";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
			next_month_map_formula.insert(pair<CString,CString> (formula_name,formula));
			v_pRecordset->MoveNext();
		}
		v_cAdoConn.ExitConnect();

		v_piterator_map_formula = month_map_formula.begin();
		v_piterator_map_formula_nextmonth = next_month_map_formula.begin();
		if (month_map_formula.size() != next_month_map_formula.size())
		{
			next_month_map_formula.clear();
			month_map_formula.clear();
			return true;
		}
		while(v_piterator_map_formula != month_map_formula.end() || v_piterator_map_formula_nextmonth != next_month_map_formula.end())
		{
			if (v_piterator_map_formula->first == v_piterator_map_formula->first)
			{
				v_piterator_map_formula++;
				v_piterator_map_formula_nextmonth++;
				continue;
			}
			next_month_map_formula.clear();
			month_map_formula.clear();
			return true;
		}
		next_month_map_formula.clear();
		month_map_formula.clear();
		return false;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：:创建下个月标准公式性能表,如果存在删除后新建
//------------------------------------------------------------------------------------------------------
void DBMaintenance::UpdateCONPmLTECalNextMonthTable()
{
	CString			      v_sTemp,v_sSql;					 //临时变量；SQL执行；
	CString			      v_sPm,sTemp,sKey="name";	         //对应于表CON_System中的变量名称
	list<CString>         listTemp;                          //各个表的Pm名称
	COleDateTime          tableTime;                         //表时间
	list<CString>         listPmTable;                       //保存查询pm表名称
	_RecordsetPtr v_pRecordset;								 //记录集
	CMyADOConn v_cAdoConn;									 //数据库实例
	bool       table_flag=false;
	try
	{
		sTemp="select [name] from [sysobjects] where [type] = 'u' and name like 'CON_Pm_LTE_Cal%' order by [name]";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sTemp);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //断开连接
		while(listPmTable.size()>0)
		{
			sTemp=*listPmTable.begin();
			if (sTemp == "CON_Pm_LTE_Cal_Next_Month")
			{
				table_flag=true;
			}
			listPmTable.pop_front();
		}
		if (table_flag == false)
		{
			CMyADOConn			v_cAdoConn;							 //数据库实例
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(100);
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_Next_Month FROM CON_Pm_LTE_Cal";     //创建下月标准性能计算表
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			v_cAdoConn.ExitConnect();	
		}		
		else
		{
			CMyADOConn			v_cAdoConn;							 //数据库实例
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(100);
			v_sSql = "drop table CON_Pm_LTE_Cal_Next_Month";
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_Next_Month FROM CON_Pm_LTE_Cal";     //创建下月标准性能计算表
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			v_cAdoConn.ExitConnect();
		}
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：:创建本月标准公式性能表,如果存在删除后新建
//------------------------------------------------------------------------------------------------------
void DBMaintenance::UpdateCONPmLTECalThisMonthTable()
{
	CString			      v_sTemp,v_sSql;					 //临时变量；SQL执行；
	CString			      v_sPm,sTemp,sKey="name";	         //对应于表CON_System中的变量名称
	list<CString>         listTemp;                          //各个表的Pm名称
	COleDateTime          tableTime;                         //表时间
	list<CString>         listPmTable;                       //保存查询pm表名称
	_RecordsetPtr		v_pRecordset;						 //记录集
	CMyADOConn			v_cAdoConn;							 //数据库实例
	bool       table_flag=false;
	try
	{
		sTemp="select [name] from [sysobjects] where [type] = 'u' and name like 'CON_Pm_LTE_Cal%' order by [name]";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sTemp);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //断开连接
		while(listPmTable.size()>0)
		{
			sTemp=*listPmTable.begin();
			if (sTemp == "CON_Pm_LTE_Cal_This_Month")
			{
				table_flag=true;
			}
			listPmTable.pop_front();
		}
		if (table_flag == false)
		{
			CMyADOConn			v_cAdoConn;							 //数据库实例
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(2000);
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_This_Month FROM CON_Pm_LTE_Cal_Next_Month";     //创建下月标准性能计算表
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			v_cAdoConn.ExitConnect();	
		}		
		else
		{
			CMyADOConn			v_cAdoConn;							 //数据库实例
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(2000);
			v_sSql = "drop table CON_Pm_LTE_Cal_This_Month";
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			v_cAdoConn.ExitConnect();
			Sleep(1000);
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(2000);
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_This_Month FROM CON_Pm_LTE_Cal_Next_Month";     //创建下月标准性能计算表
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
			v_cAdoConn.ExitConnect();
		}
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：判断是否已经创建下月表
//参    数:delete_time:表过期时间
//------------------------------------------------------------------------------------------------------
void DBMaintenance::DeleteStaleData(CString delete_time)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name",table_name;        //临时变量；SQL执行；
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;    //对应于表CON_System中的变量名称
	COleDateTime    tableTime,deleteTime;										    //表时间
	list<CString>   listPmTable;									    //保存查询pm表名称
	deleteTime.ParseDateTime(delete_time);
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	bool flag;
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_LTE%' order by [name]";     //表Pm原始数据排序
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		//v_cAdoConn.ExitConnect();
		while(listPmTable.size()>0)
		{
			sTime = tableTime.Format("%Y%m%d");
			sTemp = *(listPmTable.begin());
			table_name = sTemp;
			sTemp = sTemp.Right(8);
			sTemp = sTemp.Left(4) + "-" + sTemp.Mid(4,2) + "-" + sTemp.Right(2);
			//判断是否为时间
			// ParseDateTime()只能判断间隔符为“-”的日期字符串
			flag = tableTime.ParseDateTime(sTemp, VAR_DATEVALUEONLY);   
			if (flag)
			{
				tableTime.ParseDateTime(sTemp);	
				if (tableTime <= deleteTime)
				{
					//删除操作
					v_sSql = "drop table " + table_name;
					v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
				}
			} 
				
			listPmTable.pop_front();
		}
		v_cAdoConn.ExitConnect();	
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
		theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：CON_System表数据获取
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::GetSystemParameter()	
{
	_RecordsetPtr v_pRecordset;										//记录集
	CMyADOConn    v_cAdoConn;										//数据库实例
	CString       v_sSql="SELECT * FROM CON_System ";               //sql语句
	CString		  v_sTemp,sValue;							                //临时变量
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			int nTemp;
			COleDateTime timeTemp;
			nTemp=v_cAdoConn.GetField_Int("zPm_LTE_Month");
			sValue.Format("%d",nTemp);
			pm_save_month_ = sValue;
			nTemp=v_cAdoConn.GetField_Int("zPm_LTE_Day");
			sValue.Format("%d",nTemp);
			pm_save_day_ = sValue;
			nTemp=v_cAdoConn.GetField_Int("zPm_LTE_Day_FTP");
			sValue.Format("%d",nTemp);
			log_save_day_ = sValue;
			timeTemp=v_cAdoConn.GetField_DataTime("zPm_LTE_Time_Data");
			sValue=timeTemp.Format("%Y-%m-%d %H:%M:%S");
			granularity_time_ = sValue;
			v_pRecordset->MoveNext();									//
		}
		v_cAdoConn.ExitConnect();										//断开连接
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//关闭数据库
		CString v_sError = (char *)e.Description();						//获得：出错信息
		theApp.m_MyLog.WriteText("异常："+v_sError+"  "+v_sSql+"\r\n");	//log
		return false;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能:数据变量初始化
//------------------------------------------------------------------------------------------------------
void DBMaintenance::Init()
{
	GetSystemParameter();
}

//------------------------------------------------------------------------------------------------------			
//函数功能：给定目录名称删除目录及目录下GZ文件
//参    数：sFolderName :目录名称；sFolderPath：文件目录
//------------------------------------------------------------------------------------------------------
void DBMaintenance::DeleteFolderandFile(CString sFolderName,CString sFolderPath)
{
	// TODO: 在此添加控件通知处理程序代码
	CFileFind   tempFind;  
	CString     temp;                           
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sFolderPath+"\\"+sFolderName+"\\*.*"));//判断文件夹
	while(IsFinded)														//有效文件夹？？？
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							//寻找下一个文件
		CString sFileName= tempFind.GetFileName();				    //获取：文件名称/目录名称
		if(!tempFind.IsDots())											//..或者.？？？  
		{
			CString sFileName=  tempFind.GetFileName();				//获取：文件名称/目录名称
			if(tempFind.IsDirectory())									//目录？？？
				RemoveDirectory((LPCSTR)(sFolderPath+"\\"+sFolderName+"\\"+sFileName));		//删除目录
			else
			{	
				temp = sFolderPath+"\\"+sFolderName+"\\"+sFileName;
				DeleteFile((LPCSTR)temp);		//删除文件			
			}
		}
	} 
	RemoveDirectory((LPCSTR)(sFolderPath+"\\"+sFolderName));		    //删除目录
	tempFind.Close(); 													//当寻找文件的事件结束就结束寻找功能
}

//------------------------------------------------------------------------------------------------------			
//函数功能：给定目录名称删除目录及目录下GZ文件
//参    数：grantularity_time :粒度时间；sFolderPath：文件目录
//------------------------------------------------------------------------------------------------------
void DBMaintenance::DeleteBulkInsertFile(CString grantularity_time,CString sFolderPath)
{
	// TODO: 在此添加控件通知处理程序代码
	CFileFind   tempFind;  
	CString     file_path,file_time;
	COleDateTime ole_file_time,delete_grantularity_time;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sFolderPath + "\\*.*"));    //判断文件夹
	while(IsFinded)														         //有效文件夹？？？
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							         //寻找下一个文件
		CString sFileName= tempFind.GetFileName();				                 //获取：文件名称/目录名称
		if(!tempFind.IsDots())											         //..或者.？？？  
		{
			CString sFileName=  tempFind.GetFileName();				            //获取：文件名称/目录名称
			if(tempFind.IsDirectory())									        //目录？？？
				RemoveDirectory((LPCSTR)(sFolderPath + "\\"+sFileName));		//删除目录
			else
			{	
				file_time = sFileName.Right(20);
				file_time = file_time.Left(13);
				file_time.Replace("-"," ");
				file_time.Insert(4,"-");
				file_time.Insert(7,"-");
				file_time.Insert(13,":");
				file_time.Insert(16,":00");
				ole_file_time.ParseDateTime(file_time);
                delete_grantularity_time.ParseDateTime(grantularity_time);
				delete_grantularity_time -= COleDateTimeSpan(7,0,0,0);
				if (delete_grantularity_time > ole_file_time)
				{
					file_path = sFolderPath + "\\"  + sFileName;
					DeleteFile((LPCSTR)file_path);		//删除文件
				}			
			}
		}
	} 
	tempFind.Close(); 													//当寻找文件的事件结束就结束寻找功能
}