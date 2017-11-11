#pragma once
//XML文件解析
class CMyFile_XML;
//#include "MyFile_XML.h"
//视图类
class CServer_DataDlg;
//预警类
#include "PerformanceWarning.h"
class HourBasePmProcessing
{
public:
	HourBasePmProcessing(void);
	~HourBasePmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//变量
	//------------------------------------------------------------------------------------------------------
	//6个文件 
	CFile       write_file_[6];                                   
	//6个文件写入变量
	CString		write_context_[6];                                
	//保存头两列数据
	CString     cell_key_;                                        
	//判断254还是227个性能数据
	bool        object_type_;                                     
	//计算公式结构
	struct PM_Formula_Struct								      
	{
		CString		v_sPM_Formula;								  //运算符/运算值
		bool		v_bFormula;								      //1：运算值；		0：运算符；
	};
	//性能公式队列：mPM_Formula
	typedef list<PM_Formula_Struct> mPM_Formula;				  
	//一刻钟公式性能保存文本文件	
	CFile        formula_Pm_file_;                                
	//保存性能对象;公式计算使用
	map_ObjType	 Pm_object_;							          
	//保存本月小区性能公式
	map_Counter  map_formula_; 
	//保存小时数据 
	map_ObjType	 Pm_hour_object_;
	//254个性能类型
	map<CString,int>  base254Pm_type_;
	//用于小区预警
	CPerformanceWarning  Pm_warning_;
	//------------------------------------------------------------------------------------------------------
	//	初始化
	//------------------------------------------------------------------------------------------------------
	//类功能初始化
	void Init();   
	//保存4个粒度基本数据
	void SaveHourPmData(CMyFile_XML &v_cFile_XML);  
	//------------------------------------------------------------------------------------------------------
	//	基础254性能处理函数
	//------------------------------------------------------------------------------------------------------
	//打开待保存的6个TXT文件
	void OpenPmFile(CString directory_path,CString XML_file_name);                                            
	//关闭6个TXT文件
	void ClosePmFile();                                                                 
	//处理227个性能数据
	void WritePm227(int iCount,CString sPM);                                            
	//处理254个性能数据
	void WritePm254(int iCount,CString sPM);  
	//处理基本性能数据
	void WriteBasePm(list<CString> *Pm_classification,File_Object &file_object);
	//将cell性能数据分解为六个文件
	void ProcessData(CString file_time,list<CString> *Pm_classification);                       
	//6个TXT文件数据上传小时数据表
	void UploadingHourData(CString sFileTime,CString save_directory_path,CServer_DataDlg* pDlg,CString table_suffix,int control_count,CString XML_file_name,CString file_time);                                 
	//------------------------------------------------------------------------------------------------------
	//	公式性能计算
	//------------------------------------------------------------------------------------------------------
	//从数据库中读取性能公式
	void GetFomula();                                         
	//合成性能公式
	void CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula);      
	//保存l小时粒度公式性能到TXT
	void SaveFormulaPmToTXT(CString derictory_path,CString file_name,CString file_time);                     
	//分解性能公式
	void DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula);               
	//计算小区公式性能
	void CalculatePerformance(CString time);                              
	//性能公式文件数据上传小时数据库
	void UploadingHourFormulaPm(CString sFileTime,CString save_directory_path,CServer_DataDlg* pDlg,CString table_suffix,int control_count,CString XML_file_name,CString file_time);
	//获得254个基本性能类型
	void GetBase254PmType();                                          
};

