//======================================================================================================
//	功能：Telnet处理函数，用于Telent的处理动作【连接、发送、接收、关闭】
//  范围：告警采集、告警分析；
//======================================================================================================
#include "StdAfx.h"
#include "TelnetManager.h"
#include "Server_DataDlg.h"

void CALLBACK My_IOCP_Task(unsigned long long* v_pClient, const char *v_pBuffer, int v_iLength);
void CALLBACK My_IOCP_Colse(unsigned long long* v_pClient);

//------------------------------------------------------------------------------------------------------			
//	函数功能：构造函数
//------------------------------------------------------------------------------------------------------			
CTelnetManager::CTelnetManager(void)
{
}

CTelnetManager::CTelnetManager(LPVOID pdlg)
:pTheDlg(pdlg),
v_bThreadAlive(TRUE)
{
	
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：析构函数
//------------------------------------------------------------------------------------------------------			
CTelnetManager::~CTelnetManager(void)
{
	v_mClient_Connect.clear();											//容器清除
}

//-------------------------------------------------------------------------------------------------
//	功能：	IOCP任务处理回调函数 [接收IOCP的网络数据，放入数据缓冲区]
//	参数：	(1)v_pClient：完成Key结构；	(2)v_pBuffer：接收数据区指针；	(3)v_iLength：接收数据长度；
//	过程：	获取信息，放入指令队列；
//-------------------------------------------------------------------------------------------------
void CALLBACK My_IOCP_Task(unsigned long long* v_pClient, const char *v_pBuffer, int v_iLength)
{
	v_sCommand			v_cCommand;										//指令结构
	u_char				v_chFunCode;									//临时变量
	int					v_iLen,v_iHead;									//临时变量
	CString				v_sTemp;										//临时变量
	bool				v_bOK;											//临时变量

	CServer_DataDlg* pDlg;
	pDlg=(CServer_DataDlg*)theApp.m_pMainWnd;							//获得对话窗窗口指针
	CTelnetManager* pthis=(CTelnetManager*)&pDlg->TelnetManager;		//TelnetManager指针

	v_sClient_Connect* v_pClient_Data=(v_sClient_Connect*)v_pClient;	//获得：数据指针

	if (v_iLength<=2000)												//信息最低长度？？？
	{
		//查找客户端信息
		map<SOCKET,v_sClient_Connect>::iterator v_pIter;					//迭代器
		v_pIter=pthis->v_mClient_Connect.find(v_pClient_Data->v_iSocket);	//查找？？？
		if (v_pIter!=pthis->v_mClient_Connect.end())						//找到？？？[SOCKET]
		{
			//拷贝接收数据到客户端接收缓冲区
			memcpy(v_pIter->second.v_cReceData+v_pIter->second.v_iLen,v_pBuffer,v_iLength);		//数据拷贝
			v_pIter->second.v_iLen+=v_iLength;							//数据长度
			//接收数据处理
			v_iHead=0;
			while(v_pIter->second.v_iLen != 0)							//循环处理[接收数据中可能有多帧数据]
			{
				v_bOK=false;											//复位：退出标记
				if(v_pIter->second.v_cReceData[v_iHead]!='C'|| v_pIter->second.v_cReceData[v_iHead+1]!='M')		//数据包头？？？
				{
					v_iHead++;											//包头位置++
					if (v_iHead>=v_pIter->second.v_iLen-1)				//达到数据尾部？？？
						break;											//退出
					continue;											//继续
				}

				//接收数据处理[帧处理]
				while(v_pIter->second.v_cReceData[v_iHead]=='C'&& v_pIter->second.v_cReceData[v_iHead+1]=='M')	//数据包头？？？
				{
					v_iLen=(u_char)v_pIter->second.v_cReceData[v_iHead+2]*256+(u_char)v_pIter->second.v_cReceData[v_iHead+3];		//数据长度
					if (v_iLen>1100)									//帧长度异常？？？？
					{
						v_iHead++;										//包头位置++
						if (v_iHead>=v_pIter->second.v_iLen-1)			//达到数据尾部？？？
							break;										//退出
						continue;										//继续
					}
					if (v_iHead+v_iLen>v_pIter->second.v_iLen)			//数据不全？？？
					{
						v_bOK=true;										//置位：退出标记
						break;
					}
					u_char v_chJundgeCode=v_pIter->second.v_cReceData[v_iHead+v_iLen-1];							//获得：发送数据校验码
					u_char v_chJundgeCodeLast=pthis->AddCheckBits((u_char*)v_pIter->second.v_cReceData+v_iHead);	//计算：接收数据校验码
					if(v_chJundgeCode!=v_chJundgeCodeLast)				//数据校验 OK ？？？
					{
						v_iHead++;										//包头位置++
						if (v_iHead>=v_pIter->second.v_iLen-1)			//达到数据尾部？？？
							break;										//退出
						continue;										//继续
					}
					v_chFunCode=v_pIter->second.v_cReceData[v_iHead+4];	//接收数据功能码
					if (v_chFunCode==0x01 || v_chFunCode==0x02 || v_chFunCode==0x03 || v_chFunCode==0x05 || v_chFunCode==0x04)	//心跳、登录、指令、BSC采集异常、指令执行结果
					{
						v_sTemp="";										//接收数据
						for(int vi=5;vi<v_iLen-1;vi++)					//数据循环
							v_sTemp+=(u_char)v_pIter->second.v_cReceData[v_iHead+vi];		//获得：数据【字符串】
						v_cCommand.v_iCode=v_chFunCode;					//指令：功能码
						v_cCommand.v_sText=v_sTemp;						//指令：信息
						v_cCommand.v_iSocket=v_pClient_Data->v_iSocket;	//指令：客户端SOCKET
						pDlg->v_cLock_Command.Lock();					//加锁
						if(v_chFunCode==03 || v_chFunCode==05)
							pDlg->v_lCommand.push_back(v_cCommand);		//增加：指令链表
						else
							pDlg->v_lConnect.push_back(v_cCommand);		//增加：连接链表
						pDlg->v_cLock_Command.Unlock();					//解锁
					}
					v_iHead+=v_iLen;									//数据位置调整
					break;												//退出
				}

				//数据搬迁
				if (v_iHead!=0)											//头位置需要搬迁
				{
					if (v_pIter->second.v_iLen>v_iHead)					//剩余数量有效？？？
					{
						for(int vi=0;vi<v_pIter->second.v_iLen-v_iHead;vi++)							//数据循环
							v_pIter->second.v_cReceData[vi]=v_pIter->second.v_cReceData[vi+v_iHead];	//数据搬迁
					}
					v_pIter->second.v_iLen-=v_iHead;					//计算：数据长度
					v_iHead=0;											//头位置调整
				}
				if (v_bOK)												//退出标记？？？[数据不全，不够一个完整的帧]
					break;												//退出
			}
		}
	}
	pthis->v_cIocp.RecvDataRequest(v_pClient_Data->v_iSocket);			//投递客户端接收信息请求
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：计算校验函数
//------------------------------------------------------------------------------------------------------
u_char  CTelnetManager::AddCheckBits(u_char* sendData)
{
	int v_ilen=(u_char)sendData[2]*256+(u_char)sendData[3];				//数据长度
	if(v_ilen<6)														//无效数据？？？
		return FALSE;													//无效返回
	u_char v_chSun=0x00;												//默认校验和
	for (int i=0;i<v_ilen-1;i++)										//数据遍历
		v_chSun+=sendData[i];											//计算：校验和
	return 0xFF-v_chSun;												//返回：校验和
}

//-------------------------------------------------------------------------------------------------
//	功能：	IOCP客户端关闭回调函数 [客户端关闭，清除客户端信息]
//	参数：	(1)v_cSocket；
//-------------------------------------------------------------------------------------------------
void CALLBACK My_IOCP_Colse(unsigned long long* v_pClient)
{
	CString v_sTemp;													//临时变量

	CServer_DataDlg* pDlg;
	pDlg=(CServer_DataDlg*)theApp.m_pMainWnd;							//获得对话窗窗口指针
	CTelnetManager* pthis=(CTelnetManager*)&pDlg->TelnetManager;		//Telent指针

	v_sClient_Connect* v_pClient_Data=(v_sClient_Connect*)v_pClient;	//获得：数据指针

	//客户端信息处理【(1)关闭客户Socket；(2)从客户列表清除客户显示信息；(3)从客户容器清除客户信息；】
	pthis->v_cTelnet_Lock.Lock();										//加锁

	//(1)关闭客户Socket
	if (v_pClient_Data->v_iSocket != NULL)
		closesocket(v_pClient_Data->v_iSocket);							//关闭SOCKET
	//(2)从客户列表清除客户显示信息
	for (int vi=0;vi<pDlg->m_cList_Client.GetItemCount();vi++)			//遍历：客户列表
	{
		if (v_pClient_Data->v_iSocket==atoi(pDlg->m_cList_Client.GetItemText(vi,3)))	//找到？？？【依据：客户SOCKET】
		{
			if (pDlg->m_cList_Client.GetItemText(vi,4)=="采集服务器")	//是采集服务器？？？
			{
				pDlg->v_sServer_Socket=NULL;							//复位：采集端SOCKET
				pDlg->m_cServer.EnableWindow(false);					//无效：采集端
			}
			pDlg->m_cList_Client.DeleteItem(vi);						//清除客户列表信息
			break;														//退出
		}
	}
	//列表序号调整
	for (int vi=0;vi<pDlg->m_cList_Client.GetItemCount();vi++)			//遍历：客户列表
	{
		v_sTemp.Format("%04d",vi+1);
		pDlg->m_cList_Client.SetItemText(vi,0," "+v_sTemp);				//序号
	}
	//(3)从客户容器清除客户信息
	map<SOCKET,v_sClient_Connect>::iterator v_pIter;					//迭代器
	v_pIter=pthis->v_mClient_Connect.find(v_pClient_Data->v_iSocket);	//查找？？？
	if (v_pIter!=pthis->v_mClient_Connect.end())						//找到？？？
		pthis->v_mClient_Connect.erase(v_pIter);						//删除客户端信息

	pthis->v_cTelnet_Lock.Unlock();										//解锁
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：开启系统
//------------------------------------------------------------------------------------------------------			
BOOL CTelnetManager::Start()
{
	//告警获取
	//网络数据初始化-------------------------------------------------------------------------------
	WSADATA wsaData;													//socket数据环境
	if (WSAStartup(MAKEWORD(2,1),&wsaData))								//调用Windows Sockets DLL
	{ 
		theApp.m_MyLog.WriteText("异常：网络初始化失败！！！\r\n");		//log
		return false;													//无效返回
	}
	//IOCP初始化-----------------------------------------------------------------------------------
	v_cIocp.Start(THREAD_MAX,THREAD_MAX,My_IOCP_Task,My_IOCP_Colse);	//IOCP初始化(最少，最大，回调函数)
	v_sTelnet_Text="";													//复位

	//开启监听线程---------------------------------------------------------------------------------
	int			v_iRret;												//临时变量
	CString		v_sTemp;												//临时变量
	SOCKADDR_IN	sockStruct;												//临时变量

	v_cSocket_Listen=socket(PF_INET,SOCK_STREAM,0);						//使用TCP/IP创建
	if(v_cSocket_Listen == INVALID_SOCKET)
	{
		theApp.m_MyLog.WriteText("异常：无法创建监听 socket  ！！！\r\n");		//log
		return false;
	}
	sockStruct.sin_family=AF_INET;										//使用TCP/IP协议
	sockStruct.sin_port = htons(theApp.a_iAlarmNotifyPort);				//端口
	sockStruct.sin_addr.S_un.S_addr = 0;								//默认地址
	v_iRret=bind(v_cSocket_Listen,(LPSOCKADDR)&sockStruct,sizeof(sockStruct));
	v_iRret=listen(v_cSocket_Listen,ACCEPT_MAX);						//socket 监听开始
	if(v_iRret==SOCKET_ERROR)
	{
		closesocket(v_cSocket_Listen);									//清除socket
		theApp.m_MyLog.WriteText("异常：监听失败  ！！！\r\n");			//log
		return false;
	}
	hMutex[0]=CreateEvent(NULL, FALSE, FALSE, NULL);					//产生事件
	AlarmThread[0]=AfxBeginThread(My_Thread_Listen,this);				//生成：线程

	return TRUE;
}

//-------------------------------------------------------------------------------------------------
//	功能：服务器监听线程
//	参数：lparam：主程序窗口指针；
//	性能：在指定的端口等待 Client 的接入,并把接入的client的信息放入客户连接池，投递IOCP;
//-------------------------------------------------------------------------------------------------
UINT CTelnetManager::My_Thread_Listen(LPVOID lparam)	
{	
	sockaddr_in v_cStru_Connect;										//socket地址结构
	int			v_iLength;												//临时变量
	CString		v_sTemp,v_sIp;											//临时变量
	v_sClient_Connect	v_cClient_Connect;								//临时变量

	CTelnetManager *pthis=(CTelnetManager *)lparam;						//获得窗口指针
	CServer_DataDlg* pDlg=(CServer_DataDlg*)pthis->pTheDlg;				//对话窗指针
	v_iLength = sizeof(SOCKADDR);

	while(pthis->v_bThreadAlive)										//监听循环体
	{
		//(1)线程在这里将会阻塞，直到客户端的连接请求发来(v_cSocket_Connect是新创建的客户端socket)
		pthis->v_cSocket_Connect=accept(pthis->v_cSocket_Listen,(LPSOCKADDR)&v_cStru_Connect,(LPINT)&v_iLength);
		if(pthis->v_cSocket_Connect == INVALID_SOCKET)					//连接故障，清除连接socket
			break;
		//(2)判断是否有强制退出标志：pDlg->v_bClose
		if(!pthis->v_bThreadAlive)
		{
			closesocket(pthis->v_cSocket_Connect);						//清除连接socket
			break;
		}
		//(3)得到客户端信息，并保存日志
		v_sIp=inet_ntoa(v_cStru_Connect.sin_addr);						//得到客户端IP地址
		//(4)把客户端信息加入客户端信息中，并显示；
		v_cClient_Connect.v_sIP=v_sIp;									//IP
		v_cClient_Connect.v_iSocket=pthis->v_cSocket_Connect;			//Socket
		v_cClient_Connect.v_sName="";									//姓名
		v_cClient_Connect.v_iSum=0;										//心跳数量
		v_cClient_Connect.v_iTimer=0;									//定时器数量
		v_cClient_Connect.v_iLen=0;										//接收数据长度

		map<SOCKET,v_sClient_Connect>::iterator	v_pIter;				//迭代器
		pthis->v_mClient_Connect.insert(pair<SOCKET,v_sClient_Connect>(pthis->v_cSocket_Connect,v_cClient_Connect));
		v_pIter=pthis->v_mClient_Connect.find(pthis->v_cSocket_Connect);//客户端数据指针

		pDlg->My_ShowList_Client(v_sIp,pthis->v_cSocket_Connect,v_cClient_Connect.v_sName);		//显示：客户端信息
		//(5)完成客户SOCKET与工作IOCP完成端口绑定，参数1：客户socket；参数2：工作IOCP端口；参数3：客户连接池地址结构。
		if (CreateIoCompletionPort((HANDLE)pthis->v_cSocket_Connect,pthis->v_cIocp.m_hWorkerIO,
						(unsigned long long)&v_pIter->second,0)!=NULL)
		{
			//(6)向工作IOCP端口投递客户数据接收请求
			pthis->v_cIocp.RecvDataRequest(pthis->v_cSocket_Connect);	//投递客户端接收信息请求
		}
	}
	//(7)关闭线程处理
	closesocket(pthis->v_cSocket_Listen);								//清除socket
	SetEvent(pthis->hMutex[0]);	
	return 0;
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：关闭Telnet 【由于Telnet中有线程，需要通知线程退出】
//------------------------------------------------------------------------------------------------------			
BOOL CTelnetManager::ShutDowm()
{
	v_bThreadAlive=FALSE;												//设置退出标志
	if (v_cSocket_Listen !=NULL)										//SOCKET有效？？？
		closesocket(v_cSocket_Listen);									//关闭SOCKET
	WaitForMultipleObjects(1,hMutex,TRUE,INFINITE);						//等待线程退出
	v_cIocp.Stop();														//IOCP关闭
	return TRUE;
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
