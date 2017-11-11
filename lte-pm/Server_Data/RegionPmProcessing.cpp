#include "stdafx.h"
#include "RegionPmProcessing.h"
//视图类
#include "Server_DataDlg.h"
//XML文件解析
//#include "MyFile_XML.h"
RegionPmProcessing::RegionPmProcessing(void)
{
}

RegionPmProcessing::~RegionPmProcessing(void)
{
}
//-----------------------------------------------------------------------------------------------------------------
//  函数功能：类功能初始化
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::Init()
{
	//从数据库中读取性能公式
	GetRegionFomula(); 
	//获得个基本性能名称保存
	GetBasePm();
	//获得小区及其区域
	GetCellAndRegion();
	//获得区域及其基本性能
	GetRegionandPm();
	//单文件全网基本性能性能子项初始化
	network_Pm_iterm_ = basePm_name_;
	//粒度全网基本性能性能子项
	network_grantularity_Pm_iterm_ = basePm_name_;
	//单文件区域基本性能性能子项初始化
	other_Pm_iterm_ = basePm_name_;
	//粒度区域基本性能性能子项
	other_grantularity_Pm_iterm_ = basePm_name_;
	//单文件华为基本性能性能子项初始化
	hw_Pm_iterm_ = basePm_name_;
	//粒度华为基本性能性能子项
	hw_grantularity_Pm_iterm_ = basePm_name_;
	//单文件中兴基本性能性能子项初始化
	zte_Pm_iterm_ = basePm_name_;
	//粒度中兴基本性能性能子项
	zte_grantularity_Pm_iterm_ = basePm_name_;
	//区域预警初始化；
	Pm_warning_.Init();

}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：类功能初始化
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::Updata()
{
	//获得小区及其区域
	GetCellAndRegion();
	//区域预警初始化；
	Pm_warning_.Init();

}

//-----------------------------------------------------------------------------------------------------------------
//  功能描述:获得区域及其基本性能
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetRegionandPm()
{
	region_Pm_iterm_.clear();
	_RecordsetPtr v_pRecordset;											  //记录集
	CMyADOConn v_cAdoConn;												  //数据库实例
	CString			sql,sTemp,key,value;							      //临时变量
	try
	{
		sql="SELECT zRegion FROM CON_Region";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String("zRegion");
			key = sTemp;
			region_Pm_iterm_.insert(pair<CString,map_Counter>(key,basePm_name_));
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
	region_grantularity_Pm_iterm_.clear();
	region_grantularity_Pm_iterm_ = region_Pm_iterm_;
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：从数据库中读取区域性能处理公式
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetRegionFomula()
{
	// TODO: 在此添加控件通知处理程序代码
	CString v_sSql,formula,formula_name;
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	map_formula_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);
	v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Region";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
	while (!v_pRecordset->adoEOF)
	{
		formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
		formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
		map_formula_.insert(pair<CString,CString> (formula_name,formula));
		v_pRecordset->MoveNext();
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：获得基本性能名称
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetBasePm()
{
	int				v_iItem,v_iID=0;									//KPI数量
	CString			v_sTemp,v_sSql,v_sLog_Key;							//临时变量
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//临时变量
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	basePm_name_.clear();
	try
	{											//条目选择无效

		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
		v_sSql += "SELECT zPm FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			basePm_name_.insert(pair<CString,CString>(v_sPm,""));
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
//函数功能：全网单文件基本中求和性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForWholeNetwork(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_performance_iterm;				    //性能公式迭代器
	map_Counter::iterator	iterator_sum_iterm;                         //全网性能公式求和子项迭代器
	map_Counter::iterator   iterator_cell,iterator_performance;         //小区迭代器
	CString cell,region,performance,stemp;                              //小区名称
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number(0);
	bool cell_state;                                                     //小区状态判断
	//子项求和迭代
	iterator_sum_iterm = network_Pm_iterm_.begin();
	//性能循环
	while (iterator_sum_iterm != network_Pm_iterm_.end())
	{
		iterator_performance_objtype = v_mObjType.begin();
		//文件循环
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//小区循环
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//过滤工程小区
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//判断小区状态
				if (cell_state)
				{  
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
					performance = iterator_sum_iterm->first;
					//性能求和
					performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
					} 
				} 
				iterator_performance_object++;
			}
			iterator_performance_objtype++;
		}
		//性能赋值
		iterator_sum_iterm->second.Format("%f",performance_number);
		performance_number = 0;
		iterator_sum_iterm++;
	}	
}

//------------------------------------------------------------------------------------------------------			
//函数功能：华为单文件基本中求和性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForHW(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_performance_iterm;				    //性能公式迭代器
	map_Counter::iterator	iterator_sum_iterm;                         //全网性能公式求和子项迭代器
	map_Counter::iterator   iterator_cell,iterator_performance;         //小区迭代器
	CString cell,region,performance,stemp;                              //小区名称
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number(0);
	bool cell_state;                                                     //小区状态判断
	//子项求和迭代
	iterator_sum_iterm = hw_Pm_iterm_.begin();
	//性能循环
	while (iterator_sum_iterm != hw_Pm_iterm_.end())
	{
		iterator_performance_objtype = v_mObjType.begin();
		//文件循环
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//小区循环
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//过滤工程小区
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//判断小区状态
				if (cell_state)
				{  
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
						performance = iterator_sum_iterm->first;
						//性能求和
						performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
					} 
				} 
				iterator_performance_object++;
			}
			iterator_performance_objtype++;
		}
		//性能赋值
		iterator_sum_iterm->second.Format("%f",performance_number);
		performance_number = 0;
		iterator_sum_iterm++;
	}	
}

//------------------------------------------------------------------------------------------------------			
//函数功能：中兴单文件基本中求和性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForZTE(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_performance_iterm;				    //性能公式迭代器
	map_Counter::iterator	iterator_sum_iterm;                         //全网性能公式求和子项迭代器
	map_Counter::iterator   iterator_cell,iterator_performance;         //小区迭代器
	CString cell,region,performance,stemp;                              //小区名称
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number(0);
	bool cell_state;                                                     //小区状态判断
	//子项求和迭代
	iterator_sum_iterm = zte_Pm_iterm_.begin();
	//性能循环
	while (iterator_sum_iterm != zte_Pm_iterm_.end())
	{
		iterator_performance_objtype = v_mObjType.begin();
		//文件循环
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//小区循环
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//过滤工程小区
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//判断小区状态
				if (cell_state)
				{  
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
						performance = iterator_sum_iterm->first;
						//性能求和
						performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
					} 
				} 
				iterator_performance_object++;
			}
			iterator_performance_objtype++;
		}
		//性能赋值
		iterator_sum_iterm->second.Format("%f",performance_number);
		performance_number = 0;
		iterator_sum_iterm++;
	}	
}

//------------------------------------------------------------------------------------------------------			
//函数功能：全网每个粒度基本性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForWholeNetwork()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//迭代器
	map_Counter::iterator	iterator_network_Pm_iterm;				    //迭代器
	float value;
	iterator_whole_network_grantularity_Pm_iterm = network_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != network_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = network_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != network_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：华为每个粒度基本性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForHW()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//迭代器
	map_Counter::iterator	iterator_network_Pm_iterm;				    //迭代器
	float value;
	iterator_whole_network_grantularity_Pm_iterm = hw_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != hw_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = hw_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != hw_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：中兴每个粒度基本性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForZTE()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//迭代器
	map_Counter::iterator	iterator_network_Pm_iterm;				    //迭代器
	float value;
	iterator_whole_network_grantularity_Pm_iterm = zte_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != zte_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = zte_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != zte_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现性能公式分解
//  参    数:v_sFormula：具体计算公式 v_mFormula：分解公式
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula)
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
//  函数功能：实现性能公式合成
//  参    数:CSstringformula:合成公式 StructFormula：分解公式
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula)
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

//------------------------------------------------------------------------------------------------------			
//函数功能：全网粒度性能计算
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForWholeNetwork()
{
	network_Pm_.clear();                                          //清空数据
	//公式计算
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	bool performance_flag = false;//性能值匹配标签
	float number_result = 0;//性能值
	mPM_Formula  decompose_formula;//分解公式
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
				iterator_counter =  network_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  network_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//找到匹配的性能值
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//应该改为异常显示
					//MessageBoxA("性能未匹配");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//还原计算公式
		CompositionFormula(&formula_result, decompose_formula);
		//性能计算
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		network_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//数据更新
	iterator_performance_iterm = network_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != network_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：华为粒度性能计算
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForHW()
{
	hw_Pm_.clear();                                          //清空数据
	//公式计算
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	bool performance_flag = false;//性能值匹配标签
	float number_result = 0;//性能值
	mPM_Formula  decompose_formula;//分解公式
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
				iterator_counter =  hw_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  hw_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//找到匹配的性能值
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//应该改为异常显示
					//MessageBoxA("性能未匹配");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//还原计算公式
		CompositionFormula(&formula_result, decompose_formula);
		//性能计算
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		hw_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//数据更新
	iterator_performance_iterm = hw_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != hw_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：中兴粒度性能计算
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForZTE()
{
	zte_Pm_.clear();                                          //清空数据
	//公式计算
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	bool performance_flag = false;//性能值匹配标签
	float number_result = 0;//性能值
	mPM_Formula  decompose_formula;//分解公式
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
				iterator_counter =  zte_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  zte_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//找到匹配的性能值
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//应该改为异常显示
					//MessageBoxA("性能未匹配");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//还原计算公式
		CompositionFormula(&formula_result, decompose_formula);
		//性能计算
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		zte_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//数据更新
	iterator_performance_iterm = zte_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != zte_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：性能数据上传数据库
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::UploadingPm(CString time,map_Counter &Pm,CString region)
{
	_RecordsetPtr   v_pRecordset;									//记录集
	CMyADOConn      v_cAdoConn;												//数据库实例
	CString			sql,sql_key="",sql_value="";							            //临时变量
	map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
	try
	{  
		sql="INSERT INTO Pm_LTE_Region ( zTime,zCell";
		iterator_whole_network_performance_updata = Pm.begin();
		while (iterator_whole_network_performance_updata != Pm.end())
		{   sql_key += ",[" + iterator_whole_network_performance_updata->first + "]";
		sql_value += "," + iterator_whole_network_performance_updata->second;
		iterator_whole_network_performance_updata ++;
		}
		sql += sql_key + ")VALUES('" + time + "','" + region + "'" + sql_value + ")";
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
	//文件时间
	Pm_warning_.file_time_ = time;
	//性能预警
	if (region = "其他")
	{
		Pm_warning_.CompareToOtherPerformance(Pm);
	}
	else if (region = "全网")
	{
		Pm_warning_.CompareToWholeNetworkPerformance(Pm);
	}
	else if (region = "中兴")
	{
		Pm_warning_.CompareToZTEPerformance(Pm);
	}
	else if (region = "华为")
	{
		Pm_warning_.CompareToHWPerformance(Pm);
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：其他粒度性能计算
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForOther()
{
	other_Pm_.clear();                                                    //清空数据
	//公式计算
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	bool performance_flag = false;//性能值匹配标签
	float number_result = 0;//性能值
	mPM_Formula  decompose_formula;//分解公式
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
				iterator_counter =  other_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  other_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//找到匹配的性能值
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//应该改为异常显示
					//MessageBoxA("性能未匹配");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//还原计算公式
		CompositionFormula(&formula_result, decompose_formula);
		//性能计算
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		other_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//数据更新
	iterator_performance_iterm = other_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != other_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：区域单文件基本中求和性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForRegionandOther(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_performance_iterm;				    //性能公式迭代器
	map_Counter::iterator	iterator_sum_iterm;                         //全网性能公式求和子项迭代器
	map_Counter::iterator   iterator_cell,iterator_performance;         //小区迭代器
	CString cell,region,performance,stemp;                              //小区名称
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number=0,region_performance_number=0;;
	bool cell_state;                                                     //小区状态判断
	//性能子项求和迭代
	iterator_sum_iterm = basePm_name_.begin();
	//性能循环
	while (iterator_sum_iterm != basePm_name_.end())
	{
		//性能名称
		performance = iterator_sum_iterm->first;
		iterator_performance_objtype = v_mObjType.begin();
		//文件循环
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//小区循环
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//过滤工程小区
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//判断小区状态
				if (cell_state)
				{
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
						//区域求和
						iterator_cell = cell_region_.find(cell);
						if (iterator_cell != cell_region_.end())
						{
							region = iterator_cell->second;
							iterator_region = region_Pm_iterm_.find(region);
							if (iterator_region != region_Pm_iterm_.end())
							{
								iterator_performance = iterator_region->second.find(performance);
								if (iterator_performance != iterator_region->second.end())
								{
									stemp = iterator_performance->second;
									region_performance_number = atof(stemp);
									region_performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
									iterator_performance->second.Format("%f",region_performance_number);
								}
							}
							else
							{
								iterator_performance = other_Pm_iterm_.find(performance);
								if (iterator_performance != other_Pm_iterm_.end())
								{
									stemp = iterator_performance->second;
									region_performance_number = atof(stemp);
									region_performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
									iterator_performance->second.Format("%f",region_performance_number);
								}
							}
						}
						else
						{
							iterator_performance = other_Pm_iterm_.find(performance);
							if (iterator_performance != other_Pm_iterm_.end())
							{
								stemp = iterator_performance->second;
								region_performance_number = atof(stemp);
								region_performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
								iterator_performance->second.Format("%f",region_performance_number);
							}
						}
					} 
				}
				iterator_performance_object++;			
			}
			iterator_performance_objtype++;
		}
		iterator_sum_iterm++;
	}	
}
//-----------------------------------------------------------------------------------------------------------------
//  功能描述:获取小区及其区域
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetCellAndRegion()
{
	_RecordsetPtr v_pRecordset;											  //记录集
	CMyADOConn v_cAdoConn;												  //数据库实例
	CString			sql,sTemp,Key,value;							      //临时变量
	int          cell_ID;
	try
	{
		sql="SELECT zCell_ID,zRegion FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			cell_ID = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",cell_ID);
			value = v_cAdoConn.GetField_String("zRegion");
			cell_region_.insert(pair<CString,CString>(Key,value));
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
//函数功能：区域每个粒度基本性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForRegion()
{
	map_Counter::iterator	iterator_performance_iterm;						    //全网性能公式求和子项迭代器
	map_Counter::iterator	iterator_formula_performance_iterm;		            //全网性能公式求和子项迭代器
	map_Counter::iterator	iterator_base254_performance_iterm;				    //全网性能公式求和子项迭代器
	float performance_whole_network,sub_performance_whole_network;
	map_Counter whole_network_performance_updata;
	CString  region_name;
	map<CString,map_Counter>::iterator iterator_region_iterm, iterator_base254_region_iterm;                   //迭代器
	iterator_region_iterm = region_grantularity_Pm_iterm_.begin();
	//区域循环
	while (iterator_region_iterm != region_grantularity_Pm_iterm_.end())
	{
		//区域名称
		region_name = iterator_region_iterm->first;           
		iterator_performance_iterm = iterator_region_iterm->second.begin();
		//性能循环
		while (iterator_performance_iterm != iterator_region_iterm->second.end())
		{
			iterator_base254_region_iterm = region_Pm_iterm_.find(region_name);
			if (iterator_base254_region_iterm != region_Pm_iterm_.end())
			{
				iterator_formula_performance_iterm = iterator_base254_region_iterm->second.find(iterator_performance_iterm->first);
				if (iterator_formula_performance_iterm != iterator_base254_region_iterm->second.end())
				{
					performance_whole_network = atof(iterator_performance_iterm->second);
					sub_performance_whole_network = atof(iterator_formula_performance_iterm->second);
					performance_whole_network += sub_performance_whole_network;
					iterator_performance_iterm->second.Format("%f",performance_whole_network);
				}

			}
			iterator_performance_iterm++;
		}
		iterator_region_iterm++ ;
	}
	//region_Pm_iterm_数据归零
	iterator_region_iterm = region_Pm_iterm_.begin();
	//区域循环
	while (iterator_region_iterm != region_Pm_iterm_.end())
	{
		iterator_performance_iterm = iterator_region_iterm->second.begin();
		//性能循环
		while (iterator_performance_iterm != iterator_region_iterm->second.end())
		{
			iterator_performance_iterm->second ="";
			iterator_performance_iterm++;
		}
		iterator_region_iterm++ ;
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：区域粒度性能计算
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculateandUploadingPmForRegion(CString time)
{
	map_Counter::iterator	iterator_performance_iterm;						    //全网性能公式求和子项迭代器
	map_Counter::iterator	iterator_formula_performance_iterm;		            //全网性能公式求和子项迭代器
	map_Counter::iterator	iterator_base254_performance_iterm;				    //全网性能公式求和子项迭代器
	float performance_whole_network,sub_performance_whole_network;
	map_Counter whole_network_performance_updata;
	CString  region_name;
	map<CString,map_Counter>::iterator iterator_region_iterm, iterator_base254_region_iterm;                   //迭代器
	//公式计算
	iterator_region_iterm = region_grantularity_Pm_iterm_.begin();
	//区域循环
	while (iterator_region_iterm != region_grantularity_Pm_iterm_.end())
	{
		region_name = iterator_region_iterm->first;           //区域名称
		map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
		mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
		map_Counter::iterator	iterator_counter;						    //count性能迭代器
		map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
		CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
		CMyFormula calculate_formula;//计算性能指标类
		char * Pcharformula = NULL;//用于性能指标输入
		bool performance_flag = false;//性能值匹配标签
		float number_result = 0;//性能值
		mPM_Formula  decompose_formula;//分解公式
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
					iterator_counter =  iterator_region_iterm->second.begin();
					performance_flag = false;
					while (iterator_counter !=  iterator_region_iterm->second.end() && (!performance_flag))
					{
						//找到匹配的性能值
						if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
						{
							iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
							performance_flag = true;
						}
						iterator_counter++;
					}
					if (!performance_flag)
					{
						//应该改为异常显示
						//MessageBoxA("性能未匹配");
					}
					iterator_decompose_formula++;
				}
				else
				{
					iterator_decompose_formula++;
					continue;
				}

			}
			//还原计算公式
			CompositionFormula(&formula_result, decompose_formula);
			//性能计算
			Pcharformula = formula_result.GetBuffer(0);
			number_result = calculate_formula.simcalc(Pcharformula);
			performace_cstring_value.Format("%f", number_result);
			whole_network_performance_updata.insert(pair <CString,CString>(performace_name,performace_cstring_value));
			iterator_map_formula++;
		}
		//数据上传
		_RecordsetPtr   v_pRecordset;									//记录集
		CMyADOConn      v_cAdoConn;												//数据库实例
		CString			sql,sql_key="",sql_value="";							            //临时变量
		map_Counter::iterator	iterator_whole_network_performance_updata;						    //count性能迭代器
		try
		{  
			sql="INSERT INTO Pm_LTE_Region ( zTime,zCell";
			iterator_whole_network_performance_updata = whole_network_performance_updata.begin();
			while (iterator_whole_network_performance_updata != whole_network_performance_updata.end())
			{   sql_key += ",[" + iterator_whole_network_performance_updata->first + "]";
			sql_value += "," + iterator_whole_network_performance_updata->second;
			iterator_whole_network_performance_updata ++;
			}
			sql += sql_key + ")VALUES('" + time + "','"+ region_name +"'" + sql_value + ")";
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
		//区域预警
		Pm_warning_.file_time_ = time;
		Pm_warning_.CompareToRegionPerformance(whole_network_performance_updata,region_name);
		whole_network_performance_updata.clear();
		iterator_region_iterm++ ;	
	}
	//数据更新
	iterator_region_iterm = region_grantularity_Pm_iterm_.begin();
	//区域循环
	while (iterator_region_iterm != region_grantularity_Pm_iterm_.end())
	{
		region_name = iterator_region_iterm->first;           //区域名称
		iterator_performance_iterm = iterator_region_iterm->second.begin();
		//性能循环
		while (iterator_performance_iterm != iterator_region_iterm->second.end())
		{
			iterator_performance_iterm->second ="";
			iterator_performance_iterm++;
		}
		iterator_region_iterm++ ;
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：其他每个粒度基本性能子项求和
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForOther()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//迭代器
	map_Counter::iterator	iterator_network_Pm_iterm;				            //迭代器
	float value;
	iterator_whole_network_grantularity_Pm_iterm = other_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != other_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = other_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != other_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
	//数据归零
	iterator_network_Pm_iterm = other_Pm_iterm_.begin();
	while (iterator_network_Pm_iterm != other_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm->second = "";
		iterator_network_Pm_iterm ++;
	}
}