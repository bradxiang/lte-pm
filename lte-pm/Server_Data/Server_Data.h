
// Server_Data.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CServer_DataApp:
// 有关此类的实现，请参阅 Server_Data.cpp
//

class CServer_DataApp : public CWinAppEx
{
public:
	CServer_DataApp();

// 重写
	DECLARE_MESSAGE_MAP()												//消息映射	

	virtual BOOL InitInstance();										//初始化

public:
	//------------------------------------------------------------------------------------------------------
	//	函数
	//------------------------------------------------------------------------------------------------------
	void	InitMap(void);												//各市信息初始化
	BOOL	DecrypteParam( CString key);								//密码验证

	//------------------------------------------------------------------------------------------------------
	//	变量
	//------------------------------------------------------------------------------------------------------
	//数据库参数
	CString				a_sDatabase_Ip;									//数据库服务器地址
	CString				a_sDatabase_Name;								//数据库名称
	CString				a_sDatabase_User;								//数据库用户
	CString				a_sDatabase_Password;							//数据库密码
	int					a_iDatabase_Type;								//数据库类型

	//系统参数
	CString				a_sLocation[2];									//配置的市信息：  [0]：市名称；	[1]：市标记；		如：[0]="大连"；	[1]="DL"；
	CTime				a_tDeadLine;									//软件有效日期
	int					a_iAlarmNotifyPort;								//告警通知端口
	CString				a_sLogFile_Path;								//Log文件路径
	CString             a_sPm_Path_;                                    //性能文件所在目录
	CString             a_save_directory_path_;                         //TXT文件保存目录
	CString				a_net_disk_IP_;                                 //网盘IP
	CString				a_net_disk_drive_;								//网盘盘符
	CString             a_net_disk_user_;                               //网盘用户名
	CString             a_net_disk_passwd_;								//网盘密码
	CString             a_auto_start_;
	//自定义变量	
	CMyLogWriter		m_MyLog;										//写日志
	CString				m_sPath;										//默认文件路径
	CString				m_sAppName;										//默认：应用程序名称 [程序运行一次需要]

	CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapProvince;				//容器：各市信息
};

extern CServer_DataApp theApp;

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
