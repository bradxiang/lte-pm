#pragma once
//XML�ļ�����
class CMyFile_XML;
//��ͼ��
class CServer_DataDlg;
//С������
class CMyFilter;
//Ԥ����
#include "PerformanceWarning.h"
//class CPerformanceWarning;

class RegionPmProcessing
{
public:
	RegionPmProcessing(void);
	~RegionPmProcessing(void);
	//------------------------------------------------------------------------------------------------------
	//����
	//------------------------------------------------------------------------------------------------------
	//�����������ܹ�ʽ
	map_Counter         map_formula_; 
	//��������
	map_Counter         basePm_name_;
	//��ǰ����ʱ��
	CString             current_granularity_;  
	//�ϸ���������ʱ��
	CString             before_granularity_;   
	//���㹫ʽ�ṹ
	struct PM_Formula_Struct								      
	{
		CString		v_sPM_Formula;								  //�����/����ֵ
		bool		v_bFormula;								      //1������ֵ��		0���������
	};
	//���ܹ�ʽ���У�mPM_Formula
	typedef list<PM_Formula_Struct> mPM_Formula;	
	//��������Ԥ��
	CPerformanceWarning		 Pm_warning_;

	//------------------------------------------------------------------------------------------------------
	//ȫ������
	//------------------------------------------------------------------------------------------------------
	//���浥�ļ�ȫ������������������
	map_Counter         network_Pm_iterm_;
	//��������ȫ������������������
	map_Counter         network_grantularity_Pm_iterm_;   
	//ȫ������ָ��
	map_Counter         network_Pm_;

	//------------------------------------------------------------------------------------------------------
	//��Ϊ����
	//------------------------------------------------------------------------------------------------------
	//���浥�ļ�ȫ������������������
	map_Counter         hw_Pm_iterm_;
	//��������ȫ������������������
	map_Counter         hw_grantularity_Pm_iterm_;   
	//��������ָ��
	map_Counter         hw_Pm_;

	//------------------------------------------------------------------------------------------------------
	//���˱���
	//------------------------------------------------------------------------------------------------------
	//���浥�ļ�ȫ������������������
	map_Counter         zte_Pm_iterm_;
	//��������ȫ������������������
	map_Counter         zte_grantularity_Pm_iterm_;   
	//��������ָ��
	map_Counter         zte_Pm_;

	//------------------------------------------------------------------------------------------------------
	//�������
	//------------------------------------------------------------------------------------------------------
	//���ļ��������������������
	map<CString,map_Counter> region_Pm_iterm_;                             
	//���������������������������
	map<CString,map_Counter> region_grantularity_Pm_iterm_;    
	//С��������������
	map_Counter              cell_region_;     
	//���浥�ļ���������������������
	map_Counter				 other_Pm_iterm_;
	//����������������������������
	map_Counter				 other_grantularity_Pm_iterm_;  
	//��������ָ��
	map_Counter				 other_Pm_;

	//------------------------------------------------------------------------------------------------------
	//	��ʼ��
	//------------------------------------------------------------------------------------------------------
	//�๦�ܳ�ʼ��
	void Init(); 
	//�๦�ܳ�ʼ��
	void Updata(); 
	//��û����������Ʊ���
	void GetBasePm();
	//�����ݿ��ж�ȡ���ܹ�ʽ
	void GetRegionFomula(); 
	//���������ϴ����ݿ�
	void UploadingPm(CString time,map_Counter &Pm,CString region);
	//�ֽ����ܹ�ʽ
	void DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula); 
	//�ϳ����ܹ�ʽ
	void CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula);  
	//���С����������
	void GetCellAndRegion();
	//����������������
	void GetRegionandPm();

	//------------------------------------------------------------------------------------------------------
	//	ȫ�����ܼ��㺯��
	//------------------------------------------------------------------------------------------------------
	//ȫ�����ļ����������������
	void SumBasePmForWholeNetwork(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//ȫ��ÿ�����Ȼ��������������
	void SumBasePmInGranularityForWholeNetwork();
	//ȫ���������ܼ���
	void CalculatePmForWholeNetwork();

	//------------------------------------------------------------------------------------------------------
	//	������������ܼ��㺯��
	//------------------------------------------------------------------------------------------------------
	//������������ļ����������������
	void SumBasePmForRegionandOther(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//����ÿ�����Ȼ��������������
	void SumBasePmInGranularityForRegion();
	//�����������ܼ���
	void CalculateandUploadingPmForRegion(CString time);
	//����ÿ�����Ȼ��������������
	void SumBasePmInGranularityForOther();
	//�����������ܼ���
	void CalculatePmForOther();

	//------------------------------------------------------------------------------------------------------
	//	��Ϊ���ܼ��㺯��
	//------------------------------------------------------------------------------------------------------
	//��Ϊ���ļ����������������
	void SumBasePmForHW(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//��Ϊÿ�����Ȼ��������������
	void SumBasePmInGranularityForHW();
	//��Ϊ�������ܼ���
	void CalculatePmForHW();

	//------------------------------------------------------------------------------------------------------
	//	�������ܼ��㺯��
	//------------------------------------------------------------------------------------------------------
	//���˵��ļ����������������
	void SumBasePmForZTE(map_ObjType &v_mObjType,CMyFilter &cell_filter);
	//����ÿ�����Ȼ��������������
	void SumBasePmInGranularityForZTE();
	//�����������ܼ���
	void CalculatePmForZTE();
};

