#pragma once
//XML�ļ�����
class CMyFile_XML;
//#include "MyFile_XML.h"
//��ͼ��
class CServer_DataDlg;
//Ԥ����
#include "PerformanceWarning.h"
class HourBasePmProcessing
{
public:
	HourBasePmProcessing(void);
	~HourBasePmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//����
	//------------------------------------------------------------------------------------------------------
	//6���ļ� 
	CFile       write_file_[6];                                   
	//6���ļ�д�����
	CString		write_context_[6];                                
	//����ͷ��������
	CString     cell_key_;                                        
	//�ж�254����227����������
	bool        object_type_;                                     
	//���㹫ʽ�ṹ
	struct PM_Formula_Struct								      
	{
		CString		v_sPM_Formula;								  //�����/����ֵ
		bool		v_bFormula;								      //1������ֵ��		0���������
	};
	//���ܹ�ʽ���У�mPM_Formula
	typedef list<PM_Formula_Struct> mPM_Formula;				  
	//һ���ӹ�ʽ���ܱ����ı��ļ�	
	CFile        formula_Pm_file_;                                
	//�������ܶ���;��ʽ����ʹ��
	map_ObjType	 Pm_object_;							          
	//���汾��С�����ܹ�ʽ
	map_Counter  map_formula_; 
	//����Сʱ���� 
	map_ObjType	 Pm_hour_object_;
	//254����������
	map<CString,int>  base254Pm_type_;
	//����С��Ԥ��
	CPerformanceWarning  Pm_warning_;
	//------------------------------------------------------------------------------------------------------
	//	��ʼ��
	//------------------------------------------------------------------------------------------------------
	//�๦�ܳ�ʼ��
	void Init();   
	//����4�����Ȼ�������
	void SaveHourPmData(CMyFile_XML &v_cFile_XML);  
	//------------------------------------------------------------------------------------------------------
	//	����254���ܴ�����
	//------------------------------------------------------------------------------------------------------
	//�򿪴������6��TXT�ļ�
	void OpenPmFile(CString directory_path,CString XML_file_name);                                            
	//�ر�6��TXT�ļ�
	void ClosePmFile();                                                                 
	//����227����������
	void WritePm227(int iCount,CString sPM);                                            
	//����254����������
	void WritePm254(int iCount,CString sPM);  
	//���������������
	void WriteBasePm(list<CString> *Pm_classification,File_Object &file_object);
	//��cell�������ݷֽ�Ϊ�����ļ�
	void ProcessData(CString file_time,list<CString> *Pm_classification);                       
	//6��TXT�ļ������ϴ�Сʱ���ݱ�
	void UploadingHourData(CString sFileTime,CString save_directory_path,CServer_DataDlg* pDlg,CString table_suffix,int control_count,CString XML_file_name,CString file_time);                                 
	//------------------------------------------------------------------------------------------------------
	//	��ʽ���ܼ���
	//------------------------------------------------------------------------------------------------------
	//�����ݿ��ж�ȡ���ܹ�ʽ
	void GetFomula();                                         
	//�ϳ����ܹ�ʽ
	void CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula);      
	//����lСʱ���ȹ�ʽ���ܵ�TXT
	void SaveFormulaPmToTXT(CString derictory_path,CString file_name,CString file_time);                     
	//�ֽ����ܹ�ʽ
	void DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula);               
	//����С����ʽ����
	void CalculatePerformance(CString time);                              
	//���ܹ�ʽ�ļ������ϴ�Сʱ���ݿ�
	void UploadingHourFormulaPm(CString sFileTime,CString save_directory_path,CServer_DataDlg* pDlg,CString table_suffix,int control_count,CString XML_file_name,CString file_time);
	//���254��������������
	void GetBase254PmType();                                          
};

