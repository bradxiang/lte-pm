#pragma once
//��ͼ��
class CServer_DataDlg;
//�����
#include "Server_Data.h"

class DBMaintenance
{
public:
	DBMaintenance(void);
	~DBMaintenance(void);
	//����
	CString pm_save_month_;
	CString pm_save_day_;
	CString log_save_day_;
	CString granularity_time_;
	//��ʼ��
	void Init();
	//�ж��Ƿ��Ѿ��������±�
	bool JudgeHourGranularityTableExist(CString sTableTime);
	bool JudgeThisMonthHourGranularityTableExist(CString sTableTime);
	//����Сʱ�������ܱ�
	void CreateHourGranularityTable(CString sTableTime);
	void CreateThisMonthHourGranularityTable(CString sTableTime);
	//�ж��Ƿ��Ѿ�����15�������ȱ�
	bool JudgeQuarterGranularityTableExist(CString sTableTime);
	//����15�����������ܱ�
	void CreateQuarterGranularityTable(CString sTableTime);
	//�ж��Ƿ����CON_Pm_LTE_Cal_Next_Month��
	bool JudgeCalculateTableUpdate();	
	//�ж��Ƿ����CON_Pm_LTE_Cal_This_Month��
	bool JudgeThisCalculateTableUpdate();
	//�����¸��±�׼��ʽ���ܱ�,�������ɾ�����½�
	void UpdateCONPmLTECalNextMonthTable();
	//������׼��ʽ���ܱ�,�������ɾ�����½�
	void UpdateCONPmLTECalThisMonthTable();
	//ɾ������15�������ȱ�
	void DeleteStaleData(CString delete_time);
	//CON_System�����ݻ�ȡ
	bool GetSystemParameter();
	//ɾ��Ŀ¼��Ŀ¼��GZ�ļ�
	void DeleteFolderandFile(CString sFolderName,CString sFolderPath);
	//ɾ��Ŀ¼�еĹ���txt�ļ��ļ�
	void DeleteBulkInsertFile(CString grantularity_time,CString sFolderPath);
};

