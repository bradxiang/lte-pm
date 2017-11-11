#pragma once
//视图类
class CServer_DataDlg;
//框架类
#include "Server_Data.h"

class DBMaintenance
{
public:
	DBMaintenance(void);
	~DBMaintenance(void);
	//变量
	CString pm_save_month_;
	CString pm_save_day_;
	CString log_save_day_;
	CString granularity_time_;
	//初始化
	void Init();
	//判断是否已经创建下月表
	bool JudgeHourGranularityTableExist(CString sTableTime);
	bool JudgeThisMonthHourGranularityTableExist(CString sTableTime);
	//创建小时粒度性能表
	void CreateHourGranularityTable(CString sTableTime);
	void CreateThisMonthHourGranularityTable(CString sTableTime);
	//判断是否已经创建15分钟粒度表
	bool JudgeQuarterGranularityTableExist(CString sTableTime);
	//创建15分钟粒度性能表
	void CreateQuarterGranularityTable(CString sTableTime);
	//判断是否更新CON_Pm_LTE_Cal_Next_Month表？
	bool JudgeCalculateTableUpdate();	
	//判断是否更新CON_Pm_LTE_Cal_This_Month表？
	bool JudgeThisCalculateTableUpdate();
	//创建下个月标准公式性能表,如果存在删除后新建
	void UpdateCONPmLTECalNextMonthTable();
	//创建标准公式性能表,如果存在删除后新建
	void UpdateCONPmLTECalThisMonthTable();
	//删除过期15分钟粒度表
	void DeleteStaleData(CString delete_time);
	//CON_System表数据获取
	bool GetSystemParameter();
	//删除目录及目录下GZ文件
	void DeleteFolderandFile(CString sFolderName,CString sFolderPath);
	//删除目录中的过期txt文件文件
	void DeleteBulkInsertFile(CString grantularity_time,CString sFolderPath);
};

