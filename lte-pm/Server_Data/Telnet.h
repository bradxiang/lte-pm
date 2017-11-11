// Telnet.h: interface for the CTelnet class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include  <afxmt.h> 


#if !defined(AFX_SOCKETDX_H__DFEE9DED_9AF1_11D2_A6CE_204C4F4F5020__INCLUDED_)
#define AFX_SOCKETDX_H__DFEE9DED_9AF1_11D2_A6CE_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTelnet  
{
public:
	CTelnet(void);
	~CTelnet(void);

	//------------------------------------------------------------------------------------------------------
	//	函数定义	
	//------------------------------------------------------------------------------------------------------
	int			Create();												//生成Socket
	int			Connect(CString strIP,int nPort);						//连接：设备
	void		Set_TelnetManager(LPVOID lparam);						//设置：TelnetManager指针

	SOCKET		TelnetConnect(BSCLoginInfo* v_cBSC);					//连接：Telnet
	void		Close_Telnet();											//关闭：Telnet
	CString		Rece();													//接收：Telnet
	int			Send(CString v_sSend,BSCLoginInfo* v_cBSC);				//发送：Telnet

	//------------------------------------------------------------------------------------------------------
	//	变量定义
	//------------------------------------------------------------------------------------------------------
private:
	sockaddr_in			m_sockaddr_in;
	SOCKET				m_hSocket;

	LPVOID				v_pTelnetManager;								//TelnetManager指针

	int					v_iIOCP_Num;									//对应IOCP的序号
};

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------

#endif // !defined(AFX_SOCKETDX_H__DFEE9DED_9AF1_11D2_A6CE_204C4F4F5020__INCLUDED_)
