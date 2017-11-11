#pragma once
class CPerformanceWarning
{
public:
	CPerformanceWarning(void);
	~CPerformanceWarning(void);
//------------------------------------------------------------------------------------------------------
//	性能预警：变量
//------------------------------------------------------------------------------------------------------
map<CString,map<CString,CString>>         formula_performance_warning_;                      //公式性能预警
map<CString,map<CString,CString>>         region_performance_warning_;                       //区域性能预警
map<CString,map<CString,CString>>         base_performance_warning_;                         //基本性能预警
CString     file_time_;                                                                      //提取的时间
map<CString,CString>                     cellID_map_name_;                                   //key:小区ID,value:小区名称

//------------------------------------------------------------------------------------------------------
//	性能预警：函数
//------------------------------------------------------------------------------------------------------

//初始化
void Init();
//获得小区ID到小区名称映射
void GetCellMap();                                                                 
//从数据库中读取区域性能预警数据
void GetRegionPmWarning();	
//从数据库中读取公式性能预警数据
void GetFormulaPmWarning();
//从数据库中读取基本性能预警数据
void GetBasePmWarning();
//小区计算性能预警
void CompareToFormulaPerformance(map_ObjType &performance_object); 
//小区基本性能预警
void CompareToBasePerformance(map_ObjType &performance_object); 
//全网性能预警
void CompareToWholeNetworkPerformance(map_Counter  whole_network_performance);
//ZTE性能预警
void CompareToZTEPerformance(map_Counter  whole_network_performance); 
//HW性能预警
void CompareToHWPerformance(map_Counter  whole_network_performance);        
//其他性能预警
void CompareToOtherPerformance(map_Counter  whole_network_performance);           
//区域性能预警
void CompareToRegionPerformance(map_Counter  whole_network_performance,CString region_name);    
};

