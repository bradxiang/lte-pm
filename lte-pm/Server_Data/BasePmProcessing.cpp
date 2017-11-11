#include "stdafx.h"
#include "BasePmProcessing.h"
//视图类
#include "Server_DataDlg.h"

BasePmProcessing::BasePmProcessing(void)
{
	for (int i = 0 ; i < 6 ; i++)
	{
		CFile *a =new CFile();
		write_file_.push_back(a);
	}
}


BasePmProcessing::~BasePmProcessing(void)
{
}

//------------------------------------------------------------------------------------------------------			
//函数功能：打开待保存TXT文件
//参    数：directory_path:路径；file_name：压缩文件名称
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::OpenPmFile(CString directory_path,CString file_name)
{
	CString temp = directory_path + "\\Pm_LTE_MAC_"+ file_name + ".txt";
	write_file_[0]->Open(directory_path + "\\Pm_LTE_MAC_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[1]->Open(directory_path + "\\Pm_LTE_RRC_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[2]->Open(directory_path + "\\Pm_LTE_PDCP_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[3]->Open(directory_path + "\\Pm_LTE_PHY_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[4]->Open(directory_path + "\\Pm_LTE_THO_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[5]->Open(directory_path + "\\Pm_LTE_TXT_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
}

//------------------------------------------------------------------------------------------------------			
//函数功能：将cell性能数据分解为文件
//参    数：ObjType：文件数据；file_time：文件时间；Pm_classification：cell性能分类标准
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::ProcessData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器,XML总体数据
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器，保存全体性能Map列表
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter迭代器，操作总体每个Map容器迭代器
	map_Counter::iterator	v_pIterator_File;							//map_Counter迭代器，操作总体每个FileNameMap容器迭代器
	v_pIterator_ObjType = ObjType.begin();							    //获取总体数据的迭代器
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType中有Object？[即：Object数量为空？？？]
		{
			v_pIterator_Object=v_pIterator_ObjType->second.begin();		        //获得：Object迭代器起始位置
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//条件：Object迭代器终止位置
			{
				cell_key_ = file_time+","+v_pIterator_Object->v_sObject_ID;     //保存头两列数据,txt六个文件添加每行头信息
				WriteBasePm(Pm_classification,*v_pIterator_Object);
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;	   
	}

	//基本性能预警
	Pm_warning_.file_time_ = file_time;
	Pm_warning_.CompareToBasePerformance(ObjType);
}

//------------------------------------------------------------------------------------------------------			
//函数功能：将cell性能数据分解为文件
//参    数：ObjType：文件数据；file_time：文件时间；Pm_classification：cell性能分类标准
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::ProcessHourData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器,XML总体数据
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器，保存全体性能Map列表
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter迭代器，操作总体每个Map容器迭代器
	map_Counter::iterator	v_pIterator_File;							//map_Counter迭代器，操作总体每个FileNameMap容器迭代器
	v_pIterator_ObjType = ObjType.begin();							    //获取总体数据的迭代器
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType中有Object？[即：Object数量为空？？？]
		{
			v_pIterator_Object=v_pIterator_ObjType->second.begin();		        //获得：Object迭代器起始位置
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//条件：Object迭代器终止位置
			{
				cell_key_ = file_time+","+v_pIterator_Object->v_sObject_ID;     //保存头两列数据,txt六个文件添加每行头信息
				WriteBasePm(Pm_classification,*v_pIterator_Object);
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;	   
	}

	//基本性能预警
	//Pm_warning_.file_time_ = file_time;
	//Pm_warning_.CompareToBasePerformance(ObjType);
}

//------------------------------------------------------------------------------------------------------			
//函数功能：处理基本性能数据
//参    数：Pm_classification：分类标准；file_object：小区性能分类
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::WriteBasePm(list<CString> *Pm_classification,File_Object &file_object)
{
	list<CString>::iterator Pm_classification_iterator;
	map<CString,CString>::iterator    Pm_iterator;
	CString write_context;
	for (int i = 0;i < write_file_.size();i ++)
	{
		write_context = cell_key_;
		Pm_classification_iterator = Pm_classification[i].begin();
		while (Pm_classification_iterator != Pm_classification[i].end())
		{
			Pm_iterator = file_object.v_mCounter.find(*Pm_classification_iterator);
			if (Pm_iterator != file_object.v_mCounter.end())
			{
				write_context += "," + Pm_iterator->second;
			} 
			else
			{
				write_context += ",";
			}
			Pm_classification_iterator ++;
		}
		if (Pm_classification_iterator == Pm_classification[i].end())
		{
			write_context+="\r\n";
			write_file_[i]->Write(write_context,write_context.GetLength());
		}
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：关闭txt文件
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::ClosePmFile()
{
	for (int iNum=0 ; iNum < write_file_.size() ; iNum++)//关闭6个文本文件
	{
		write_file_[iNum]->Close();
	}	
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------		
//函数功能：bulk insert语句入栈
//参    数：save_directory_path：bulk insert文件路径,table_suffix：表后缀,control_count :控制批处理数量;file_name：文件名称;sql_stack:保存bulkinsert语句栈
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BasePmProcessing::PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack)
{
	CString sTemp,sMsg,count;
	COleDateTime monthTime;
	count.Format("%d",control_count);
	sTemp="bulk insert Pm_LTE_PDCP" + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_PDCP_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
	sTemp="bulk insert Pm_LTE_TXT" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_TXT_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";	
	sql_stack.push(sTemp);	
	sTemp="bulk insert Pm_LTE_RRC" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_RRC_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";	
	sql_stack.push(sTemp);	
	sTemp="bulk insert Pm_LTE_THO" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_THO_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
	sTemp="bulk insert Pm_LTE_MAC" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_MAC_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
	sTemp="bulk insert Pm_LTE_PHY" + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_PHY_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：初始化
//-----------------------------------------------------------------------------------------------------------------
void BasePmProcessing::Init()
{
	Pm_warning_.Init();
	GetPmClassification(Pm_classification);
	GetBasePmType();
	GetCellID(hourCellID_cnt_);
	hour_Pm_object_.clear();
	list_Object temp;
	hour_Pm_object_.insert(pair<CString,list_Object>("1",temp));
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：参数更新
//-----------------------------------------------------------------------------------------------------------------
void BasePmProcessing::Updata()
{
	Pm_warning_.Init();
	GetCellID(hourCellID_cnt_);
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能:获得小区的唯一编码
//  参    数:hourCellID_cnt_ : 小区其编码及一个小时粒度出现次数
//-----------------------------------------------------------------------------------------------------------------
void BasePmProcessing:: GetCellID(map<CString,int> &hourCellID_cnt_)
{
	_RecordsetPtr v_pRecordset;											  //记录集
	CMyADOConn v_cAdoConn;												  //数据库实例
	CString		   sql,sTemp,Key;							      //临时变量
	int            iTemp,value(0);
	try
	{
		hourCellID_cnt_.clear();
		sql="SELECT zCell_ID FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		          //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			iTemp = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",iTemp);
			hourCellID_cnt_.insert(pair<CString,int>(Key,value));
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
}

//------------------------------------------------------------------------------------------------------			
//函数功能：获得性能分类
//参    数:Pm_classification:不同类别按顺序保存在不同的list中
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::GetPmClassification(list<CString> *Pm_classification)
{
	CString			     v_sTemp,v_sSql,temp;							    //临时变量；SQL执行；
	CString			     v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//对应于表CON_System中的变量名称
	map<CString,CString> map_temp;
	COleDateTime         tableTime;                                         //表时间
	_RecordsetPtr v_pRecordset;											    //记录集
	CMyADOConn v_cAdoConn;												    //数据库实例
	try
	{   
		for (int i = 0;i<=5;i++)
		{
			Pm_classification[i].clear();
		}
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
				Pm_classification[0].push_back(v_sPm);
			}
			else if(v_sTable_Key=="RRC")
			{
				Pm_classification[1].push_back(v_sPm);
			}
			else if(v_sTable_Key=="PDCP")
			{
				Pm_classification[2].push_back(v_sPm);
			}
			else if(v_sTable_Key=="PHY")
			{
				Pm_classification[3].push_back(v_sPm);
			}
			else if(v_sTable_Key=="THO")
			{
				Pm_classification[4].push_back(v_sPm);
			}
			else if(v_sTable_Key=="TXT")
			{
				Pm_classification[5].push_back(v_sPm);
			}
			else
			{
				v_sTemp= "获取PM名称错误"+v_sPm;
				theApp.m_MyLog.WriteText("异常："+v_sTemp+"  "+v_sSql+"\r\n");	  //log
			}	
			v_pRecordset->MoveNext();									      //
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
//函数功能：获得基本性能类型
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::GetBasePmType()
{
	int				v_iItem,v_iID=0,value;									//KPI数量
	CString			v_sTemp,v_sSql,v_sLog_Key;							//临时变量
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//临时变量
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	basePm_type_.clear();
	try
	{											//条目选择无效

		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
		v_sSql += "SELECT zPm,zHour FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			value = v_cAdoConn.GetField_Int("zHour");                   //zHour
			basePm_type_.insert(pair<CString,int>(v_sPm,value));
			v_pRecordset->MoveNext();									//
		}
		v_cAdoConn.ExitConnect();										//断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：保存4个粒度基本数据
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::SaveHourPmData(map_ObjType &ObjType)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器,XML总体数据
	map_ObjType::iterator	v_pIterator_hour_ObjType;				    //list_ObjType指示器,保存小时数据
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器，保存全体性能Map列表
	list_Object::iterator	v_pIterator_hour_Object;				    //list_Object迭代器，保存小区
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter迭代器，操作总体每个Map容器迭代器
	map_Counter::iterator	v_pIterator_hour_Counter;				    //map_Counter迭代器，操作性能容器迭代器
	map<CString,int>::iterator	v_pIterator_type;						//map_Counter迭代器
	map<CString,int>::iterator	v_pIterator_cellID;						//map_Counter迭代器
	CString cell_name;													//小区名称
	float value,value1;                                                 //数据值
	int   Pm_type;
	File_Object  temp;                                                  //临时变量
	v_pIterator_ObjType = ObjType.begin();		                        //获取总体数据的迭代器
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType中有Object？[即：Object数量为空？？？]
		{
			v_pIterator_Object = v_pIterator_ObjType->second.begin();		    //获得：Object迭代器起始位置
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//条件：Object迭代器终止位置
			{
				cell_name = v_pIterator_Object->v_sObject_ID;                   //小区名称
				v_pIterator_cellID = hourCellID_cnt_.find(cell_name);
				if(v_pIterator_cellID == hourCellID_cnt_.end()){
					v_pIterator_Object++;
					continue;
				}
				v_pIterator_cellID->second++;
				v_pIterator_hour_ObjType = hour_Pm_object_.find("1");
				if (v_pIterator_hour_ObjType != hour_Pm_object_.end())
				{
					v_pIterator_hour_Object = v_pIterator_hour_ObjType->second.begin(); 
					while(v_pIterator_hour_Object != v_pIterator_hour_ObjType->second.end())
					{
						if (v_pIterator_hour_Object->v_sObject_ID == cell_name)
						{
							v_pIterator_hour_Counter = v_pIterator_hour_Object->v_mCounter.begin();
							//v_pIterator_Counter = v_pIterator_Object->v_mCounter.begin();
							while (v_pIterator_hour_Counter != v_pIterator_hour_Object->v_mCounter.end())
							{
								v_pIterator_Counter = v_pIterator_Object->v_mCounter.find(v_pIterator_hour_Counter->first);
								if(v_pIterator_Counter == v_pIterator_Object->v_mCounter.end()){
									v_pIterator_hour_Counter ++;
									continue;
								}
								v_pIterator_type = basePm_type_.find(v_pIterator_hour_Counter->first);
								Pm_type = v_pIterator_type->second;
								switch (Pm_type)
								{
								case 0:
									value = _ttof(v_pIterator_hour_Counter->second);
									value += _ttof(v_pIterator_Counter->second);
									break;
								case 1:
									try{
										value = _ttof(v_pIterator_hour_Counter->second);
										value += _ttof(v_pIterator_Counter->second);
										break;
									}
									catch(...){
										//Log：程序开始
										theApp.m_MyLog.WriteText(cell_name+"********************\r\n");		//Log
									}

								case 2:
									value = _ttof(v_pIterator_hour_Counter->second);
									value1= _ttof(v_pIterator_Counter->second);
									if(value > value1)
									{
										value = value;
									}
									else
									{
										value = value1;
									}
									break;
								case 3:
									value = _ttof(v_pIterator_hour_Counter->second);
									value1= _ttof(v_pIterator_Counter->second);
									if(value < value1)
									{
										value = value;
									}
									else
									{
										value = value1;
									}
									break;
								default:
									value = _ttof(v_pIterator_hour_Counter->second);
									value += _ttof(v_pIterator_Counter->second);
									break;
								}
								v_pIterator_hour_Counter->second.Format("%f",value);
								v_pIterator_hour_Counter ++;
							}
							break;
						}
						v_pIterator_hour_Object ++;
					}
					if (v_pIterator_hour_Object == v_pIterator_hour_ObjType->second.end())
					{
						temp.v_mCounter = v_pIterator_Object->v_mCounter;
						temp.v_sObject_ID = v_pIterator_Object->v_sObject_ID;
						temp.v_sObject_DN = v_pIterator_Object->v_sObject_DN;
						v_pIterator_hour_Counter = temp.v_mCounter.begin();
						while (v_pIterator_hour_Counter != temp.v_mCounter.end())
						{
							v_pIterator_type = basePm_type_.find(v_pIterator_hour_Counter->first);
							Pm_type = v_pIterator_type->second;
							switch (Pm_type)
							{
							case 0:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 1:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 2:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 3:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							default:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							}
							v_pIterator_hour_Counter->second.Format("%f",value);
							v_pIterator_hour_Counter ++;
						}
						v_pIterator_hour_ObjType->second.push_back(temp);
					}
				} 
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：小时粒度count求平均值
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::meanHourPmCount(map_ObjType &ObjType)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器,XML总体数据
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器，保存全体性能Map列表
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter迭代器，操作总体每个Map容器迭代器
	map<CString,int>::iterator	v_pIterator_type;						//map_Counter迭代器
	map<CString,int>::iterator	v_pIterator_cellID;						//map_Counter迭代器
	CString cell_name;													//小区名称
	float value,value1;                                                 //数据值
	int   Pm_type;
	File_Object  temp;                                                  //临时变量
	v_pIterator_ObjType = ObjType.begin();		                        //获取总体数据的迭代器
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType中有Object？[即：Object数量为空？？？]
		{
			v_pIterator_Object = v_pIterator_ObjType->second.begin();		    //获得：Object迭代器起始位置
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//条件：Object迭代器终止位置
			{
				cell_name = v_pIterator_Object->v_sObject_ID;                   //小区名称
				v_pIterator_Counter = v_pIterator_Object->v_mCounter.begin();
				while(v_pIterator_Counter!=v_pIterator_Object->v_mCounter.end()){
					v_pIterator_type = basePm_type_.find(v_pIterator_Counter->first);
					Pm_type = v_pIterator_type->second;
					switch (Pm_type)
					{
					case 1:
						try{
							v_pIterator_cellID = hourCellID_cnt_.find(cell_name);
							if(v_pIterator_cellID == hourCellID_cnt_.end()){
							break;
							}
							if(v_pIterator_cellID->second == 0){
							value = 0;
							v_pIterator_Counter->second.Format("%f",value);
							}
							value = _ttof(v_pIterator_Counter->second)/v_pIterator_cellID->second;
							v_pIterator_Counter->second.Format("%f",value);
							break;
						}
						catch(...){
							//Log：程序开始
							theApp.m_MyLog.WriteText(cell_name+"********************\r\n");		//Log
						}
					default:
						break;
					}
					v_pIterator_Counter++;
				}
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;
	}
}
//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------

