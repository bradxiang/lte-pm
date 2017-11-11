#pragma once
//视图类
class CServer_DataDlg;
//预警类
#include "PerformanceWarning.h"

class CalculationPmProcessing
{
public:
	CalculationPmProcessing(void);
	~CalculationPmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//变量
	//------------------------------------------------------------------------------------------------------
	//计算公式结构
	struct PM_Formula_Struct								      
	{
		CString		v_sPM_Formula;								  //运算符/运算值
		bool		v_bFormula;								      //1：运算值；		0：运算符；
	};
	//性能公式队列：mPM_Formula
	typedef list<PM_Formula_Struct> mPM_Formula;				  
	//文件实例 
	CFile			formula_Pm_file_;  
	//保存性能对象
	map_ObjType		Pm_object_;							          
	//保存本月小区性能公式
	map_Counter		map_formula_;
	//254个性能类型
	map<CString,int>     basePm_type_;
	//小时粒度小区出现的次数
	map<CString,int>     hourCellID_cnt_;
	//保存小时数据 
	map_ObjType	         hour_Pm_object_;
	//用于小区预警
	CPerformanceWarning  Pm_warning_;
	//------------------------------------------------------------------------------------------------------
	//性能处理函数
	//------------------------------------------------------------------------------------------------------
	//类功能初始化
	void Init(); 
	//类功能更新
	void Updata(); 
	//获得基本性能类型
	void GetBasePmType();
	//保存4个15分钟粒度数据
	void SaveHourPmData(map_ObjType &ObjType); 
	//从数据库中读取性能公式
	void GetFomula();                                         
	//合成性能公式
	void CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula);      
	//保存一刻钟公式性能到TXT
	void SaveFormulaPmToTXT(CString sFilePath,CString XML_file_name,CString file_time);                     
	//分解性能公式
	void DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula);               
	//计算小区15分钟公式性能
	void CalculatePerformance(map_ObjType &ObjType,CString time);     
	//计算小时粒度小区公式性能
	void CalculateHourPerformance(CString time);
	//bulk insert语句入栈
	void PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack);
	//获得小区映射ID用于统计小区小时粒度出现次数
	void  GetCellID(map<CString,int> &hourCellID_cnt_);
	//小时粒度count求平均值
	void meanHourPmCount(map_ObjType &ObjType);

};


