#include "stdafx.h"
#include "DBMaintenance.h"


DBMaintenance::DBMaintenance(void)
{
}


DBMaintenance::~DBMaintenance(void)
{
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ���ʱ����ÿ�������ı�
//��    ��:sTableTime:��ʱ�䡾��ʽ��Ӧ�����ݿ�ʱ����ʽ����:201602��
//------------------------------------------------------------------------------------------------------
void DBMaintenance::CreateHourGranularityTable(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp;							//��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��Ӧ�ڱ�CON_System�еı�������
	CString              sPmName[6];                        //6��������
	list<CString>        listTemp[6];                       //�������Pm����
	list<CString>        listCalculateTemp;                          //�������Pm����
	COleDateTime         tableTime;                         //��ʱ��
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{   //�������ܱ���
		tableTime+=COleDateTimeSpan(10,0,0,0);                           //20���Ժ��10��ͻ����һ����
	    for (int control_count = 1;control_count <= 6; control_count ++)
	    {
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(1000);
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
			v_sSql += "SELECT * FROM CON_Pm_LTE ";
			v_sSql+=" order by ztype,ztype_1,zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
				//��������
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
					v_sTemp= "��ȡPM���ƴ���"+v_sPm;
					theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	  //log
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
				v_sSql="CREATE TABLE Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//ʱ�䡢С��
				while(listTemp[i].size()>0)
				{
					v_sSql+=",["+*(listTemp[i].begin())+"] [float] NULL";
					listTemp[i].pop_front();
				}
				v_sSql+=")\r\n";										         //����
				v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_"+sPmName[i]+"_"+v_sTemp+" ON Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"(zCell)";	//������
				v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
				listTemp[i].clear();
			}
	    }
		//��ʽ���ܱ���
		for (int control_count = 1;control_count <= 6; control_count ++)
		{
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
			v_sSql += "SELECT * FROM CON_Pm_LTE_Cal_Next_Month ";
			v_sSql+=" order by zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				listCalculateTemp.push_back(v_sPm);
				v_pRecordset->MoveNext();									      //
			}
			v_sTemp=tableTime.Format("%Y%m");
			temp.Format("%d",control_count);
			v_sTemp += "_" + temp;
			v_sSql="CREATE TABLE Pm_LTE_Cal_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//ʱ�䡢С��
			while(listCalculateTemp.size()>0)
			{
				v_sSql+=",["+*(listCalculateTemp.begin())+"] [float] NULL";
				listCalculateTemp.pop_front();
			}
			v_sSql+=")\r\n";										         //����
			v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_Cal_"+v_sTemp+" ON Pm_LTE_Cal_"+v_sTemp+"(zCell)";	//������
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			listCalculateTemp.clear();
		}
		v_cAdoConn.ExitConnect();										//�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���ʱ����ÿ�������ı�
//��    ��:sTableTime:��ʱ�䡾��ʽ��Ӧ�����ݿ�ʱ����ʽ����:201602��
//------------------------------------------------------------------------------------------------------
void DBMaintenance::CreateThisMonthHourGranularityTable(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp;							//��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��Ӧ�ڱ�CON_System�еı�������
	CString              sPmName[6];                        //6��������
	list<CString>        listTemp[6];                       //�������Pm����
	list<CString>        listCalculateTemp;                          //�������Pm����
	COleDateTime         tableTime;                         //��ʱ��
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{   //�������ܱ���
		//tableTime+=COleDateTimeSpan(10,0,0,0);                           //20���Ժ��10��ͻ����һ����
	    for (int control_count = 1;control_count <= 6; control_count ++)
	    {
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(1000);
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
			v_sSql += "SELECT * FROM CON_Pm_LTE ";
			v_sSql+=" order by ztype,ztype_1,zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
				//��������
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
					v_sTemp= "��ȡPM���ƴ���"+v_sPm;
					theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	  //log
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
				v_sSql="CREATE TABLE Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//ʱ�䡢С��
				while(listTemp[i].size()>0)
				{
					v_sSql+=",["+*(listTemp[i].begin())+"] [float] NULL";
					listTemp[i].pop_front();
				}
				v_sSql+=")\r\n";										         //����
				v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_"+sPmName[i]+"_"+v_sTemp+" ON Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"(zCell)";	//������
				v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
				listTemp[i].clear();
			}
	    }
		//��ʽ���ܱ���
		for (int control_count = 1;control_count <= 6; control_count ++)
		{
			v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
			v_sSql += "SELECT * FROM CON_Pm_LTE_Cal_Next_Month ";
			v_sSql+=" order by zPm";
			v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
			while(!v_pRecordset->adoEOF)
			{
				v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
				listCalculateTemp.push_back(v_sPm);
				v_pRecordset->MoveNext();									      //
			}
			v_sTemp=tableTime.Format("%Y%m");
			temp.Format("%d",control_count);
			v_sTemp += "_" + temp;
			v_sSql="CREATE TABLE Pm_LTE_Cal_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//ʱ�䡢С��
			while(listCalculateTemp.size()>0)
			{
				v_sSql+=",["+*(listCalculateTemp.begin())+"] [float] NULL";
				listCalculateTemp.pop_front();
			}
			v_sSql+=")\r\n";										         //����
			v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_Cal_"+v_sTemp+" ON Pm_LTE_Cal_"+v_sTemp+"(zCell)";	//������
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			listCalculateTemp.clear();
		}
		v_cAdoConn.ExitConnect();										//�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���ʱ����ÿ�������ı�
//��    ��:sTableTime:��ʱ�䡾��ʽ��Ӧ�����ݿ�ʱ����ʽ����:20160201��
//------------------------------------------------------------------------------------------------------
void DBMaintenance::CreateQuarterGranularityTable(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp;							//��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��Ӧ�ڱ�CON_System�еı�������
	CString              sPmName[6];                        //6��������
	list<CString>        listTemp[6];                       //�������Pm����
	list<CString>        listCalculateTemp;                          //�������Pm����
	COleDateTime         tableTime;                         //��ʱ��
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{   //�������ܱ���
		//tableTime+=COleDateTimeSpan(1,0,0,0);                           //��һ��
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
		v_sSql += "SELECT * FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
			//��������
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
				v_sTemp= "��ȡPM���ƴ���"+v_sPm;
				theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	  //log
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
			v_sSql="CREATE TABLE Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//ʱ�䡢С��
			while(listTemp[i].size()>0)
			{
				v_sSql+=",["+*(listTemp[i].begin())+"] [float] NULL";
				listTemp[i].pop_front();
			}
			v_sSql+=")\r\n";										         //����
			v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_"+sPmName[i]+"_"+v_sTemp+" ON Pm_LTE_"+sPmName[i]+"_"+v_sTemp+"(zCell)";	//������
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			listTemp[i].clear();
		}
		//��ʽ���ܱ���
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
		v_sSql += "SELECT * FROM CON_Pm_LTE_Cal_This_Month ";
		v_sSql+=" order by zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			listCalculateTemp.push_back(v_sPm);
			v_pRecordset->MoveNext();									      //
		}
		v_sTemp=tableTime.Format("%Y%m%d");
		v_sSql="CREATE TABLE Pm_LTE_Cal_"+v_sTemp+"([zTime] [datetime] NOT NULL,[zCell] [int] NOT NULL";	//ʱ�䡢С��
		while(listCalculateTemp.size()>0)
		{
			v_sSql+=",["+*(listCalculateTemp.begin())+"] [float] NULL";
			listCalculateTemp.pop_front();
		}
		v_sSql+=")\r\n";										         //����
		v_sSql+="CREATE NONCLUSTERED INDEX PK_Pm_LTE_Cal_"+v_sTemp+" ON Pm_LTE_Cal_"+v_sTemp+"(zCell)";	//������
		v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
		listCalculateTemp.clear();
		v_cAdoConn.ExitConnect();										//�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ��ж��Ƿ��Ѿ��������±�
//��    ��:sTableTime:��ʱ�䡾��ʽ��Ӧ�����ݿ�ʱ����ʽ����:201602��
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeHourGranularityTableExist(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name";       //��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;   //��Ӧ�ڱ�CON_System�еı�������
	COleDateTime    tableTime;										   //��ʱ��
	list<CString>   listPmTable;									   //�����ѯpm������
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{
		tableTime+=COleDateTimeSpan(10,0,0,0);                           //20���Ժ��10��ͻ����һ����
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_%' order by [name]";     //��Pmԭʼ��������
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //ִ��SELETE���	
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
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ��ж��Ƿ��Ѿ��������±�
//��    ��:sTableTime:��ʱ�䡾��ʽ��Ӧ�����ݿ�ʱ����ʽ����:201602��
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeThisMonthHourGranularityTableExist(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name";       //��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;   //��Ӧ�ڱ�CON_System�еı�������
	COleDateTime    tableTime;										   //��ʱ��
	list<CString>   listPmTable;									   //�����ѯpm������
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{
		//tableTime+=COleDateTimeSpan(10,0,0,0);                           //20���Ժ��10��ͻ����һ����
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_%' order by [name]";     //��Pmԭʼ��������
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //ִ��SELETE���	
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
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ��ж��Ƿ��Ѿ��������±�
//��    ��:sTableTime:��ʱ�䡾��ʽ��Ӧ�����ݿ�ʱ����ʽ����:20160201��
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeQuarterGranularityTableExist(CString sTableTime)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name";        //��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;    //��Ӧ�ڱ�CON_System�еı�������
	COleDateTime    tableTime;										    //��ʱ��
	list<CString>   listPmTable;									    //�����ѯpm������
	tableTime.ParseDateTime(sTableTime);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{
		//tableTime += COleDateTimeSpan(1,0,0,0);                           //��һ��
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_%' order by [name]";     //��Pmԭʼ��������
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //ִ��SELETE���	
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
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ��ж��Ƿ����CON_Pm_LTE_Cal_Next_Month��
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeCalculateTableUpdate()
{
	CString v_sSql,formula,formula_name,v_sTemp;
	_RecordsetPtr v_pRecordset;											//��¼��
	map_Counter next_month_map_formula,month_map_formula;
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map_Counter::iterator	v_piterator_map_formula;						//�������ܵ���
	map_Counter::iterator   v_piterator_map_formula_nextmonth;              //�������ܵ���
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//��ʽ����
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //��ʽ����ֵ
			month_map_formula.insert(pair<CString,CString> (formula_name,formula));
			v_pRecordset->MoveNext();
		}
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_Next_Month";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//��ʽ����
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //��ʽ����ֵ
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
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ��ж��Ƿ����CON_Pm_LTE_Cal_This_Month��
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::JudgeThisCalculateTableUpdate()
{
	CString v_sSql,formula,formula_name,v_sTemp;
	_RecordsetPtr v_pRecordset;											//��¼��
	map_Counter next_month_map_formula,month_map_formula;
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map_Counter::iterator	v_piterator_map_formula;						//�������ܵ���
	map_Counter::iterator   v_piterator_map_formula_nextmonth;              //�������ܵ���
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_This_Month";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//��ʽ����
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //��ʽ����ֵ
			month_map_formula.insert(pair<CString,CString> (formula_name,formula));
			v_pRecordset->MoveNext();
		}
		v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_Next_Month";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
		while (!v_pRecordset->adoEOF)
		{
			formula_name = v_cAdoConn.GetField_String("zPm");			//��ʽ����
			formula = v_cAdoConn.GetField_String("zPm_Formula");			       //��ʽ����ֵ
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
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�:�����¸��±�׼��ʽ���ܱ�,�������ɾ�����½�
//------------------------------------------------------------------------------------------------------
void DBMaintenance::UpdateCONPmLTECalNextMonthTable()
{
	CString			      v_sTemp,v_sSql;					 //��ʱ������SQLִ�У�
	CString			      v_sPm,sTemp,sKey="name";	         //��Ӧ�ڱ�CON_System�еı�������
	list<CString>         listTemp;                          //�������Pm����
	COleDateTime          tableTime;                         //��ʱ��
	list<CString>         listPmTable;                       //�����ѯpm������
	_RecordsetPtr v_pRecordset;								 //��¼��
	CMyADOConn v_cAdoConn;									 //���ݿ�ʵ��
	bool       table_flag=false;
	try
	{
		sTemp="select [name] from [sysobjects] where [type] = 'u' and name like 'CON_Pm_LTE_Cal%' order by [name]";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sTemp);		      //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //�Ͽ�����
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
			CMyADOConn			v_cAdoConn;							 //���ݿ�ʵ��
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(100);
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_Next_Month FROM CON_Pm_LTE_Cal";     //�������±�׼���ܼ����
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			v_cAdoConn.ExitConnect();	
		}		
		else
		{
			CMyADOConn			v_cAdoConn;							 //���ݿ�ʵ��
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(100);
			v_sSql = "drop table CON_Pm_LTE_Cal_Next_Month";
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_Next_Month FROM CON_Pm_LTE_Cal";     //�������±�׼���ܼ����
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			v_cAdoConn.ExitConnect();
		}
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�:�������±�׼��ʽ���ܱ�,�������ɾ�����½�
//------------------------------------------------------------------------------------------------------
void DBMaintenance::UpdateCONPmLTECalThisMonthTable()
{
	CString			      v_sTemp,v_sSql;					 //��ʱ������SQLִ�У�
	CString			      v_sPm,sTemp,sKey="name";	         //��Ӧ�ڱ�CON_System�еı�������
	list<CString>         listTemp;                          //�������Pm����
	COleDateTime          tableTime;                         //��ʱ��
	list<CString>         listPmTable;                       //�����ѯpm������
	_RecordsetPtr		v_pRecordset;						 //��¼��
	CMyADOConn			v_cAdoConn;							 //���ݿ�ʵ��
	bool       table_flag=false;
	try
	{
		sTemp="select [name] from [sysobjects] where [type] = 'u' and name like 'CON_Pm_LTE_Cal%' order by [name]";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sTemp);		      //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String(sKey);
			listPmTable.push_back(sTemp);
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //�Ͽ�����
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
			CMyADOConn			v_cAdoConn;							 //���ݿ�ʵ��
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(2000);
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_This_Month FROM CON_Pm_LTE_Cal_Next_Month";     //�������±�׼���ܼ����
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			v_cAdoConn.ExitConnect();	
		}		
		else
		{
			CMyADOConn			v_cAdoConn;							 //���ݿ�ʵ��
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(2000);
			v_sSql = "drop table CON_Pm_LTE_Cal_This_Month";
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			v_cAdoConn.ExitConnect();
			Sleep(1000);
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(2000);
			v_sSql="SELECT * INTO CON_Pm_LTE_Cal_This_Month FROM CON_Pm_LTE_Cal_Next_Month";     //�������±�׼���ܼ����
			v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
			v_cAdoConn.ExitConnect();
		}
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ��ж��Ƿ��Ѿ��������±�
//��    ��:delete_time:�����ʱ��
//------------------------------------------------------------------------------------------------------
void DBMaintenance::DeleteStaleData(CString delete_time)
{
	CString			v_sTemp,v_sSql,temp,sTime,sTemp,sKey="name",table_name;        //��ʱ������SQLִ�У�
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;    //��Ӧ�ڱ�CON_System�еı�������
	COleDateTime    tableTime,deleteTime;										    //��ʱ��
	list<CString>   listPmTable;									    //�����ѯpm������
	deleteTime.ParseDateTime(delete_time);
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	bool flag;
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql = "select [name] from [sysobjects] where [type] = 'u' and name like 'Pm_LTE%' order by [name]";     //��Pmԭʼ��������
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		      //ִ��SELETE���	
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
			//�ж��Ƿ�Ϊʱ��
			// ParseDateTime()ֻ���жϼ����Ϊ��-���������ַ���
			flag = tableTime.ParseDateTime(sTemp, VAR_DATEVALUEONLY);   
			if (flag)
			{
				tableTime.ParseDateTime(sTemp);	
				if (tableTime <= deleteTime)
				{
					//ɾ������
					v_sSql = "drop table " + table_name;
					v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
				}
			} 
				
			listPmTable.pop_front();
		}
		v_cAdoConn.ExitConnect();	
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�CON_System�����ݻ�ȡ
//------------------------------------------------------------------------------------------------------
bool DBMaintenance::GetSystemParameter()	
{
	_RecordsetPtr v_pRecordset;										//��¼��
	CMyADOConn    v_cAdoConn;										//���ݿ�ʵ��
	CString       v_sSql="SELECT * FROM CON_System ";               //sql���
	CString		  v_sTemp,sValue;							                //��ʱ����
	try
	{
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
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
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�ر����ݿ�
		CString v_sError = (char *)e.Description();						//��ã�������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+v_sSql+"\r\n");	//log
		return false;
	}
}

//------------------------------------------------------------------------------------------------------			
//��������:���ݱ�����ʼ��
//------------------------------------------------------------------------------------------------------
void DBMaintenance::Init()
{
	GetSystemParameter();
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�����Ŀ¼����ɾ��Ŀ¼��Ŀ¼��GZ�ļ�
//��    ����sFolderName :Ŀ¼���ƣ�sFolderPath���ļ�Ŀ¼
//------------------------------------------------------------------------------------------------------
void DBMaintenance::DeleteFolderandFile(CString sFolderName,CString sFolderPath)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileFind   tempFind;  
	CString     temp;                           
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sFolderPath+"\\"+sFolderName+"\\*.*"));//�ж��ļ���
	while(IsFinded)														//��Ч�ļ��У�����
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							//Ѱ����һ���ļ�
		CString sFileName= tempFind.GetFileName();				    //��ȡ���ļ�����/Ŀ¼����
		if(!tempFind.IsDots())											//..����.������  
		{
			CString sFileName=  tempFind.GetFileName();				//��ȡ���ļ�����/Ŀ¼����
			if(tempFind.IsDirectory())									//Ŀ¼������
				RemoveDirectory((LPCSTR)(sFolderPath+"\\"+sFolderName+"\\"+sFileName));		//ɾ��Ŀ¼
			else
			{	
				temp = sFolderPath+"\\"+sFolderName+"\\"+sFileName;
				DeleteFile((LPCSTR)temp);		//ɾ���ļ�			
			}
		}
	} 
	RemoveDirectory((LPCSTR)(sFolderPath+"\\"+sFolderName));		    //ɾ��Ŀ¼
	tempFind.Close(); 													//��Ѱ���ļ����¼������ͽ���Ѱ�ҹ���
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�����Ŀ¼����ɾ��Ŀ¼��Ŀ¼��GZ�ļ�
//��    ����grantularity_time :����ʱ�䣻sFolderPath���ļ�Ŀ¼
//------------------------------------------------------------------------------------------------------
void DBMaintenance::DeleteBulkInsertFile(CString grantularity_time,CString sFolderPath)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileFind   tempFind;  
	CString     file_path,file_time;
	COleDateTime ole_file_time,delete_grantularity_time;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sFolderPath + "\\*.*"));    //�ж��ļ���
	while(IsFinded)														         //��Ч�ļ��У�����
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							         //Ѱ����һ���ļ�
		CString sFileName= tempFind.GetFileName();				                 //��ȡ���ļ�����/Ŀ¼����
		if(!tempFind.IsDots())											         //..����.������  
		{
			CString sFileName=  tempFind.GetFileName();				            //��ȡ���ļ�����/Ŀ¼����
			if(tempFind.IsDirectory())									        //Ŀ¼������
				RemoveDirectory((LPCSTR)(sFolderPath + "\\"+sFileName));		//ɾ��Ŀ¼
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
					DeleteFile((LPCSTR)file_path);		//ɾ���ļ�
				}			
			}
		}
	} 
	tempFind.Close(); 													//��Ѱ���ļ����¼������ͽ���Ѱ�ҹ���
}