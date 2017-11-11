#include "stdafx.h"
#include "CalculationPmProcessing.h"
//视图类
#include "Server_DataDlg.h"

CalculationPmProcessing::CalculationPmProcessing(void)
{
}


CalculationPmProcessing::~CalculationPmProcessing(void)
{
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：初始化
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::Init()
{
	//获取小区性能公式
	GetFomula();                             
	Pm_warning_.Init();
	GetBasePmType();
	GetCellID(hourCellID_cnt_);
	hour_Pm_object_.clear();
	list_Object temp;
	hour_Pm_object_.insert(pair<CString,list_Object>("1",temp));
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：初始化
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::Updata()
{
	//获取小区性能公式
	GetFomula();                             
	Pm_warning_.Init();
	GetCellID(hourCellID_cnt_);
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能:获得小区的唯一编码
//  参    数:hourCellID_cnt_ : 小区其编码及一个小时粒度出现次数
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing:: GetCellID(map<CString,int> &hourCellID_cnt_)
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


//-----------------------------------------------------------------------------------------------------------------
//  函数功能：从数据库中读取性能处理公式
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::GetFomula()
{
	// TODO: 在此添加控件通知处理程序代码
	CString v_sSql,formula,formula_name;
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	map_formula_.clear();                                               //清空数据
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);
	v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_This_Month";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
	while (!v_pRecordset->adoEOF)
	{
		formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
		formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
		map_formula_.insert(pair<CString,CString> (formula_name,formula));
		v_pRecordset->MoveNext();
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现性能公式合成
//  参    数:CSstringformula:合成公式 StructFormula：分解公式
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula)
{
	// TODO: 在此添加控件通知处理程序代码
	mPM_Formula::iterator PMiterator;
	CMyFormula calculateformula;
	CString CSstringtempary;
	PMiterator = StructFormula.begin();
	*CSstringformula = "";
	while (PMiterator != StructFormula.end())
	{
		if (PMiterator->v_bFormula == true)
		{
			*CSstringformula += PMiterator->v_sPM_Formula;
		}
		else
		{
			*CSstringformula += PMiterator->v_sPM_Formula;
		}
		PMiterator++;
	}

}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：保存一刻钟公式性能到TXT
//参    数：directory_path:路径；file_name：文件名称；file_time 文件时间
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::SaveFormulaPmToTXT(CString derictory_path,CString file_name,CString file_time)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	CString formula_performace="",cell_name;
	//打开公式性能文件
	formula_Pm_file_.Open(derictory_path + "\\Pm_LTE_Cal_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	iterator_performance_objtype = Pm_object_.begin();
	//将Cell公式性能写入文件
	while(iterator_performance_objtype != Pm_object_.end())
	{
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while(iterator_performance_object != iterator_performance_objtype->second.end())
		{
			cell_name = iterator_performance_object->v_sObject_ID;
			formula_performace = file_time+","+cell_name;
			iterator_map_formula = iterator_performance_object->v_mCounter.begin();
			while(iterator_map_formula != iterator_performance_object->v_mCounter.end())
			{
				formula_performace += "," + iterator_map_formula->second;
				iterator_map_formula++;
			}
			formula_performace += "\r\n";
			formula_Pm_file_.Write(formula_performace,formula_performace.GetLength());
			formula_performace = "";
			iterator_performance_object++;
		}
		iterator_performance_objtype++;
	}
	//关闭公式性能文件
	formula_Pm_file_.Close();
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现性能公式分解
//  参    数:v_sFormula：具体计算公式 v_mFormula：分解公式
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula)
{
	CString		v_sStr = "";												//临时变量
	bool		v_bFlag = false;											//标记：运算符
	char*		v_pFormula;												    //临时变量
	PM_Formula_Struct	v_cPM_Formula;									    //结构
	v_mFormula->clear();
	v_pFormula = v_sFormula.GetBuffer(0);
	for (int vi = 0; vi<v_sFormula.GetLength(); vi++)						//遍历计算公式
	{
		if ((unsigned char)v_pFormula[vi]<0x30)							    //运算符？？？
		{
			if (v_bFlag)
			{
				if (!v_sStr.IsEmpty())
				{
					v_cPM_Formula.v_sPM_Formula = v_sStr;
					v_cPM_Formula.v_bFormula = v_bFlag;
					if (v_sStr.Left(1) <= "9")							    //若预算值的第一位必须为字符，否则为常数，如：100等，按运算符处理【即不用获取Counter值】
						v_cPM_Formula.v_bFormula = false;					//运算符
					v_mFormula->push_back(v_cPM_Formula);				    //增加：运算值
					v_sStr = "";											//复位
				}
				v_bFlag = false;											//标记：运算符
			}
		}
		else
		{
			if (!v_bFlag)
			{
				if (!v_sStr.IsEmpty())
				{
					v_cPM_Formula.v_sPM_Formula = v_sStr;
					v_cPM_Formula.v_bFormula = v_bFlag;
					v_mFormula->push_back(v_cPM_Formula);				    //增加：运算符
					v_sStr = "";											//复位
				}
				v_bFlag = true;											    //标记：运算值
			}
		}
		v_sStr += v_sFormula.Mid(vi, 1);									//获取：字符串
	}
	if (!v_sStr.IsEmpty())
	{
		v_cPM_Formula.v_sPM_Formula = v_sStr;
		v_cPM_Formula.v_bFormula = v_bFlag;
		if (v_cPM_Formula.v_bFormula)									    //运算值？？？
		{
			if (v_sStr.Left(1) <= "9")									    //若预算值的第一位必须为字符，否则为常数，如：100等，按运算符处理【即不用获取Counter值】
				v_cPM_Formula.v_bFormula = false;							//运算符
		}
		v_mFormula->push_back(v_cPM_Formula);							    //增加：运算值/运算符
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现性能计算，计算值保存在全局变量performance_object_中【加小区预警】
//  参   数：ObjType:接口数据；file_time；文件时间
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::CalculateHourPerformance(CString file_time)
{
	//以下为实际过程
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	list_Object::iterator	iterator_Object;							//count小区迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	float number_result = 0;//性能值
	int icount = -1;
	mPM_Formula  decompose_formula;//分解公式
	//循环处理变量替换性能值并计算
	//文件循环
	Pm_object_ = hour_Pm_object_;
	iterator_objtype = hour_Pm_object_.begin();
	iterator_performance_objtype =Pm_object_.begin();
	while (iterator_objtype != hour_Pm_object_.end())
	{
		//小区循环
		iterator_Object = iterator_objtype->second.begin();
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_Object != iterator_objtype->second.end())
		{
			icount ++;
			//清空原map中的count数据
			iterator_performance_object->v_mCounter.clear();
			//性能公式循环
			iterator_map_formula = map_formula_.begin();
			while(iterator_map_formula != map_formula_.end())
			{
				//性能名称
				performace_name = iterator_map_formula->first;
				//保存性能数据结构
				formula = iterator_map_formula->second;
				//替换"."为aaa
				formula.Replace(".", "aaa");
				//将公式分解;成变量和运算符
				DecomposeFormula(formula, &decompose_formula);
				//还原性能名称中的"."
				iterator_decompose_formula = decompose_formula.begin();
				while (iterator_decompose_formula != decompose_formula.end())
				{
					iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
					iterator_decompose_formula++;
				}
				//公式变量替换为性能值
				iterator_decompose_formula = decompose_formula.begin();
				while (iterator_decompose_formula != decompose_formula.end())
				{
					//替代公式中的变量
					if (iterator_decompose_formula->v_bFormula == true)
					{
						iterator_counter = iterator_Object->v_mCounter.find(iterator_decompose_formula->v_sPM_Formula);
						if (iterator_counter != iterator_Object->v_mCounter.end())
						{
							iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						} 
						else
						{
							//异常填0
							iterator_decompose_formula->v_sPM_Formula = "0";
						}
					}
					iterator_decompose_formula++;
				}
				//还原计算公式
				CompositionFormula(&formula_result, decompose_formula);
				//性能计算
				Pcharformula = formula_result.GetBuffer(0);
				number_result = calculate_formula.simcalc(Pcharformula);
				performace_cstring_value.Format("%f", number_result);
				//插入性能
				iterator_performance_object->v_mCounter.insert(pair<CString, CString>(performace_name, performace_cstring_value));
				//Reset变量
				formula_result = "";
				decompose_formula.clear();
				iterator_map_formula++;
			}
			iterator_Object++;
   			iterator_performance_object++;
		}
		iterator_objtype++;
		iterator_performance_objtype++;
	}
	//小区预警
	//Pm_warning_.file_time_ = file_time;
	//Pm_warning_.CompareToFormulaPerformance(Pm_object_);
	//Reset变量
	decompose_formula.clear();
	hour_Pm_object_.clear();
	list_Object temp;
	hour_Pm_object_.insert(pair<CString,list_Object>("1",temp));
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现15分钟粒度性能计算
//  参   数：ObjType:接口数据；file_time；文件时间
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::CalculatePerformance(map_ObjType &ObjType,CString file_time)
{
	//以下为实际过程
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	list_Object::iterator	iterator_Object;							//count小区迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	float number_result = 0;//性能值
	int icount = -1;
	mPM_Formula  decompose_formula;//分解公式
	//循环处理变量替换性能值并计算
	//文件循环
	Pm_object_ = ObjType;
	iterator_objtype = ObjType.begin();
	iterator_performance_objtype =Pm_object_.begin();
	while (iterator_objtype != ObjType.end())
	{
		//小区循环
		iterator_Object = iterator_objtype->second.begin();
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_Object != iterator_objtype->second.end())
		{
			icount ++;
			//清空原map中的count数据
			iterator_performance_object->v_mCounter.clear();
			//性能公式循环
			iterator_map_formula = map_formula_.begin();
			while(iterator_map_formula != map_formula_.end())
			{
				//性能名称
				performace_name = iterator_map_formula->first;
				//保存性能数据结构
				formula = iterator_map_formula->second;
				//替换"."为aaa
				formula.Replace(".", "aaa");
				//将公式分解;成变量和运算符
				DecomposeFormula(formula, &decompose_formula);
				//还原性能名称中的"."
				iterator_decompose_formula = decompose_formula.begin();
				while (iterator_decompose_formula != decompose_formula.end())
				{
					iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
					iterator_decompose_formula++;
				}
				//公式变量替换为性能值
				iterator_decompose_formula = decompose_formula.begin();
				while (iterator_decompose_formula != decompose_formula.end())
				{
					//替代公式中的变量
					if (iterator_decompose_formula->v_bFormula == true)
					{
						iterator_counter = iterator_Object->v_mCounter.find(iterator_decompose_formula->v_sPM_Formula);
						if (iterator_counter != iterator_Object->v_mCounter.end())
						{
							iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						} 
						else
						{
							//异常填0
							iterator_decompose_formula->v_sPM_Formula = "0";
						}
					}
					iterator_decompose_formula++;
				}
				//还原计算公式
				CompositionFormula(&formula_result, decompose_formula);
				//性能计算
				Pcharformula = formula_result.GetBuffer(0);
				number_result = calculate_formula.simcalc(Pcharformula);
				performace_cstring_value.Format("%f", number_result);
				//插入性能
				iterator_performance_object->v_mCounter.insert(pair<CString, CString>(performace_name, performace_cstring_value));
				//Reset变量
				formula_result = "";
				decompose_formula.clear();
				iterator_map_formula++;
			}
			iterator_Object++;
			iterator_performance_object++;
		}
		iterator_objtype++;
		iterator_performance_objtype++;
	}
	//小区预警
	Pm_warning_.file_time_ = file_time;
	Pm_warning_.CompareToFormulaPerformance(Pm_object_);
	//Reset变量
	decompose_formula.clear();
	ObjType.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------		
//函数功能：bulk insert语句入栈
//参    数：save_directory_path：bulk insert文件路径,table_suffix：表后缀,control_count :控制批处理数量;file_name：文件名称;sql_stack:保存bulkinsert语句栈
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack)
{
	CString sTemp,sMsg,count;
	COleDateTime monthTime;
	count.Format("%d",control_count);
	sTemp="bulk insert Pm_LTE_Cal" + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_Cal_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
}

//------------------------------------------------------------------------------------------------------			
//函数功能：获得基本性能类型
//------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::GetBasePmType()
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
void CalculationPmProcessing::SaveHourPmData(map_ObjType &ObjType)
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
								//v_pIterator_Counter ++;
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
void CalculationPmProcessing::meanHourPmCount(map_ObjType &ObjType)
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
