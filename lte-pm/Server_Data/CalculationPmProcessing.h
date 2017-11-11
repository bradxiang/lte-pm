#pragma once
//��ͼ��
class CServer_DataDlg;
//Ԥ����
#include "PerformanceWarning.h"

class CalculationPmProcessing
{
public:
	CalculationPmProcessing(void);
	~CalculationPmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//����
	//------------------------------------------------------------------------------------------------------
	//���㹫ʽ�ṹ
	struct PM_Formula_Struct								      
	{
		CString		v_sPM_Formula;								  //�����/����ֵ
		bool		v_bFormula;								      //1������ֵ��		0���������
	};
	//���ܹ�ʽ���У�mPM_Formula
	typedef list<PM_Formula_Struct> mPM_Formula;				  
	//�ļ�ʵ�� 
	CFile			formula_Pm_file_;  
	//�������ܶ���
	map_ObjType		Pm_object_;							          
	//���汾��С�����ܹ�ʽ
	map_Counter		map_formula_;
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
	//��û�����������
	void GetBasePmType();
	//����4��15������������
	void SaveHourPmData(map_ObjType &ObjType); 
	//�����ݿ��ж�ȡ���ܹ�ʽ
	void GetFomula();                                         
	//�ϳ����ܹ�ʽ
	void CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula);      
	//����һ���ӹ�ʽ���ܵ�TXT
	void SaveFormulaPmToTXT(CString sFilePath,CString XML_file_name,CString file_time);                     
	//�ֽ����ܹ�ʽ
	void DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula);               
	//����С��15���ӹ�ʽ����
	void CalculatePerformance(map_ObjType &ObjType,CString time);     
	//����Сʱ����С����ʽ����
	void CalculateHourPerformance(CString time);
	//bulk insert�����ջ
	void PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack);
	//���С��ӳ��ID����ͳ��С��Сʱ���ȳ��ִ���
	void  GetCellID(map<CString,int> &hourCellID_cnt_);
	//Сʱ����count��ƽ��ֵ
	void meanHourPmCount(map_ObjType &ObjType);

};


