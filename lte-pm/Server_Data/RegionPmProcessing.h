#pragma once
//XML文件解析
class CMyFile_XML;
//视图类
class CServer_DataDlg;
//小区过滤
class CMyFilter;
//预警类
#include "PerformanceWarning.h"
//class CPerformanceWarning;

class RegionPmProcessing
{
public:
	RegionPmProcessing(void);
	~RegionPmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//变量
	//------------------------------------------------------------------------------------------------------
	//保存区域性能公式
	map_Counter         map_formula_; 
	//基本性能
	map_Counter         basePm_name_;
	//当前粒度时间
	CString             current_granularity_;  
	//上个粒度粒度时间
	CString             before_granularity_;   
	//计算公式结构
	struct PM_Formula_Struct								      
	{
		CString		v_sPM_Formula;								  //运算符/运算值
		bool		v_bFormula;								      //1：运算值；		0：运算符；
	};
	//性能公式队列：mPM_Formula
	typedef list<PM_Formula_Struct> mPM_Formula;	
	//用于性能预警
	CPerformanceWarning		 Pm_warning_;

	//------------------------------------------------------------------------------------------------------
	//全网变量
	//------------------------------------------------------------------------------------------------------
	//保存单文件全网基本性能性能子项
	map_Counter         network_Pm_iterm_;
	//保存粒度全网基本性能性能子项
	map_Counter         network_grantularity_Pm_iterm_;   
	//全网性能指标
	map_Counter         network_Pm_;

	//------------------------------------------------------------------------------------------------------
	//华为变量
	//------------------------------------------------------------------------------------------------------
	//保存单文件全网基本性能性能子项
	map_Counter         hw_Pm_iterm_;
	//保存粒度全网基本性能性能子项
	map_Counter         hw_grantularity_Pm_iterm_;   
	//中兴性能指标
	map_Counter         hw_Pm_;

	//------------------------------------------------------------------------------------------------------
	//中兴变量
	//------------------------------------------------------------------------------------------------------
	//保存单文件全网基本性能性能子项
	map_Counter         zte_Pm_iterm_;
	//保存粒度全网基本性能性能子项
	map_Counter         zte_grantularity_Pm_iterm_;   
	//中兴性能指标
	map_Counter         zte_Pm_;

	//------------------------------------------------------------------------------------------------------
	//区域变量
	//------------------------------------------------------------------------------------------------------
	//单文件区域基本性能性能子项
	map<CString,map_Counter> region_Pm_iterm_;                             
	//保存粒度区域基本性能性能子项
	map<CString,map_Counter> region_grantularity_Pm_iterm_;    
	//小区及其所属区域
	map_Counter              cell_region_;     
	//保存单文件其他基本性能性能子项
	map_Counter				 other_Pm_iterm_;
	//保存粒度其他基本性能性能子项
	map_Counter				 other_grantularity_Pm_iterm_;  
	//其他性能指标
	map_Counter				 other_Pm_;

	//------------------------------------------------------------------------------------------------------
	//	初始化
	//------------------------------------------------------------------------------------------------------
	//类功能初始化
	void Init(); 
	//类功能初始化
	void Updata(); 
	//获得基本性能名称保存
	void GetBasePm();
	//从数据库中读取性能公式
	void GetRegionFomula(); 
	//性能数据上传数据库
	void UploadingPm(CString time,map_Counter &Pm,CString region);
	//分解性能公式
	void DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula); 
	//合成性能公式
	void CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula);  
	//获得小区及其区域
	void GetCellAndRegion();
	//获得区域及其基本性能
	void GetRegionandPm();

	//------------------------------------------------------------------------------------------------------
	//	全网性能计算函数
	//------------------------------------------------------------------------------------------------------
	//全网单文件基本性能子项求和
	void SumBasePmForWholeNetwork(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//全网每个粒度基本性能子项求和
	void SumBasePmInGranularityForWholeNetwork();
	//全网粒度性能计算
	void CalculatePmForWholeNetwork();

	//------------------------------------------------------------------------------------------------------
	//	区域和其他性能计算函数
	//------------------------------------------------------------------------------------------------------
	//区域和其他单文件基本性能子项求和
	void SumBasePmForRegionandOther(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//区域每个粒度基本性能子项求和
	void SumBasePmInGranularityForRegion();
	//区域粒度性能计算
	void CalculateandUploadingPmForRegion(CString time);
	//其他每个粒度基本性能子项求和
	void SumBasePmInGranularityForOther();
	//其他粒度性能计算
	void CalculatePmForOther();

	//------------------------------------------------------------------------------------------------------
	//	华为性能计算函数
	//------------------------------------------------------------------------------------------------------
	//华为单文件基本性能子项求和
	void SumBasePmForHW(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//华为每个粒度基本性能子项求和
	void SumBasePmInGranularityForHW();
	//华为粒度性能计算
	void CalculatePmForHW();

	//------------------------------------------------------------------------------------------------------
	//	中兴性能计算函数
	//------------------------------------------------------------------------------------------------------
	//中兴单文件基本性能子项求和
	void SumBasePmForZTE(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//中兴每个粒度基本性能子项求和
	void SumBasePmInGranularityForZTE();
	//中兴粒度性能计算
	void CalculatePmForZTE();
};

