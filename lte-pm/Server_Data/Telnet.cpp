// Telnet.cpp: implementation of the CTelnet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Telnet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------			
//	函数功能：构造函数
//------------------------------------------------------------------------------------------------------			
CTelnet::CTelnet(void)
{
	v_iIOCP_Num=0;														//
}
//------------------------------------------------------------------------------------------------------			
//	函数功能：析构函数
//------------------------------------------------------------------------------------------------------			
CTelnet::~CTelnet(void)
{

}

//------------------------------------------------------------------------------------------------------			
//	函数功能：设置TelnetManager指针
//------------------------------------------------------------------------------------------------------			
void CTelnet::Set_TelnetManager(LPVOID lparam)
{
	v_pTelnetManager=lparam;											//TelnetManager指针
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：Socket生成
//------------------------------------------------------------------------------------------------------			
int CTelnet::Create()
{
	m_hSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_hSocket == INVALID_SOCKET)									//正常？？？
		return -1;														//异常返回
	return 0;															//正常返回
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：Socket连接
//------------------------------------------------------------------------------------------------------			
int CTelnet::Connect(CString strIP,int nPort)
{
	unsigned long ip;

	if((ip = inet_addr(strIP.GetBuffer(0))) == INADDR_NONE)
		printf("invalid host ip given");

	m_sockaddr_in.sin_family = AF_INET;
	m_sockaddr_in.sin_port = htons(nPort);
	m_sockaddr_in.sin_addr = *(in_addr*)&ip;

	int nRet;
	nRet = connect(m_hSocket,(sockaddr*)&m_sockaddr_in,sizeof(sockaddr));		//Telnet连接
	if ( nRet == SOCKET_ERROR )											//正常？？？ 
		return -1;														//异常返回
	return 0;															//正常返回
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：Telnet连接
//------------------------------------------------------------------------------------------------------			
SOCKET CTelnet::TelnetConnect(BSCLoginInfo* v_cBSC)
{
// 	CTelnetManager* pDlg=(CTelnetManager*)v_pTelnetManager;
// 
// 	int v_iRet;
// 	
// 	v_iRet = Create();													//生成Socket
// 	if ( v_iRet < 0 ) return NULL;										//返回：无效
// 
// 	v_iRet = Connect(v_cBSC->IP,v_cBSC->Port);							//生成：连接Telnet
// 	if ( v_iRet < 0 ) return NULL;										//返回：无效
// 
// 	v_iIOCP_Num=v_cBSC->v_iNum;											//IOCP工作序号
// 	v_cBSC->v_sSocket=m_hSocket;
// 
// 	//(1)完成客户SOCKET与工作IOCP完成端口绑定，参数1：客户socket；参数2：工作IOCP端口；
// 	if (CreateIoCompletionPort((HANDLE)m_hSocket,						//Socket
// 								pDlg->v_cIocp.m_hWorkerIO,				//工作IOCP完成端口
// 								(DWORD)v_cBSC,							//CompletionKey [完成KEY，获得数据时，返回该值，用于匹配接收数据]
// 								0										//NumberOfConcurrentThreads
// 								)==NULL)								//投递失败？？？
// 	{
// 		closesocket(m_hSocket);											//关闭：Socket
// 		return NULL;													//返回：无效
// 	}
// 	pDlg->v_cIocp.RecvDataRequest(m_hSocket,v_cBSC->v_iNum);			//投递客户端接收信息请求

	return m_hSocket;													//返回：有效Socket
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：关闭Telnet连接
//------------------------------------------------------------------------------------------------------			
void	CTelnet::Close_Telnet()
{
 	if (m_hSocket!=NULL)
 		closesocket(m_hSocket);											//关闭：Socket
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：返回接收信息
//  接收过程：(1) 接收前：等待指令执行结果【最长4000ms】
//			  (2) 接收数据
//------------------------------------------------------------------------------------------------------
CString	CTelnet::Rece()
{
// 	CTelnetManager* pDlg=(CTelnetManager*)v_pTelnetManager;
// 
 	CString		v_sText;												//临时变量
// 	int			v_iNum=0;
// 
// 	//无数据处理：指令执行需要时间　[等待数据到来，最长2000ms]
// 	while (true)
// 	{
// 		if (!pDlg->v_sTelnet_Text[v_iIOCP_Num].IsEmpty())
// 			break;
// 
// 		v_iNum++;														//计数器++
// 		if (v_iNum<100)													//连续100次接收
// 		{
// 			Sleep(20);													///延时
// 			continue;													//继续
// 		}
// 		else															//指令结果不完整
// 			break;														//退出：不完整指令信息
// 	}
// 	
// 	//数据获取
// 	pDlg->v_cTelnet_Text_Lock.Lock();									//加锁
// 	v_sText=pDlg->v_sTelnet_Text[v_iIOCP_Num];							//获得：Telnet信息
// 	pDlg->v_sTelnet_Text[v_iIOCP_Num]="";								//复位：Telnet信息
// 	pDlg->v_cTelnet_Text_Lock.Unlock();									//解锁

	return v_sText;
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：发送指令
//------------------------------------------------------------------------------------------------------
int CTelnet::Send(CString v_sSend,BSCLoginInfo* v_cBSC)
{
// 	CTelnetManager* pDlg=(CTelnetManager*)v_pTelnetManager;
// 
// 	pDlg->v_cIocp.SendDataRequest(m_hSocket,v_cBSC->v_iNum,v_sSend.GetBuffer(0),v_sSend.GetLength());

	return true;
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
