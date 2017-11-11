#pragma once
//XML文件解析
#include "MyFile_XML.h"
//视图类
class CServer_DataDlg;
//预警类
#include "PerformanceWarning.h"
class BasePmProcessing
{
public:
	BasePmProcessing(void);
	~BasePmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//变量
	//------------------------------------------------------------------------------------------------------
	//文件实例 
	vector<CFile*>       write_file_;                                                                 
	//保存前两列数据【时间和唯一ID】
	CString				 cell_key_;  
	//基本性能分类
	list<CString>        Pm_classification[6];
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
	//获得性能分类
	void GetPmClassification(list<CString> *Pm_classification);
	//获得基本性能类型
	void GetBasePmType();
	//保存4个15分钟粒度数据
	void SaveHourPmData(map_ObjType &ObjType); 
	//打开待保存TXT文件
	void OpenPmFile(CString directory_path,CString file_name);                                            
	//关闭TXT文件
	void ClosePmFile();
	//将cell性能数据分解
	void ProcessData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification); 
	//将cell性能数据分解
	void ProcessHourData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification); 
	//处理基本性能数据
	void WriteBasePm(list<CString> *Pm_classification,File_Object &file_object);                      
	//bulk insert语句入栈
	void PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack);   
	//获得小区映射ID用于统计小区小时粒度出现次数
	void  GetCellID(map<CString,int> &hourCellID_cnt_);
	//小时粒度count求平均值
	void meanHourPmCount(map_ObjType &ObjType);
};

