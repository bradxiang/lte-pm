#include "stdafx.h"
#include "PerformanceWarning.h"
#include "Server_Data.h"

CPerformanceWarning::CPerformanceWarning(void)
{
}


CPerformanceWarning::~CPerformanceWarning(void)
{
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:�����ݿ��ж�ȡ��������Ԥ������
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::GetRegionPmWarning()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString v_sSql,name,stemp;
	int save_flage,alarm_type,alarm_flag;                   //���ݿ����
	double alarm_value;
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map<CString,CString> map_temporary;
	region_performance_warning_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);

	v_sSql="SELECT zPm,zSave_Flag,zAlarm_Flag,zAlarm_Value,zAlarm_Type FROM CON_Pm_LTE_Region";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
	while (!v_pRecordset->adoEOF)
	{
		name = v_cAdoConn.GetField_String("zPm");			         //��ʽ����
		alarm_flag = v_cAdoConn.GetField_Int("zAlarm_Flag");
		if (alarm_flag == 1)
		{			
			map_temporary.clear();
			save_flage = v_cAdoConn.GetField_Int("zSave_Flag");	
			stemp.Format("%d",save_flage);
			map_temporary.insert(pair<CString,CString>("zSave_Flag",stemp));
			alarm_value = v_cAdoConn.GetField_Double("zAlarm_Value");
			stemp.Format("%f",alarm_value);
			map_temporary.insert(pair<CString,CString>("zAlarm_Value",stemp));
			alarm_type =  v_cAdoConn.GetField_Int("zAlarm_Type");
			stemp.Format("%d",alarm_type);
			map_temporary.insert(pair<CString,CString>("zAlarm_Type",stemp));
			region_performance_warning_.insert(pair<CString,map<CString,CString>>(name,map_temporary));
			v_pRecordset->MoveNext();
		}
		else
		{
         v_pRecordset->MoveNext();
		}
		
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:�����ݿ��ж�ȡ��ʽ����Ԥ������
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::GetFormulaPmWarning()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString v_sSql,name,stemp;
	int save_flage,alarm_type,alarm_flag;                   //���ݿ����
	double alarm_value;
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map<CString,CString> map_temporary;
	formula_performance_warning_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);

	v_sSql="SELECT zPm,zSave_Flag,zAlarm_Flag,zAlarm_Value,zAlarm_Type FROM CON_Pm_LTE_Cal";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
	while (!v_pRecordset->adoEOF)
	{
		name = v_cAdoConn.GetField_String("zPm");			         //��ʽ����
		alarm_flag = v_cAdoConn.GetField_Int("zAlarm_Flag");
		if (alarm_flag == 1)
		{			
			map_temporary.clear();
			save_flage = v_cAdoConn.GetField_Int("zSave_Flag");	
			stemp.Format("%d",save_flage);
			map_temporary.insert(pair<CString,CString>("zSave_Flag",stemp));
			alarm_value = v_cAdoConn.GetField_Double("zAlarm_Value");
			stemp.Format("%f",alarm_value);
			map_temporary.insert(pair<CString,CString>("zAlarm_Value",stemp));
			alarm_type =  v_cAdoConn.GetField_Int("zAlarm_Type");
			stemp.Format("%d",alarm_type);
			map_temporary.insert(pair<CString,CString>("zAlarm_Type",stemp));
			formula_performance_warning_.insert(pair<CString,map<CString,CString>>(name,map_temporary));
			v_pRecordset->MoveNext();
		}
		else
		{
			v_pRecordset->MoveNext();
		}

	}
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:�����ݿ��ж�ȡ��������Ԥ������
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::GetBasePmWarning()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString v_sSql,name,stemp;
	int alarm_type,alarm_flag;                   //���ݿ����
	double alarm_value;
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map<CString,CString> map_temporary;
	base_performance_warning_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);

	v_sSql="SELECT zPm,zAlarm_Flag,zAlarm_Value,zAlarm_Type FROM CON_Pm_LTE";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
	while (!v_pRecordset->adoEOF)
	{
		name = v_cAdoConn.GetField_String("zPm");			         //��ʽ����
		alarm_flag = v_cAdoConn.GetField_Int("zAlarm_Flag");
		if (alarm_flag == 1)
		{			
			map_temporary.clear();
			alarm_value = v_cAdoConn.GetField_Double("zAlarm_Value");
			stemp.Format("%f",alarm_value);
			map_temporary.insert(pair<CString,CString>("zAlarm_Value",stemp));
			alarm_type =  v_cAdoConn.GetField_Int("zAlarm_Type");
			stemp.Format("%d",alarm_type);
			map_temporary.insert(pair<CString,CString>("zAlarm_Type",stemp));
			base_performance_warning_.insert(pair<CString,map<CString,CString>>(name,map_temporary));
			v_pRecordset->MoveNext();
		}
		else
		{
			v_pRecordset->MoveNext();
		}

	}
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:��������Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToOtherPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	float threshold_value = 0,value;
	int flag;
	iterator_warning = region_performance_warning_.begin();
	CString name = "";
	while (iterator_warning != region_performance_warning_.end())
	{
		name = iterator_warning->first;
		iterator_whole_network_performance = whole_network_performance.begin();
		while(iterator_whole_network_performance != whole_network_performance.end())
		{
			if (name == iterator_whole_network_performance->first)
			{
				iterator_temp = iterator_warning->second.find("zAlarm_Value");
				if (iterator_temp != iterator_warning->second.end())
				{
					threshold_value = atof(iterator_temp->second);
					value = atof(iterator_whole_network_performance->second);
					iterator_temp = iterator_warning->second.find("zAlarm_Type");
					if (iterator_temp != iterator_warning->second.end())
					{
						flag = atoi(iterator_temp->second);
						if (flag == 1)
						{
							if (value < threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','����',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','����',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}

							}

						}
					}			
				}

			}
			iterator_whole_network_performance ++;
		}
		iterator_warning ++;
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:ȫ������Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToWholeNetworkPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	float threshold_value = 0,value;
	int flag;
	iterator_warning = region_performance_warning_.begin();
	CString name = "";
	while (iterator_warning != region_performance_warning_.end())
	{
		name = iterator_warning->first;
		iterator_whole_network_performance = whole_network_performance.begin();
		while(iterator_whole_network_performance != whole_network_performance.end())
		{
			if (name == iterator_whole_network_performance->first)
			{
				iterator_temp = iterator_warning->second.find("zAlarm_Value");
				if (iterator_temp != iterator_warning->second.end())
				{
					threshold_value = atof(iterator_temp->second);
					value = atof(iterator_whole_network_performance->second);
					iterator_temp = iterator_warning->second.find("zAlarm_Type");
					if (iterator_temp != iterator_warning->second.end())
					{
						flag = atoi(iterator_temp->second);
						if (flag == 1)
						{
							if (value < threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','ȫ��',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','ȫ��',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}

							}
							
						}
					}			
				}

			}
			iterator_whole_network_performance ++;
		}
		iterator_warning ++;
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:HW����Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToHWPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	float threshold_value = 0,value;
	int flag;
	iterator_warning = region_performance_warning_.begin();
	CString name = "";
	while (iterator_warning != region_performance_warning_.end())
	{
		name = iterator_warning->first;
		iterator_whole_network_performance = whole_network_performance.begin();
		while(iterator_whole_network_performance != whole_network_performance.end())
		{
			if (name == iterator_whole_network_performance->first)
			{
				iterator_temp = iterator_warning->second.find("zAlarm_Value");
				if (iterator_temp != iterator_warning->second.end())
				{
					threshold_value = atof(iterator_temp->second);
					value = atof(iterator_whole_network_performance->second);
					iterator_temp = iterator_warning->second.find("zAlarm_Type");
					if (iterator_temp != iterator_warning->second.end())
					{
						flag = atoi(iterator_temp->second);
						if (flag == 1)
						{
							if (value < threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','��Ϊ',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','��Ϊ',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}

							}

						}
					}			
				}

			}
			iterator_whole_network_performance ++;
		}
		iterator_warning ++;
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:ZTE����Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToZTEPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	float threshold_value = 0,value;
	int flag;
	iterator_warning = region_performance_warning_.begin();
	CString name = "";
	while (iterator_warning != region_performance_warning_.end())
	{
		name = iterator_warning->first;
		iterator_whole_network_performance = whole_network_performance.begin();
		while(iterator_whole_network_performance != whole_network_performance.end())
		{
			if (name == iterator_whole_network_performance->first)
			{
				iterator_temp = iterator_warning->second.find("zAlarm_Value");
				if (iterator_temp != iterator_warning->second.end())
				{
					threshold_value = atof(iterator_temp->second);
					value = atof(iterator_whole_network_performance->second);
					iterator_temp = iterator_warning->second.find("zAlarm_Type");
					if (iterator_temp != iterator_warning->second.end())
					{
						flag = atoi(iterator_temp->second);
						if (flag == 1)
						{
							if (value < threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','����',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','����',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}

							}

						}
					}			
				}

			}
			iterator_whole_network_performance ++;
		}
		iterator_warning ++;
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:��������Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToRegionPerformance(map_Counter  whole_network_performance,CString region_name)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	float threshold_value = 0,value;
	int flag;
	iterator_warning = region_performance_warning_.begin();
	CString name = "";
	while (iterator_warning != region_performance_warning_.end())
	{
		name = iterator_warning->first;
		iterator_whole_network_performance = whole_network_performance.begin();
		while(iterator_whole_network_performance != whole_network_performance.end())
		{
			if (name == iterator_whole_network_performance->first)
			{
				iterator_temp = iterator_warning->second.find("zAlarm_Value");
				if (iterator_temp != iterator_warning->second.end())
				{
					threshold_value = atof(iterator_temp->second);
					value = atof(iterator_whole_network_performance->second);
					iterator_temp = iterator_warning->second.find("zAlarm_Type");
					if (iterator_temp != iterator_warning->second.end())
					{
						flag = atoi(iterator_temp->second);
						if (flag == 1)
						{
							if (value < threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','"+ region_name +"',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//�����ϴ�
								_RecordsetPtr   v_pRecordset;									//��¼��
								CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
								CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','"+ region_name +"',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'����')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
									v_cAdoConn.ExitConnect();										     //�Ͽ�����
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //�ر����ݿ�
									CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
									theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
								}

							}

						}
					}			
				}

			}
			iterator_whole_network_performance ++;
		}
		iterator_warning ++;
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:С����������Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToBasePerformance(map_ObjType &performance_object)
{
	map_Counter::iterator	iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	map<CString,CString>::iterator iterator_cellID;                       //С����ѯ������
	float threshold_value = 0,value;
	int flag;
	CString performance_name = "",cell_name="";
	//�����ļ�ѭ��
	iterator_performance_objtype = performance_object.begin();
	while (iterator_performance_objtype != performance_object.end())
	{
		//С��ѭ��
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_performance_object != iterator_performance_objtype->second.end())
		{
			cell_name = iterator_performance_object->v_sObject_ID;
			//ID�滻ΪС������
			iterator_cellID = cellID_map_name_.find(cell_name);
			if (iterator_cellID == cellID_map_name_.end())
			{
				iterator_performance_object ++;
				continue;
			}
			cell_name = iterator_cellID->second;
			//��ʽѭ��
			iterator_warning = base_performance_warning_.begin();
			while (iterator_warning != base_performance_warning_.end())
			{
				performance_name = iterator_warning->first;
				iterator_map_formula = iterator_performance_object->v_mCounter.begin();
				while (iterator_map_formula != iterator_performance_object->v_mCounter.end() &&
					performance_name != iterator_map_formula->first)
				{
					iterator_map_formula ++;
				}
				if (iterator_map_formula != iterator_performance_object->v_mCounter.end())
				{

					iterator_temp = iterator_warning->second.find("zAlarm_Value");
					if (iterator_temp != iterator_warning->second.end())
					{
						threshold_value = atof(iterator_temp->second);
						value = atof(iterator_map_formula->second);
						iterator_temp = iterator_warning->second.find("zAlarm_Type");
						if (iterator_temp != iterator_warning->second.end())
						{
							flag = atoi(iterator_temp->second);
							if (flag == 1)
							{
								if (value < threshold_value)
								{
									//�����ϴ�
									_RecordsetPtr   v_pRecordset;									//��¼��
									CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
									CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
									map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
									try
									{  

										sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
										sql += file_time_ + "','";
										sql += performance_name +"','"+cell_name+"',";
										stemp.Format("%f",value);
										sql += stemp  + ",";
										stemp.Format("%f",threshold_value);
										sql +=  stemp+",'С��Ԥ��')";
										v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
										v_cAdoConn.SetCommandTimeout(100);
										v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
										v_cAdoConn.ExitConnect();										     //�Ͽ�����
									}
									catch(_com_error e)
									{
										v_cAdoConn.ExitConnect();										  //�ر����ݿ�
										CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
										theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
									}
								}							
							} 
							else
							{
								if (value > threshold_value)
								{
									//�����ϴ�
									_RecordsetPtr   v_pRecordset;									//��¼��
									CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
									CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
									map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
									try
									{  

										sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
										sql += file_time_ + "','";
										sql += performance_name +"','"+cell_name+"',";
										stemp.Format("%f",value);
										sql += stemp  + ",";
										stemp.Format("%f",threshold_value);
										sql +=  stemp+",'С��Ԥ��')";
										v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
										v_cAdoConn.SetCommandTimeout(100);
										v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
										v_cAdoConn.ExitConnect();										     //�Ͽ�����
									}
									catch(_com_error e)
									{
										v_cAdoConn.ExitConnect();										  //�ر����ݿ�
										CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
										theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
									}

								}

							}
						}			
					}
				}
				iterator_warning ++;
			}
			iterator_performance_object++;

		}
		iterator_performance_objtype++;
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:С����ʽ����Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToFormulaPerformance(map_ObjType &performance_object)
{
	map_Counter::iterator	iterator_temp;						    //count���ܵ�����
	map<CString,map<CString,CString>>::iterator iterator_warning;
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	map<CString,CString>::iterator iterator_cellID;                       //С����ѯ������
	float threshold_value = 0,value;
	int flag;
	CString performance_name = "",cell_name="";
		//�����ļ�ѭ��
       iterator_performance_objtype = performance_object.begin();
		while (iterator_performance_objtype != performance_object.end())
		{
			//С��ѭ��
			iterator_performance_object = iterator_performance_objtype->second.begin();
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell_name = iterator_performance_object->v_sObject_ID;
				//ID�滻ΪС������
				iterator_cellID = cellID_map_name_.find(cell_name);
				if (iterator_cellID == cellID_map_name_.end())
				{
					iterator_performance_object ++;
					continue;
				}
				cell_name = iterator_cellID->second;
				//��ʽѭ��
				iterator_warning = formula_performance_warning_.begin();
				while (iterator_warning != formula_performance_warning_.end())
				{
				    performance_name = iterator_warning->first;
					iterator_map_formula = iterator_performance_object->v_mCounter.begin();
					while (iterator_map_formula != iterator_performance_object->v_mCounter.end() &&
						performance_name != iterator_map_formula->first)
					{
						iterator_map_formula ++;
					}
					if (iterator_map_formula != iterator_performance_object->v_mCounter.end())
					{

						iterator_temp = iterator_warning->second.find("zAlarm_Value");
						if (iterator_temp != iterator_warning->second.end())
						{
							threshold_value = atof(iterator_temp->second);
							value = atof(iterator_map_formula->second);
							iterator_temp = iterator_warning->second.find("zAlarm_Type");
							if (iterator_temp != iterator_warning->second.end())
							{
								flag = atoi(iterator_temp->second);
								if (flag == 1)
								{
									if (value < threshold_value)
									{
										//�����ϴ�
										_RecordsetPtr   v_pRecordset;									//��¼��
										CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
										CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
										map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
										try
										{  

											sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
											sql += file_time_ + "','";
											sql += performance_name +"','"+cell_name+"',";
											stemp.Format("%f",value);
											sql += stemp  + ",";
											stemp.Format("%f",threshold_value);
											sql +=  stemp+",'С������')";
											v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
											v_cAdoConn.SetCommandTimeout(100);
											v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
											v_cAdoConn.ExitConnect();										     //�Ͽ�����
										}
										catch(_com_error e)
										{
											v_cAdoConn.ExitConnect();										  //�ر����ݿ�
											CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
											theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
										}
									}							
								} 
								else
								{
									if (value > threshold_value)
									{
										//�����ϴ�
										_RecordsetPtr   v_pRecordset;									//��¼��
										CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
										CString			sql,sql_key="",sql_value="",stemp;							            //��ʱ����
										map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
										try
										{  

											sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
											sql += file_time_ + "','";
											sql += performance_name +"','"+cell_name+"',";
											stemp.Format("%f",value);
											sql += stemp  + ",";
											stemp.Format("%f",threshold_value);
											sql +=  stemp+",'С������')";
											v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
											v_cAdoConn.SetCommandTimeout(100);
											v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
											v_cAdoConn.ExitConnect();										     //�Ͽ�����
										}
										catch(_com_error e)
										{
											v_cAdoConn.ExitConnect();										  //�ر����ݿ�
											CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
											theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
										}

									}

								}
							}			
						}
					}
					iterator_warning ++;
				}
				iterator_performance_object++;

			}
			iterator_performance_objtype++;
		}
}                
//-----------------------------------------------------------------------------------------------------------------
//  ��������:���ʼ��
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::Init()
{
	//�����ݿ��ж�ȡ��������Ԥ������
	GetRegionPmWarning();
	//�����ݿ��ж�ȡС����ʽ����Ԥ������
	GetFormulaPmWarning();
	//�����ݿ��ж�ȡС����������Ԥ������
	GetBasePmWarning();
	//���С��ID��С������ӳ��
	GetCellMap();
	
}                   

//-----------------------------------------------------------------------------------------------------------------
//  ��������:���С�����Ӧ��Ψһ����
//  ��    ��:cellname_map_ID : С���������
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning:: GetCellMap()
{
	_RecordsetPtr v_pRecordset;											  //��¼��
	CMyADOConn v_cAdoConn;												  //���ݿ�ʵ��
	CString		   sql,sTemp,Key,value;							      //��ʱ����
	int            iTemp;
	try
	{
		sql="SELECT zCell,zCell_ID FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		          //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			sTemp = v_cAdoConn.GetField_String("zCell");
			value = sTemp;
			iTemp = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",iTemp);
			cellID_map_name_.insert(pair<CString,CString>(Key,value));
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //�ر����ݿ�

		CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"\r\n");	              //log
	}
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 