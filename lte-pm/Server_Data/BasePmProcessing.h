#pragma once
//XML�ļ�����
#include "MyFile_XML.h"
//��ͼ��
class CServer_DataDlg;
//Ԥ����
#include "PerformanceWarning.h"
class BasePmProcessing
{
public:
	BasePmProcessing(void);
	~BasePmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//����
	//------------------------------------------------------------------------------------------------------
	//�ļ�ʵ�� 
	vector<CFile*>       write_file_;                                                                 
	//����ǰ�������ݡ�ʱ���ΨһID��
	CString				 cell_key_;  
	//�������ܷ���
	list<CString>        Pm_classification[6];
	//254����������
	map<CString,int>     basePm_type_;
	//Сʱ����С�����ֵĴ���
	map<CString,int>     hourCellID_cnt_;
	//����Сʱ���� 
	map_ObjType	         hour_Pm_object_;
	//����С��Ԥ��
	CPerformanceWarning  Pm_warning_;                                                           
	//------------------------------------------------------------------------------------------------------
	//���ܴ�����
	//------------------------------------------------------------------------------------------------------
	//�๦�ܳ�ʼ��
	void Init(); 
	//�๦�ܸ���
	void Updata(); 
	//������ܷ���
	void GetPmClassification(list<CString> *Pm_classification);
	//��û�����������
	void GetBasePmType();
	//����4��15������������
	void SaveHourPmData(map_ObjType &ObjType); 
	//�򿪴�����TXT�ļ�
	void OpenPmFile(CString directory_path,CString file_name);                                            
	//�ر�TXT�ļ�
	void ClosePmFile();
	//��cell�������ݷֽ�
	void ProcessData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification); 
	//��cell�������ݷֽ�
	void ProcessHourData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification); 
	//���������������
	void WriteBasePm(list<CString> *Pm_classification,File_Object &file_object);                      
	//bulk insert�����ջ
	void PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack);   
	//���С��ӳ��ID����ͳ��С��Сʱ���ȳ��ִ���
	void  GetCellID(map<CString,int> &hourCellID_cnt_);
	//Сʱ����count��ƽ��ֵ
	void meanHourPmCount(map_ObjType &ObjType);
};

