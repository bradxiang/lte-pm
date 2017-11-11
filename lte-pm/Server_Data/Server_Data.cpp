
// Server_Data.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Server_Data.h"
#include "Server_DataDlg.h"
#include "RSA.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServer_DataApp

BEGIN_MESSAGE_MAP(CServer_DataApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServer_DataApp 构造

CServer_DataApp::CServer_DataApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CServer_DataApp 对象

CServer_DataApp theApp;


//------------------------------------------------------------------------------------------------------			
//	函数功能：CServer_DataApp 初始化
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//应用程序处理
	AfxEnableControlContainer();										//初始化：容器

	::CoInitialize(NULL);												//初始化com	

	// 标准初始化如果未使用这些功能并希望减小最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	////////////////////////////////////////////////////////////////////////////////////////////////////			
	//开启一个用应用程序名创建信号量，用于：只能运行一次该软件 [退出软件时，需要移除该信号量]
	HANDLE hSem = CreateSemaphore(NULL,1,1,m_pszAppName);				//生成：信号量句柄  
	if   (GetLastError()   ==   ERROR_ALREADY_EXISTS)					//信号量已存在？   
	{   																//失败：即号量存在，则程序已有一个实例运行   
		CloseHandle(hSem);   											//关闭：信号量句柄   
		//寻找先前已经打开的窗口   
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD);	//获得：窗体
		while (::IsWindow(hWndPrevious))								//有效窗体？
		{   
			if(::GetProp(hWndPrevious,m_pszAppName))					//判断：[从给定窗口的属性列表中检索数据句柄]
			{															//有，则是寻找的主窗   
				if (!::IsIconic(hWndPrevious))							//主窗口没有最小化？
					ShowWindow(hWndPrevious,SW_SHOWMINIMIZED);		//最小化：主窗口
				ShowWindow(hWndPrevious,SW_SHOWMAXIMIZED);				//最大化：主窗口
				return   false;   										//退出本实例   
			}   
			hWndPrevious = ::GetWindow(hWndPrevious,GW_HWNDNEXT); 		//获得：继续寻找下一个窗口
		}   
		AfxMessageBox("程序已运行！");									//前一实例已存在，但找不到其主窗，可能出错了   
		return   false;													//退出本实例   
	}  

	////////////////////////////////////////////////////////////////////////////////////////////////////			
	//获取程序默认目录
	const int nBufSize = 512;											//尺寸：缓冲区
	TCHAR chBuf[nBufSize];												//定义：缓冲区
	ZeroMemory(chBuf,nBufSize);											//复位：缓冲区
	::GetModuleFileNameA(NULL,chBuf,nBufSize);							//获取默认文件路径
	TCHAR* lpStrPath = chBuf;											//指针
	PathRemoveFileSpec(lpStrPath);										//去除：文件名称
	m_sPath=chBuf;														//获取：默认路径

	////////////////////////////////////////////////////////////////////////////////////////////////////			
	//读取配置文件：config.ini
	char v_cBuffer[512];
	GetPrivateProfileString("DATABASE_SERVER","DATABASE_SERVER",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_Ip.Format("%s",v_cBuffer);																//数据库服务器地址
	GetPrivateProfileString("DATABASE_NAME","DATABASE_NAME",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_Name.Format("%s",v_cBuffer);															//获得数据库名称
	GetPrivateProfileString("DATABASE_USER","DATABASE_USER",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_User.Format("%s",v_cBuffer);															//获得数据库用户
	GetPrivateProfileString("DATABASE_PASSWORD","DATABASE_PASSWORD",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_Password.Format("%s",v_cBuffer);														//获得数据库密码
	a_iDatabase_Type=GetPrivateProfileInt("DATABASE_TYPE","DATABASE_TYPE",0,".\\config.ini");			//获得数据库类型
	GetPrivateProfileString("ALARMNOTIFY_PORT","ALARMNOTIFY_PORT",NULL,v_cBuffer,512,".\\config.ini");
	CString v_sPort="";
	v_sPort.Format("%s",v_cBuffer);
	a_iAlarmNotifyPort=atoi(v_sPort);									//UDP：告警通知端口
	GetPrivateProfileString("LICENCE_KEY","LICENCE_KEY",NULL,v_cBuffer,512,".\\config.ini");
	CString v_sKey="";
	v_sKey.Format("%s",v_cBuffer);
	GetPrivateProfileString("PM_PATH","PM_PATH",NULL,v_cBuffer,512,".\\config.ini");
	a_sPm_Path_.Format("%s",v_cBuffer);
	GetPrivateProfileString("SAVE_DIRECTORY","SAVE_DIRECTORY",NULL,v_cBuffer,512,".\\config.ini");
	a_save_directory_path_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_IP","NET_DISK_IP",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_IP_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_DRIVE","NET_DISK_DRIVE",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_drive_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_USER","NET_DISK_USER",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_user_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_PASSWD","NET_DISK_PASSWD",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_passwd_.Format("%s",v_cBuffer);
	GetPrivateProfileString("AUTO_START","AUTO_START",NULL,v_cBuffer,512,".\\config.ini");
	a_auto_start_.Format("%s",v_cBuffer);
	//对数据库密码和key解密
	InitMap();															//各市信息初始化
	if(!DecrypteParam(v_sKey.Trim()))									//获得：密码运算[数据库密码、电话区号、有效日期]
		return FALSE;													//返回：无效

	//位置信息
	a_sLocation[0]="大连";
	a_sLocation[1]="DL";

	if(a_sDatabase_Ip.IsEmpty())
	{
		AfxMessageBox("系统配置文件( config.ini )不存在！！！");										//异常：配置文件没找到   
		return   false;																					//系统退出
	}

	//Log：程序开始
	theApp.m_MyLog.WriteText("系统开始********************\r\n");		//Log

	m_sAppName=m_pszAppName;											//应用程序名称 [程序运行一次需要]
	CServer_DataDlg dlg;												//主窗体
	m_pMainWnd = &dlg;

	dlg.DoModal();														//打开：主窗体

	//Log：程序退出
	theApp.m_MyLog.WriteText("程序退出--------------------\r\n");		//log

	return FALSE;
}

//------------------------------------------------------------------------------------------------------
//	功能：各市信息初始化
//------------------------------------------------------------------------------------------------------
void CServer_DataApp::InitMap(void)
{
	m_mapProvince.SetAt("0024","沈阳;SY");
	m_mapProvince.SetAt("0410","铁岭;TL");
	m_mapProvince.SetAt("0411","大连;DL");								//名称:标记
	m_mapProvince.SetAt("0412","鞍山;AS");
	m_mapProvince.SetAt("0413","抚顺;FS");
	m_mapProvince.SetAt("0414","本溪;BX");
	m_mapProvince.SetAt("0415","丹东;DD");
	m_mapProvince.SetAt("0416","锦州;JZ");
	m_mapProvince.SetAt("0417","营口;YK");
	m_mapProvince.SetAt("0418","阜新;FX");
	m_mapProvince.SetAt("0419","辽阳;LY");	
	m_mapProvince.SetAt("0421","朝阳;CY");
	m_mapProvince.SetAt("0427","盘锦;PJ");	
	m_mapProvince.SetAt("0429","葫芦岛;HL");	
}

//------------------------------------------------------------------------------------------------------
//	功能：密码验证
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataApp::DecrypteParam(CString key)
{
	CRSA Rsa;
	Rsa.SetDecrypteParam(PASSWORDRSAD,PASSWORDRSAN);
	a_sDatabase_Password=Rsa.DecryptStr(a_sDatabase_Password);			//解析：数据库密码

	Rsa.SetDecrypteParam(KEYRSAD,KEYRSAN);
	key=Rsa.DecryptStr(key);											//解析：电话区号、有效日期
	if(key.GetLength()!=12)												
	{
		AfxMessageBox("Key 错误！");									//错误
		return FALSE;													//返回：无效
	}
	//解析Key中的信息：电话区号、有效日期
	CString v_sPostCode="",v_sDate="",v_sProInfo="",v_sTemp="";
	int v_iYear,v_iMonth,v_iDay;
	v_sPostCode=key.Left(4);											//电话区号
	key.Delete(0,4);
	v_sDate=key;														//有效日期

	//时间转换--------------------------------------------------
	if(v_sDate.GetLength()<8)											//日期长度<8
	{
		AfxMessageBox("Key 错误！");									//错误
		return FALSE;													//返回：无效
	}
	v_sTemp=v_sDate.Left(4);											//年
	v_sDate.Delete(0,4);
	v_iYear=atoi(v_sTemp);
	v_sTemp=v_sDate.Left(2);
	v_sDate.Delete(0,2);
	v_iMonth=atoi(v_sTemp);												//月
	if(v_iMonth<0||v_iMonth>12)
	{
		AfxMessageBox("Key 错误！");
		return FALSE;
	}
	v_iDay=atoi(v_sDate);												//日
	if(v_iDay<0||v_iDay>31)
	{
		AfxMessageBox("Key 错误！");
		return FALSE;
	}
	CTime v_tDeadLine(v_iYear,v_iMonth,v_iDay,0,0,0);					//设置：有效日期
	a_tDeadLine=v_tDeadLine;

	//获取地理位置信息------------------------------------------
	m_mapProvince.Lookup(v_sPostCode,v_sProInfo);						//容器查找：依据：电话区号 [如：0411]
	if(v_sProInfo=="")													//没找到？？？		
	{
		AfxMessageBox("Key 错误！");									//返回：错误
		return FALSE;
	}
	int index=v_sProInfo.Find(";");	
	a_sLocation[0]=v_sProInfo.Left(index);								//名称：[0]="大连"
	a_sLocation[1]=v_sProInfo.Right(v_sProInfo.GetLength()-index-1);	//标记：[1]="DL"
	return TRUE;														//返回：有效
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
