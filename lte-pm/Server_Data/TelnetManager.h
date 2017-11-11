//======================================================================================================
//	功能：Telnet处理函数，用于Telent的处理动作【连接、发送、接收、关闭】
//  范围：告警采集、告警分析；
//======================================================================================================
#pragma once
#include  <afxmt.h> 
#include "Server_Data.h"
#include "ThreadPool_IOCP.h"


class CTelnetManager
{
public:
	//------------------------------------------------------------------------------------------------------
	//	函数定义	
	//------------------------------------------------------------------------------------------------------
	CTelnetManager(void);
	CTelnetManager(LPVOID pdlg);
	~CTelnetManager(void);

	//告警获取【Telnet】
	u_char		AddCheckBits(u_char* sendData);							//计算校验函数

	BOOL		Start();												//开启系统
	BOOL		ShutDowm();												//telnet关闭
	
	//线程相关
	CWinThread* AlarmThread[1];											//线程数组	
	HANDLE		hMutex[1] ;												//资源释放同步
	static UINT My_Thread_Listen(LPVOID lparam);						//监听 接收线程
	BOOL		v_bThreadAlive;											//线程标志：设置后让线程退出

	//------------------------------------------------------------------------------------------------------
	//	变量定义
	//------------------------------------------------------------------------------------------------------
	LPVOID	pTheDlg;													//窗口指针
	CCriticalSection TaskListLock;										//TaskList的同步锁 
	SOCKET	v_cSocket_Listen;											//Listen Socket
	SOCKET	v_cSocket_Connect;											//Connect Socket 

	//完成端口变量
	CThreadPool			v_cIocp;										//IOCP完成端口 

	//信息获取
	CCriticalSection	v_cTelnet_Text_Lock;							//同步锁：Telent信息
	CString				v_sTelnet_Text;									//指令执行结果
	CCriticalSection	v_cTelnet_Lock;									//同步锁：客户端信息

	//客户端信息
	map<SOCKET,v_sClient_Connect> v_mClient_Connect;					//客户端信息
};

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------
