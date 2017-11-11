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
//  功能描述:从数据库中读取区域性能预警数据
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::GetRegionPmWarning()
{
	// TODO: 在此添加控件通知处理程序代码
	CString v_sSql,name,stemp;
	int save_flage,alarm_type,alarm_flag;                   //数据库变量
	double alarm_value;
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	map<CString,CString> map_temporary;
	region_performance_warning_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);

	v_sSql="SELECT zPm,zSave_Flag,zAlarm_Flag,zAlarm_Value,zAlarm_Type FROM CON_Pm_LTE_Region";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
	while (!v_pRecordset->adoEOF)
	{
		name = v_cAdoConn.GetField_String("zPm");			         //公式名称
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
//  功能描述:从数据库中读取公式性能预警数据
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::GetFormulaPmWarning()
{
	// TODO: 在此添加控件通知处理程序代码
	CString v_sSql,name,stemp;
	int save_flage,alarm_type,alarm_flag;                   //数据库变量
	double alarm_value;
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	map<CString,CString> map_temporary;
	formula_performance_warning_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);

	v_sSql="SELECT zPm,zSave_Flag,zAlarm_Flag,zAlarm_Value,zAlarm_Type FROM CON_Pm_LTE_Cal";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
	while (!v_pRecordset->adoEOF)
	{
		name = v_cAdoConn.GetField_String("zPm");			         //公式名称
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
//  功能描述:从数据库中读取基本性能预警数据
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::GetBasePmWarning()
{
	// TODO: 在此添加控件通知处理程序代码
	CString v_sSql,name,stemp;
	int alarm_type,alarm_flag;                   //数据库变量
	double alarm_value;
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	map<CString,CString> map_temporary;
	base_performance_warning_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);

	v_sSql="SELECT zPm,zAlarm_Flag,zAlarm_Value,zAlarm_Type FROM CON_Pm_LTE";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
	while (!v_pRecordset->adoEOF)
	{
		name = v_cAdoConn.GetField_String("zPm");			         //公式名称
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
//  功能描述:其他性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToOtherPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count性能迭代器
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
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','其他',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','其他',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:全网性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToWholeNetworkPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count性能迭代器
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
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','全网',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','全网',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:HW性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToHWPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count性能迭代器
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
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','华为',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','华为',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:ZTE性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToZTEPerformance(map_Counter  whole_network_performance)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count性能迭代器
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
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','中兴',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','中兴',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:区域性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToRegionPerformance(map_Counter  whole_network_performance,CString region_name)
{
	map_Counter::iterator	iterator_whole_network_performance,iterator_temp;						    //count性能迭代器
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
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','"+ region_name +"',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
								}
							}							
						} 
						else
						{
							if (value > threshold_value)
							{
								//数据上传
								_RecordsetPtr   v_pRecordset;									//记录集
								CMyADOConn      v_cAdoConn;												//数据库实例
								CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
								map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
								try
								{  

									sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
									sql += file_time_ + "','";
									sql += name +"','"+ region_name +"',";
									stemp.Format("%f",value);
									sql += stemp  + ",";
									stemp.Format("%f",threshold_value);
									sql +=  stemp+",'区域')";
									v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
									v_cAdoConn.SetCommandTimeout(100);
									v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
									v_cAdoConn.ExitConnect();										     //断开连接
								}
								catch(_com_error e)
								{
									v_cAdoConn.ExitConnect();										  //关闭数据库
									CString v_sError = (char *)e.Description();						  //获得：出错信息
									theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:小区基本性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToBasePerformance(map_ObjType &performance_object)
{
	map_Counter::iterator	iterator_temp;						    //count性能迭代器
	map<CString,map<CString,CString>>::iterator iterator_warning;
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	map<CString,CString>::iterator iterator_cellID;                       //小区查询迭代器
	float threshold_value = 0,value;
	int flag;
	CString performance_name = "",cell_name="";
	//性能文件循环
	iterator_performance_objtype = performance_object.begin();
	while (iterator_performance_objtype != performance_object.end())
	{
		//小区循环
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_performance_object != iterator_performance_objtype->second.end())
		{
			cell_name = iterator_performance_object->v_sObject_ID;
			//ID替换为小区名称
			iterator_cellID = cellID_map_name_.find(cell_name);
			if (iterator_cellID == cellID_map_name_.end())
			{
				iterator_performance_object ++;
				continue;
			}
			cell_name = iterator_cellID->second;
			//公式循环
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
									//数据上传
									_RecordsetPtr   v_pRecordset;									//记录集
									CMyADOConn      v_cAdoConn;												//数据库实例
									CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
									map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
									try
									{  

										sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
										sql += file_time_ + "','";
										sql += performance_name +"','"+cell_name+"',";
										stemp.Format("%f",value);
										sql += stemp  + ",";
										stemp.Format("%f",threshold_value);
										sql +=  stemp+",'小区预警')";
										v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
										v_cAdoConn.SetCommandTimeout(100);
										v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
										v_cAdoConn.ExitConnect();										     //断开连接
									}
									catch(_com_error e)
									{
										v_cAdoConn.ExitConnect();										  //关闭数据库
										CString v_sError = (char *)e.Description();						  //获得：出错信息
										theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
									}
								}							
							} 
							else
							{
								if (value > threshold_value)
								{
									//数据上传
									_RecordsetPtr   v_pRecordset;									//记录集
									CMyADOConn      v_cAdoConn;												//数据库实例
									CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
									map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
									try
									{  

										sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
										sql += file_time_ + "','";
										sql += performance_name +"','"+cell_name+"',";
										stemp.Format("%f",value);
										sql += stemp  + ",";
										stemp.Format("%f",threshold_value);
										sql +=  stemp+",'小区预警')";
										v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
										v_cAdoConn.SetCommandTimeout(100);
										v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
										v_cAdoConn.ExitConnect();										     //断开连接
									}
									catch(_com_error e)
									{
										v_cAdoConn.ExitConnect();										  //关闭数据库
										CString v_sError = (char *)e.Description();						  //获得：出错信息
										theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:小区公式性能预警
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::CompareToFormulaPerformance(map_ObjType &performance_object)
{
	map_Counter::iterator	iterator_temp;						    //count性能迭代器
	map<CString,map<CString,CString>>::iterator iterator_warning;
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	map<CString,CString>::iterator iterator_cellID;                       //小区查询迭代器
	float threshold_value = 0,value;
	int flag;
	CString performance_name = "",cell_name="";
		//性能文件循环
       iterator_performance_objtype = performance_object.begin();
		while (iterator_performance_objtype != performance_object.end())
		{
			//小区循环
			iterator_performance_object = iterator_performance_objtype->second.begin();
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell_name = iterator_performance_object->v_sObject_ID;
				//ID替换为小区名称
				iterator_cellID = cellID_map_name_.find(cell_name);
				if (iterator_cellID == cellID_map_name_.end())
				{
					iterator_performance_object ++;
					continue;
				}
				cell_name = iterator_cellID->second;
				//公式循环
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
										//数据上传
										_RecordsetPtr   v_pRecordset;									//记录集
										CMyADOConn      v_cAdoConn;												//数据库实例
										CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
										map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
										try
										{  

											sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
											sql += file_time_ + "','";
											sql += performance_name +"','"+cell_name+"',";
											stemp.Format("%f",value);
											sql += stemp  + ",";
											stemp.Format("%f",threshold_value);
											sql +=  stemp+",'小区计算')";
											v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
											v_cAdoConn.SetCommandTimeout(100);
											v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
											v_cAdoConn.ExitConnect();										     //断开连接
										}
										catch(_com_error e)
										{
											v_cAdoConn.ExitConnect();										  //关闭数据库
											CString v_sError = (char *)e.Description();						  //获得：出错信息
											theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
										}
									}							
								} 
								else
								{
									if (value > threshold_value)
									{
										//数据上传
										_RecordsetPtr   v_pRecordset;									//记录集
										CMyADOConn      v_cAdoConn;												//数据库实例
										CString			sql,sql_key="",sql_value="",stemp;							            //临时变量
										map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
										try
										{  

											sql="INSERT INTO PM_Alarm ( zTime,zAlarm_PM,zAlarm_Object,zAlarm_Value,zAlarm_Level,zAlarm_Type )VALUES( '";
											sql += file_time_ + "','";
											sql += performance_name +"','"+cell_name+"',";
											stemp.Format("%f",value);
											sql += stemp  + ",";
											stemp.Format("%f",threshold_value);
											sql +=  stemp+",'小区计算')";
											v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
											v_cAdoConn.SetCommandTimeout(100);
											v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
											v_cAdoConn.ExitConnect();										     //断开连接
										}
										catch(_com_error e)
										{
											v_cAdoConn.ExitConnect();										  //关闭数据库
											CString v_sError = (char *)e.Description();						  //获得：出错信息
											theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	  //log
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
//  功能描述:类初始化
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning::Init()
{
	//从数据库中读取区域性能预警数据
	GetRegionPmWarning();
	//从数据库中读取小区公式性能预警数据
	GetFormulaPmWarning();
	//从数据库中读取小区基本性能预警数据
	GetBasePmWarning();
	//获得小区ID到小区名称映射
	GetCellMap();
	
}                   

//-----------------------------------------------------------------------------------------------------------------
//  函数功能:获得小区其对应的唯一编码
//  参    数:cellname_map_ID : 小区及其编码
//-----------------------------------------------------------------------------------------------------------------
void CPerformanceWarning:: GetCellMap()
{
	_RecordsetPtr v_pRecordset;											  //记录集
	CMyADOConn v_cAdoConn;												  //数据库实例
	CString		   sql,sTemp,Key,value;							      //临时变量
	int            iTemp;
	try
	{
		sql="SELECT zCell,zCell_ID FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		          //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp = v_cAdoConn.GetField_String("zCell");
			value = sTemp;
			iTemp = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",iTemp);
			cellID_map_name_.insert(pair<CString,CString>(Key,value));
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //关闭数据库

		CString v_sError = (char *)e.Description();						  //获得：出错信息
		theApp.m_MyLog.WriteText("异常："+v_sError+"\r\n");	              //log
	}
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 