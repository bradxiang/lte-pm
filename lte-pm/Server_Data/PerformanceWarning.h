#pragma once
class CPerformanceWarning
{
public:
	CPerformanceWarning(void);
	~CPerformanceWarning(void);
//------------------------------------------------------------------------------------------------------
//	����Ԥ��������
//------------------------------------------------------------------------------------------------------
map<CString,map<CString,CString>>         formula_performance_warning_;                      //��ʽ����Ԥ��
map<CString,map<CString,CString>>         region_performance_warning_;                       //��������Ԥ��
map<CString,map<CString,CString>>         base_performance_warning_;                         //��������Ԥ��
CString     file_time_;                                                                      //��ȡ��ʱ��
map<CString,CString>                     cellID_map_name_;                                   //key:С��ID,value:С������

//------------------------------------------------------------------------------------------------------
//	����Ԥ��������
//------------------------------------------------------------------------------------------------------

//��ʼ��
void Init();
//���С��ID��С������ӳ��
void GetCellMap();                                                                 
//�����ݿ��ж�ȡ��������Ԥ������
void GetRegionPmWarning();	
//�����ݿ��ж�ȡ��ʽ����Ԥ������
void GetFormulaPmWarning();
//�����ݿ��ж�ȡ��������Ԥ������
void GetBasePmWarning();
//С����������Ԥ��
void CompareToFormulaPerformance(map_ObjType &performance_object); 
//С����������Ԥ��
void CompareToBasePerformance(map_ObjType &performance_object); 
//ȫ������Ԥ��
void CompareToWholeNetworkPerformance(map_Counter  whole_network_performance);
//ZTE����Ԥ��
void CompareToZTEPerformance(map_Counter  whole_network_performance); 
//HW����Ԥ��
void CompareToHWPerformance(map_Counter  whole_network_performance);        
//��������Ԥ��
void CompareToOtherPerformance(map_Counter  whole_network_performance);           
//��������Ԥ��
void CompareToRegionPerformance(map_Counter  whole_network_performance,CString region_name);    
};

